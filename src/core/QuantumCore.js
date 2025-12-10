/**
 * Quantum Engine Core Module
 * @module QuantumCore
 */

import { EventEmitter } from 'events';
import { QuantumConfig } from './QuantumConfig.js';
import { QuantumLogger } from './QuantumLogger.js';
import { QuantumModuleManager } from './QuantumModuleManager.js';
import { QuantumError } from './QuantumError.js';

/**
 * Main Quantum Engine class
 * @class QuantumCore
 */
export class QuantumCore extends EventEmitter {
  /**
   * Create a new Quantum Engine instance
   * @param {Object} config - Engine configuration
   */
  constructor(config = {}) {
    super();
    
    this._config = new QuantumConfig(config);
    this._logger = new QuantumLogger(this._config);
    this._modules = new QuantumModuleManager();
    this._state = 'uninitialized';
    this._version = '1.0.0';
    this._startTime = Date.now();
    this._frameCount = 0;
    
    // Core subsystems
    this._systems = new Map();
    this._entities = new Map();
    this._scenes = new Map();
    this._assets = new Map();
    
    // Performance tracking
    this._performance = {
      fps: 0,
      frameTime: 0,
      memoryUsage: 0,
      drawCalls: 0,
      triangles: 0
    };
    
    this._logger.info(`Quantum Engine v${this._version} initialized`);
  }
  
  /**
   * Initialize the engine
   * @async
   * @returns {Promise<void>}
   */
  async initialize() {
    if (this._state !== 'uninitialized') {
      throw new QuantumError('Engine already initialized');
    }
    
    this._state = 'initializing';
    this.emit('initializing');
    
    try {
      // Load core modules
      await this._modules.loadCoreModules();
      
      // Initialize subsystems in correct order
      await this._initializeSubsystems();
      
      this._state = 'running';
      this.emit('initialized');
      
      this._logger.info('Engine initialized successfully');
      
      // Start main loop if requested
      if (this._config.get('autoStart', true)) {
        this.start();
      }
      
    } catch (error) {
      this._state = 'error';
      this.emit('initializationError', error);
      this._logger.error('Engine initialization failed:', error);
      throw error;
    }
  }
  
  /**
   * Start the engine main loop
   */
  start() {
    if (this._state !== 'running') {
      throw new QuantumError('Engine must be initialized before starting');
    }
    
    if (this._animationFrameId) {
      this._logger.warn('Engine already running');
      return;
    }
    
    this._state = 'running';
    this._lastFrameTime = performance.now();
    
    // Start main loop
    const mainLoop = (currentTime) => {
      this._animationFrameId = requestAnimationFrame(mainLoop);
      
      // Calculate delta time
      const deltaTime = (currentTime - this._lastFrameTime) / 1000;
      this._lastFrameTime = currentTime;
      
      // Clamp delta time to prevent spiral of death
      const clampedDeltaTime = Math.min(deltaTime, 0.1);
      
      // Update performance metrics
      this._updatePerformanceMetrics(clampedDeltaTime);
      
      // Update all systems
      this._update(clampedDeltaTime);
      
      // Render frame
      this._render();
      
      this._frameCount++;
      
      // Emit frame event
      this.emit('frame', {
        deltaTime: clampedDeltaTime,
        frameCount: this._frameCount,
        timestamp: currentTime
      });
    };
    
    this._animationFrameId = requestAnimationFrame(mainLoop);
    this.emit('started');
    this._logger.info('Engine main loop started');
  }
  
  /**
   * Stop the engine
   */
  stop() {
    if (this._animationFrameId) {
      cancelAnimationFrame(this._animationFrameId);
      this._animationFrameId = null;
    }
    
    this._state = 'stopped';
    this.emit('stopped');
    this._logger.info('Engine stopped');
  }
  
  /**
   * Get engine version
   * @returns {string} Version string
   */
  getVersion() {
    return this._version;
  }
  
  /**
   * Get engine state
   * @returns {string} Current state
   */
  getState() {
    return this._state;
  }
  
  /**
   * Get configuration
   * @returns {QuantumConfig} Configuration object
   */
  getConfig() {
    return this._config;
  }
  
  /**
   * Get logger
   * @returns {QuantumLogger} Logger instance
   */
  getLogger() {
    return this._logger;
  }
  
  /**
   * Get module by name
   * @param {string} moduleName - Module name
   * @returns {Object|null} Module instance
   */
  getModule(moduleName) {
    return this._modules.get(moduleName);
  }
  
  /**
   * Register a custom module
   * @param {string} name - Module name
   * @param {Object} module - Module instance
   */
  registerModule(name, module) {
    this._modules.register(name, module);
    this._logger.debug(`Module registered: ${name}`);
  }
  
  /**
   * Get performance metrics
   * @returns {Object} Performance data
   */
  getPerformance() {
    return { ...this._performance };
  }
  
  /**
   * Get uptime in seconds
   * @returns {number} Uptime in seconds
   */
  getUptime() {
    return (Date.now() - this._startTime) / 1000;
  }
  
  /**
   * Get frame count
   * @returns {number} Total frames rendered
   */
  getFrameCount() {
    return this._frameCount;
  }
  
  /**
   * Destroy the engine and cleanup resources
   * @async
   */
  async destroy() {
    this.stop();
    
    // Cleanup subsystems in reverse order
    const systemNames = Array.from(this._systems.keys()).reverse();
    for (const name of systemNames) {
      const system = this._systems.get(name);
      if (system && typeof system.destroy === 'function') {
        await system.destroy();
      }
    }
    
    // Clear all collections
    this._systems.clear();
    this._entities.clear();
    this._scenes.clear();
    this._assets.clear();
    
    this._state = 'destroyed';
    this.emit('destroyed');
    this._logger.info('Engine destroyed');
  }
  
  /**
   * Initialize all subsystems
   * @private
   * @async
   */
  async _initializeSubsystems() {
    const initializationOrder = [
      'memory',
      'events',
      'assets',
      'scene',
      'rendering',
      'physics',
      'audio',
      'input',
      'network',
      'ai',
      'ui',
      'scripting'
    ];
    
    for (const systemName of initializationOrder) {
      if (this._config.get(`systems.${systemName}.enabled`, true)) {
        await this._initializeSystem(systemName);
      }
    }
  }
  
  /**
   * Initialize a single subsystem
   * @private
   * @async
   * @param {string} systemName - System name
   */
  async _initializeSystem(systemName) {
    try {
      const SystemClass = await this._loadSystemClass(systemName);
      const system = new SystemClass(this);
      
      if (typeof system.initialize === 'function') {
        await system.initialize();
      }
      
      this._systems.set(systemName, system);
      this._logger.debug(`System initialized: ${systemName}`);
      
    } catch (error) {
      this._logger.error(`Failed to initialize system ${systemName}:`, error);
      throw error;
    }
  }
  
  /**
   * Update all systems
   * @private
   * @param {number} deltaTime - Time since last frame
   */
  _update(deltaTime) {
    // Update order is important
    const updateOrder = [
      'input',
      'scripting',
      'ai',
      'physics',
      'scene',
      'audio',
      'network',
      'ui'
    ];
    
    for (const systemName of updateOrder) {
      const system = this._systems.get(systemName);
      if (system && typeof system.update === 'function') {
        const startTime = performance.now();
        system.update(deltaTime);
        const endTime = performance.now();
        
        // Track system performance
        if (!this._performance.systemTimes) {
          this._performance.systemTimes = {};
        }
        this._performance.systemTimes[systemName] = endTime - startTime;
      }
    }
    
    // Emit update event
    this.emit('update', { deltaTime });
  }
  
  /**
   * Render frame
   * @private
   */
  _render() {
    const renderSystem = this._systems.get('rendering');
    if (renderSystem && typeof renderSystem.render === 'function') {
      const startTime = performance.now();
      renderSystem.render();
      const renderTime = performance.now() - startTime;
      
      this._performance.renderTime = renderTime;
      this.emit('render', { renderTime });
    }
  }
  
  /**
   * Update performance metrics
   * @private
   * @param {number} deltaTime - Time since last frame
   */
  _updatePerformanceMetrics(deltaTime) {
    // Calculate FPS
    if (deltaTime > 0) {
      this._performance.fps = Math.min(1 / deltaTime, 120);
    }
    
    this._performance.frameTime = deltaTime * 1000; // Convert to ms
    
    // Memory usage (if available)
    if (performance.memory) {
      this._performance.memoryUsage = performance.memory.usedJSHeapSize;
    }
    
    // Emit performance event every 60 frames
    if (this._frameCount % 60 === 0) {
      this.emit('performance', this._performance);
    }
  }
  
  /**
   * Load system class dynamically
   * @private
   * @async
   * @param {string} systemName - System name
   * @returns {Promise<Class>} System class
   */
  async _loadSystemClass(systemName) {
    // This would typically use dynamic imports
    const systemMap = {
      'scene': (await import('../scene/SceneManager.js')).SceneManager,
      'rendering': (await import('../rendering/QuantumRenderPipeline.js')).QuantumRenderPipeline,
      'physics': (await import('../physics/PhysicsSimulator.js')).PhysicsSimulator,
      'input': (await import('../input/InputController.js')).InputController,
      'audio': (await import('../audio/AudioSystem.js')).AudioSystem,
      'network': (await import('../network/NetworkCoordinator.js')).NetworkCoordinator,
      'ui': (await import('../ui/UICanvas.js')).UICanvas,
      'ai': (await import('../ai/AIBehaviorController.js')).AIBehaviorController,
      'scripting': (await import('../scripting/QuantumScriptEngine.js')).QuantumScriptEngine,
      'assets': (await import('../assets/QuantumAssetManager.js')).QuantumAssetManager,
      'memory': (await import('../utilities/memory/QuantumMemoryManager.js')).QuantumMemoryManager,
      'events': (await import('../utilities/events/QuantumEventDispatcher.js')).QuantumEventDispatcher
    };
    
    return systemMap[systemName] || null;
  }
}

// Export singleton instance
export let quantumEngine = null;

/**
 * Create and initialize Quantum Engine
 * @param {Object} config - Engine configuration
 * @returns {Promise<QuantumCore>} Engine instance
 */
export async function createQuantumEngine(config = {}) {
  if (quantumEngine) {
    console.warn('Quantum Engine already created. Returning existing instance.');
    return quantumEngine;
  }
  
  quantumEngine = new QuantumCore(config);
  await quantumEngine.initialize();
  
  return quantumEngine;
}

/**
 * Get the global Quantum Engine instance
 * @returns {QuantumCore|null} Engine instance
 */
export function getQuantumEngine() {
  return quantumEngine;
}

// Default export
export default QuantumCore;
