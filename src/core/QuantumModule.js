/**
 * Quantum Engine Module System
 * @module QuantumModule
 */

import { QuantumError } from './QuantumError.js';
import { QuantumLogger } from './QuantumLogger.js';

/**
 * Module lifecycle states
 * @enum {string}
 */
export const ModuleState = {
  UNLOADED: 'unloaded',
  LOADING: 'loading',
  LOADED: 'loaded',
  INITIALIZING: 'initializing',
  INITIALIZED: 'initialized',
  STARTING: 'starting',
  RUNNING: 'running',
  STOPPING: 'stopping',
  STOPPED: 'stopped',
  DESTROYING: 'destroying',
  DESTROYED: 'destroyed',
  ERROR: 'error'
};

/**
 * Module dependency information
 * @typedef {Object} ModuleDependency
 * @property {string} name - Module name
 * @property {string} version - Required version
 * @property {boolean} optional - Whether dependency is optional
 */

/**
 * Module metadata
 * @typedef {Object} ModuleMetadata
 * @property {string} name - Module name
 * @property {string} version - Module version
 * @property {string} description - Module description
 * @property {string[]} authors - Module authors
 * @property {string} license - Module license
 * @property {ModuleDependency[]} dependencies - Module dependencies
 * @property {string[]} tags - Module tags
 */

/**
 * Base Quantum Module class
 * @class QuantumModule
 */
export class QuantumModule {
  /**
   * Create a new module
   * @param {string} name - Module name
   * @param {QuantumEngine} engine - Engine instance
   * @param {Object} [options] - Module options
   */
  constructor(name, engine, options = {}) {
    if (new.target === QuantumModule) {
      throw new QuantumError('QuantumModule is an abstract class and cannot be instantiated directly');
    }
    
    this._name = name;
    this._engine = engine;
    this._options = options;
    this._state = ModuleState.UNLOADED;
    this._metadata = this._getDefaultMetadata();
    this._dependencies = new Map();
    this._dependents = new Set();
    this._resources = new Map();
    this._eventListeners = new Map();
    this._services = new Map();
    
    // Performance tracking
    this._performance = {
      loadTime: 0,
      initializeTime: 0,
      startTime: 0,
      updateTime: 0,
      memoryUsage: 0
    };
    
    // Initialize logger
    this._logger = new QuantumLogger(engine.getConfig());
    
    // Bind methods
    this._handleEngineEvent = this._handleEngineEvent.bind(this);
  }
  
  /**
   * Get module name
   * @returns {string} Module name
   */
  getName() {
    return this._name;
  }
  
  /**
   * Get module version
   * @returns {string} Module version
   */
  getVersion() {
    return this._metadata.version;
  }
  
  /**
   * Get module state
   * @returns {ModuleState} Current state
   */
  getState() {
    return this._state;
  }
  
  /**
   * Get module metadata
   * @returns {ModuleMetadata} Module metadata
   */
  getMetadata() {
    return { ...this._metadata };
  }
  
  /**
   * Get module dependencies
   * @returns {Map<string, QuantumModule>} Module dependencies
   */
  getDependencies() {
    return new Map(this._dependencies);
  }
  
  /**
   * Get module dependents
   * @returns {Set<QuantumModule>} Modules that depend on this module
   */
  getDependents() {
    return new Set(this._dependents);
  }
  
  /**
   * Get module options
   * @returns {Object} Module options
   */
  getOptions() {
    return { ...this._options };
  }
  
  /**
   * Get module performance data
   * @returns {Object} Performance data
   */
  getPerformance() {
    return { ...this._performance };
  }
  
  /**
   * Load module resources
   * @async
   * @returns {Promise<void>}
   */
  async load() {
    if (this._state !== ModuleState.UNLOADED) {
      throw new QuantumError(`Module ${this._name} is not in UNLOADED state`, 'MODULE_STATE_ERROR', {
        currentState: this._state,
        expectedState: ModuleState.UNLOADED
      });
    }
    
    this._state = ModuleState.LOADING;
    this._logger.info(`Loading module: ${this._name}`);
    
    const startTime = performance.now();
    
    try {
      // Check dependencies
      await this._checkDependencies();
      
      // Load module resources
      await this._loadResources();
      
      // Call module-specific load implementation
      if (typeof this.onLoad === 'function') {
        await this.onLoad();
      }
      
      this._state = ModuleState.LOADED;
      this._performance.loadTime = performance.now() - startTime;
      
      this._logger.info(`Module loaded: ${this._name} (${this._performance.loadTime.toFixed(2)}ms)`);
      this._engine.emit('moduleLoaded', { module: this });
      
    } catch (error) {
      this._state = ModuleState.ERROR;
      this._logger.error(`Failed to load module ${this._name}:`, error);
      throw error;
    }
  }
  
  /**
   * Initialize module
   * @async
   * @returns {Promise<void>}
   */
  async initialize() {
    if (this._state !== ModuleState.LOADED) {
      throw new QuantumError(`Module ${this._name} is not in LOADED state`, 'MODULE_STATE_ERROR', {
        currentState: this._state,
        expectedState: ModuleState.LOADED
      });
    }
    
    this._state = ModuleState.INITIALIZING;
    this._logger.info(`Initializing module: ${this._name}`);
    
    const startTime = performance.now();
    
    try {
      // Initialize dependencies first
      for (const [name, dependency] of this._dependencies) {
        if (dependency.getState() === ModuleState.LOADED) {
          await dependency.initialize();
        }
      }
      
      // Initialize module services
      await this._initializeServices();
      
      // Call module-specific initialize implementation
      if (typeof this.onInitialize === 'function') {
        await this.onInitialize();
      }
      
      this._state = ModuleState.INITIALIZED;
      this._performance.initializeTime = performance.now() - startTime;
      
      this._logger.info(`Module initialized: ${this._name} (${this._performance.initializeTime.toFixed(2)}ms)`);
      this._engine.emit('moduleInitialized', { module: this });
      
    } catch (error) {
      this._state = ModuleState.ERROR;
      this._logger.error(`Failed to initialize module ${this._name}:`, error);
      throw error;
    }
  }
  
  /**
   * Start module
   * @async
   * @returns {Promise<void>}
   */
  async start() {
    if (this._state !== ModuleState.INITIALIZED) {
      throw new QuantumError(`Module ${this._name} is not in INITIALIZED state`, 'MODULE_STATE_ERROR', {
        currentState: this._state,
        expectedState: ModuleState.INITIALIZED
      });
    }
    
    this._state = ModuleState.STARTING;
    this._logger.info(`Starting module: ${this._name}`);
    
    const startTime = performance.now();
    
    try {
      // Start dependencies first
      for (const [name, dependency] of this._dependencies) {
        if (dependency.getState() === ModuleState.INITIALIZED) {
          await dependency.start();
        }
      }
      
      // Register engine event listeners
      this._registerEngineListeners();
      
      // Call module-specific start implementation
      if (typeof this.onStart === 'function') {
        await this.onStart();
      }
      
      this._state = ModuleState.RUNNING;
      this._performance.startTime = performance.now() - startTime;
      
      this._logger.info(`Module started: ${this._name} (${this._performance.startTime.toFixed(2)}ms)`);
      this._engine.emit('moduleStarted', { module: this });
      
    } catch (error) {
      this._state = ModuleState.ERROR;
      this._logger.error(`Failed to start module ${this._name}:`, error);
      throw error;
    }
  }
  
  /**
   * Stop module
   * @async
   * @returns {Promise<void>}
   */
  async stop() {
    if (this._state !== ModuleState.RUNNING) {
      throw new QuantumError(`Module ${this._name} is not in RUNNING state`, 'MODULE_STATE_ERROR', {
        currentState: this._state,
        expectedState: ModuleState.RUNNING
      });
    }
    
    this._state = ModuleState.STOPPING;
    this._logger.info(`Stopping module: ${this._name}`);
    
    try {
      // Call module-specific stop implementation
      if (typeof this.onStop === 'function') {
        await this.onStop();
      }
      
      // Unregister engine event listeners
      this._unregisterEngineListeners();
      
      // Stop dependents first (reverse order)
      const dependents = Array.from(this._dependents);
      for (let i = dependents.length - 1; i >= 0; i--) {
        const dependent = dependents[i];
        if (dependent.getState() === ModuleState.RUNNING) {
          await dependent.stop();
        }
      }
      
      this._state = ModuleState.STOPPED;
      
      this._logger.info(`Module stopped: ${this._name}`);
      this._engine.emit('moduleStopped', { module: this });
      
    } catch (error) {
      this._state = ModuleState.ERROR;
      this._logger.error(`Failed to stop module ${this._name}:`, error);
      throw error;
    }
  }
  
  /**
   * Update module (called every frame)
   * @param {number} deltaTime - Time since last update in seconds
   */
  update(deltaTime) {
    if (this._state !== ModuleState.RUNNING) {
      return;
    }
    
    const startTime = performance.now();
    
    try {
      // Call module-specific update implementation
      if (typeof this.onUpdate === 'function') {
        this.onUpdate(deltaTime);
      }
      
      // Update module services
      for (const service of this._services.values()) {
        if (typeof service.update === 'function') {
          service.update(deltaTime);
        }
      }
      
      this._performance.updateTime = performance.now() - startTime;
      
    } catch (error) {
      this._logger.error(`Error updating module ${this._name}:`, error);
      this._engine.emit('moduleUpdateError', { module: this, error });
    }
  }
  
  /**
   * Destroy module and cleanup resources
   * @async
   * @returns {Promise<void>}
   */
  async destroy() {
    if (this._state === ModuleState.DESTROYED || this._state === ModuleState.DESTROYING) {
      return;
    }
    
    // Stop if running
    if (this._state === ModuleState.RUNNING) {
      await this.stop();
    }
    
    this._state = ModuleState.DESTROYING;
    this._logger.info(`Destroying module: ${this._name}`);
    
    try {
      // Call module-specific destroy implementation
      if (typeof this.onDestroy === 'function') {
        await this.onDestroy();
      }
      
      // Destroy dependencies in reverse order
      const dependencies = Array.from(this._dependencies.keys()).reverse();
      for (const depName of dependencies) {
        const dependency = this._dependencies.get(depName);
        if (dependency) {
          dependency._removeDependent(this);
        }
      }
      
      // Clear resources
      this._clearResources();
      
      // Clear services
      this._services.clear();
      
      // Clear event listeners
      this._eventListeners.clear();
      
      this._state = ModuleState.DESTROYED;
      
      this._logger.info(`Module destroyed: ${this._name}`);
      this._engine.emit('moduleDestroyed', { module: this });
      
    } catch (error) {
      this._state = ModuleState.ERROR;
      this._logger.error(`Failed to destroy module ${this._name}:`, error);
      throw error;
    }
  }
  
  /**
   * Add dependency to module
   * @param {QuantumModule} dependency - Dependency module
   */
  addDependency(dependency) {
    if (this._dependencies.has(dependency.getName())) {
      this._logger.warn(`Dependency ${dependency.getName()} already added`);
      return;
    }
    
    this._dependencies.set(dependency.getName(), dependency);
    dependency._addDependent(this);
    
    this._logger.debug(`Added dependency: ${dependency.getName()} -> ${this._name}`);
  }
  
  /**
   * Remove dependency from module
   * @param {string} dependencyName - Dependency name
   */
  removeDependency(dependencyName) {
    const dependency = this._dependencies.get(dependencyName);
    if (dependency) {
      dependency._removeDependent(this);
      this._dependencies.delete(dependencyName);
      
      this._logger.debug(`Removed dependency: ${dependencyName} -> ${this._name}`);
    }
  }
  
  /**
   * Check if module has dependency
   * @param {string} dependencyName - Dependency name
   * @returns {boolean} True if module has dependency
   */
  hasDependency(dependencyName) {
    return this._dependencies.has(dependencyName);
  }
  
  /**
   * Get service by name
   * @param {string} serviceName - Service name
   * @returns {Object|null} Service instance
   */
  getService(serviceName) {
    return this._services.get(serviceName) || null;
  }
  
  /**
   * Register service
   * @param {string} serviceName - Service name
   * @param {Object} service - Service instance
   */
  registerService(serviceName, service) {
    if (this._services.has(serviceName)) {
      this._logger.warn(`Service ${serviceName} already registered, overwriting`);
    }
    
    this._services.set(serviceName, service);
    this._logger.debug(`Service registered: ${serviceName}`);
  }
  
  /**
   * Unregister service
   * @param {string} serviceName - Service name
   */
  unregisterService(serviceName) {
    const service = this._services.get(serviceName);
    if (service && typeof service.destroy === 'function') {
      service.destroy();
    }
    
    this._services.delete(serviceName);
    this._logger.debug(`Service unregistered: ${serviceName}`);
  }
  
  /**
   * Add event listener
   * @param {string} eventName - Event name
   * @param {Function} listener - Event listener
   * @returns {Function} Remove listener function
   */
  addEventListener(eventName, listener) {
    if (!this._eventListeners.has(eventName)) {
      this._eventListeners.set(eventName, []);
    }
    
    const listeners = this._eventListeners.get(eventName);
    listeners.push(listener);
    
    this._logger.debug(`Event listener added: ${eventName}`);
    
    return () => {
      this.removeEventListener(eventName, listener);
    };
  }
  
  /**
   * Remove event listener
   * @param {string} eventName - Event name
   * @param {Function} listener - Event listener to remove
   */
  removeEventListener(eventName, listener) {
    const listeners = this._eventListeners.get(eventName);
    if (listeners) {
      const index = listeners.indexOf(listener);
      if (index > -1) {
        listeners.splice(index, 1);
        this._logger.debug(`Event listener removed: ${eventName}`);
      }
    }
  }
  
  /**
   * Emit event to module listeners
   * @param {string} eventName - Event name
   * @param {Object} data - Event data
   */
  emit(eventName, data = {}) {
    const listeners = this._eventListeners.get(eventName);
    if (listeners) {
      for (const listener of listeners) {
        try {
          listener({ ...data, module: this._name, event: eventName });
        } catch (error) {
          this._logger.error(`Error in event listener for ${eventName}:`, error);
        }
      }
    }
    
    // Also emit to engine for global listeners
    this._engine.emit(`module:${this._name}:${eventName}`, data);
  }
  
  /**
   * Lifecycle methods (to be overridden by subclasses)
   */
  
  /**
   * Called when module is loading
   * @virtual
   * @async
   * @returns {Promise<void>}
   */
  async onLoad() {
    // Override in subclass
  }
  
  /**
   * Called when module is initializing
   * @virtual
   * @async
   * @returns {Promise<void>}
   */
  async onInitialize() {
    // Override in subclass
  }
  
  /**
   * Called when module is starting
   * @virtual
   * @async
   * @returns {Promise<void>}
   */
  async onStart() {
    // Override in subclass
  }
  
  /**
   * Called when module is stopping
   * @virtual
   * @async
   * @returns {Promise<void>}
   */
  async onStop() {
    // Override in subclass
  }
  
  /**
   * Called every frame when module is running
   * @virtual
   * @param {number} deltaTime - Time since last frame
   */
  onUpdate(deltaTime) {
    // Override in subclass
  }
  
  /**
   * Called when module is destroying
   * @virtual
   * @async
   * @returns {Promise<void>}
   */
  async onDestroy() {
    // Override in subclass
  }
  
  // Internal methods
  
  /**
   * Add dependent module
   * @private
   * @param {QuantumModule} dependent - Dependent module
   */
  _addDependent(dependent) {
    this._dependents.add(dependent);
  }
  
  /**
   * Remove dependent module
   * @private
   * @param {QuantumModule} dependent - Dependent module
   */
  _removeDependent(dependent) {
    this._dependents.delete(dependent);
  }
  
  /**
   * Get default metadata
   * @private
   * @returns {ModuleMetadata} Default metadata
   */
  _getDefaultMetadata() {
    return {
      name: this._name,
      version: '1.0.0',
      description: 'Quantum Engine Module',
      authors: ['Quantum Engine Team'],
      license: 'MIT',
      dependencies: [],
      tags: []
    };
  }
  
  /**
   * Check module dependencies
   * @private
   * @async
   */
  async _checkDependencies() {
    for (const dependency of this._metadata.dependencies) {
      if (!this._dependencies.has(dependency.name)) {
        throw new QuantumError(
          `Missing required dependency: ${dependency.name}`,
          'MODULE_DEPENDENCY_ERROR',
          { dependency }
        );
      }
    }
  }
  
  /**
   * Load module resources
   * @private
   * @async
   */
  async _loadResources() {
    // Default implementation does nothing
    // Override in subclass to load resources
  }
  
  /**
   * Initialize module services
   * @private
   * @async
   */
  async _initializeServices() {
    // Initialize all registered services
    for (const [name, service] of this._services) {
      if (typeof service.initialize === 'function') {
        await service.initialize();
      }
    }
  }
  
  /**
   * Clear module resources
   * @private
   */
  _clearResources() {
    for (const [key, resource] of this._resources) {
      if (typeof resource.destroy === 'function') {
        resource.destroy();
      }
    }
    this._resources.clear();
  }
  
  /**
   * Register engine event listeners
   * @private
   */
  _registerEngineListeners() {
    // Listen to engine events
    this._engine.on('update', this._handleEngineEvent);
    this._engine.on('render', this._handleEngineEvent);
    this._engine.on('error', this._handleEngineEvent);
  }
  
  /**
   * Unregister engine event listeners
   * @private
   */
  _unregisterEngineListeners() {
    this._engine.off('update', this._handleEngineEvent);
    this._engine.off('render', this._handleEngineEvent);
    this._engine.off('error', this._handleEngineEvent);
  }
  
  /**
   * Handle engine events
   * @private
   * @param {Object} event - Engine event
   */
  _handleEngineEvent(event) {
    // Forward engine events to module
    this.emit(`engine:${event.type}`, event);
  }
}

/**
 * Module Manager
 * @class QuantumModuleManager
 */
export class QuantumModuleManager {
  constructor() {
    this._modules = new Map();
    this._moduleFactories = new Map();
    this._loadingPromises = new Map();
    this._dependencyGraph = new Map();
  }
  
  /**
   * Register module factory
   * @param {string} moduleType - Module type identifier
   * @param {Function} factory - Module factory function
   */
  registerFactory(moduleType, factory) {
    if (typeof factory !== 'function') {
      throw new QuantumError('Module factory must be a function', 'MODULE_FACTORY_ERROR');
    }
    
    this._moduleFactories.set(moduleType, factory);
  }
  
  /**
   * Create and register module
   * @param {string} moduleType - Module type
   * @param {string} moduleName - Module name
   * @param {QuantumEngine} engine - Engine instance
   * @param {Object} options - Module options
   * @returns {Promise<QuantumModule>} Created module
   */
  async createModule(moduleType, moduleName, engine, options = {}) {
    if (this._modules.has(moduleName)) {
      throw new QuantumError(`Module ${moduleName} already exists`, 'MODULE_EXISTS_ERROR');
    }
    
    const factory = this._moduleFactories.get(moduleType);
    if (!factory) {
      throw new QuantumError(`No factory registered for module type: ${moduleType}`, 'MODULE_FACTORY_NOT_FOUND');
    }
    
    // Create module instance
    const module = factory(moduleName, engine, options);
    
    if (!(module instanceof QuantumModule)) {
      throw new QuantumError('Module factory must return a QuantumModule instance', 'INVALID_MODULE');
    }
    
    // Register module
    this._modules.set(moduleName, module);
    this._dependencyGraph.set(moduleName, new Set());
    
    return module;
  }
  
  /**
   * Get module by name
   * @param {string} moduleName - Module name
   * @returns {QuantumModule|null} Module instance
   */
  getModule(moduleName) {
    return this._modules.get(moduleName) || null;
  }
  
  /**
   * Check if module exists
   * @param {string} moduleName - Module name
   * @returns {boolean} True if module exists
   */
  hasModule(moduleName) {
    return this._modules.has(moduleName);
  }
  
  /**
   * Get all modules
   * @returns {QuantumModule[]} Array of all modules
   */
  getAllModules() {
    return Array.from(this._modules.values());
  }
  
  /**
   * Get modules by state
   * @param {ModuleState} state - Module state to filter by
   * @returns {QuantumModule[]} Filtered modules
   */
  getModulesByState(state) {
    return this.getAllModules().filter(module => module.getState() === state);
  }
  
  /**
   * Load core modules
   * @async
   * @returns {Promise<void>}
   */
  async loadCoreModules() {
    const coreModules = [
      'scene',
      'rendering',
      'physics',
      'input',
      'audio',
      'network',
      'ui',
      'ai',
      'scripting',
      'assets'
    ];
    
    for (const moduleType of coreModules) {
      try {
        await this.loadModule(moduleType, moduleType);
      } catch (error) {
        console.warn(`Failed to load core module ${moduleType}:`, error);
      }
    }
  }
  
  /**
   * Load module with dependencies
   * @async
   * @param {string} moduleType - Module type
   * @param {string} moduleName - Module name
   * @param {Object} options - Module options
   * @returns {Promise<QuantumModule>} Loaded module
   */
  async loadModule(moduleType, moduleName, options = {}) {
    // Check if already loading
    if (this._loadingPromises.has(moduleName)) {
      return this._loadingPromises.get(moduleName);
    }
    
    // Create loading promise
    const loadingPromise = this._loadModuleInternal(moduleType, moduleName, options);
    this._loadingPromises.set(moduleName, loadingPromise);
    
    try {
      const module = await loadingPromise;
      this._loadingPromises.delete(moduleName);
      return module;
    } catch (error) {
      this._loadingPromises.delete(moduleName);
      throw error;
    }
  }
  
  /**
   * Unload module
   * @async
   * @param {string} moduleName - Module name
   * @returns {Promise<void>}
   */
  async unloadModule(moduleName) {
    const module = this._modules.get(moduleName);
    if (!module) {
      throw new QuantumError(`Module ${moduleName} not found`, 'MODULE_NOT_FOUND');
    }
    
    // Check if other modules depend on this module
    const dependents = module.getDependents();
    if (dependents.size > 0) {
      const dependentNames = Array.from(dependents).map(dep => dep.getName());
      throw new QuantumError(
        `Cannot unload module ${moduleName} because other modules depend on it`,
        'MODULE_DEPENDENCY_ERROR',
        { dependents: dependentNames }
      );
    }
    
    // Destroy module
    await module.destroy();
    
    // Remove from collections
    this._modules.delete(moduleName);
    this._dependencyGraph.delete(moduleName);
    
    // Remove dependencies
    for (const [depName, dependencies] of this._dependencyGraph) {
      dependencies.delete(moduleName);
    }
  }
  
  /**
   * Update all running modules
   * @param {number} deltaTime - Time since last update
   */
  updateModules(deltaTime) {
    const runningModules = this.getModulesByState(ModuleState.RUNNING);
    
    // Sort by update priority (could be configurable)
    const sortedModules = this._sortModulesForUpdate(runningModules);
    
    for (const module of sortedModules) {
      module.update(deltaTime);
    }
  }
  
  /**
   * Destroy all modules
   * @async
   */
  async destroyAllModules() {
    const modules = this.getAllModules();
    
    // Destroy in reverse dependency order
    const sortedModules = this._getTopologicalSort(true);
    
    for (const moduleName of sortedModules) {
      const module = this._modules.get(moduleName);
      if (module) {
        try {
          await module.destroy();
        } catch (error) {
          console.error(`Error destroying module ${moduleName}:`, error);
        }
      }
    }
    
    this._modules.clear();
    this._dependencyGraph.clear();
    this._loadingPromises.clear();
  }
  
  /**
   * Internal module loading
   * @private
   * @async
   */
  async _loadModuleInternal(moduleType, moduleName, options) {
    // Create module
    const module = await this.createModule(moduleType, moduleName, options.engine, options);
    
    // Resolve dependencies
    await this._resolveDependencies(module);
    
    // Load module
    await module.load();
    
    // Initialize module
    await module.initialize();
    
    // Start module
    await module.start();
    
    return module;
  }
  
  /**
   * Resolve module dependencies
   * @private
   * @async
   * @param {QuantumModule} module - Module to resolve dependencies for
   */
  async _resolveDependencies(module) {
    const metadata = module.getMetadata();
    
    for (const depInfo of metadata.dependencies) {
      let dependency = this._modules.get(depInfo.name);
      
      // Create dependency if it doesn't exist
      if (!dependency) {
        // Try to auto-create core modules
        if (depInfo.name === 'scene' || depInfo.name === 'rendering' || depInfo.name === 'physics') {
          dependency = await this.createModule(depInfo.name, depInfo.name, module._engine, {});
        } else if (!depInfo.optional) {
          throw new QuantumError(
            `Required dependency ${depInfo.name} not found`,
            'DEPENDENCY_NOT_FOUND',
            { dependency: depInfo }
          );
        }
      }
      
      if (dependency) {
        module.addDependency(dependency);
        this._addDependency(module.getName(), dependency.getName());
      }
    }
  }
  
  /**
   * Add dependency to graph
   * @private
   * @param {string} moduleName - Module name
   * @param {string} dependencyName - Dependency name
   */
  _addDependency(moduleName, dependencyName) {
    if (!this._dependencyGraph.has(moduleName)) {
      this._dependencyGraph.set(moduleName, new Set());
    }
    this._dependencyGraph.get(moduleName).add(dependencyName);
  }
  
  /**
   * Sort modules for update
   * @private
   * @param {QuantumModule[]} modules - Modules to sort
   * @returns {QuantumModule[]} Sorted modules
   */
  _sortModulesForUpdate(modules) {
    // Simple dependency-based sorting
    // Modules with fewer dependencies update first
    return modules.sort((a, b) => {
      const depsA = this._dependencyGraph.get(a.getName())?.size || 0;
      const depsB = this._dependencyGraph.get(b.getName())?.size || 0;
      return depsA - depsB;
    });
  }
  
  /**
   * Get topological sort of modules
   * @private
   * @param {boolean} reverse - Whether to reverse the order
   * @returns {string[]} Sorted module names
   */
  _getTopologicalSort(reverse = false) {
    const visited = new Set();
    const stack = [];
    
    const visit = (moduleName) => {
      if (visited.has(moduleName)) return;
      visited.add(moduleName);
      
      const dependencies = this._dependencyGraph.get(moduleName) || new Set();
      for (const depName of dependencies) {
        visit(depName);
      }
      
      stack.push(moduleName);
    };
    
    for (const moduleName of this._dependencyGraph.keys()) {
      visit(moduleName);
    }
    
    return reverse ? stack.reverse() : stack;
  }
}

// Default export
export default QuantumModule;
