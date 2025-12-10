/**
 * Quantum Engine Plugin System
 * @module QuantumPluginSystem
 */

import { QuantumError } from './QuantumError.js';
import { QuantumModule } from './QuantumModule.js';
import { QuantumRegistry } from './QuantumRegistry.js';
import { EventEmitter } from 'events';

/**
 * Plugin metadata
 * @typedef {Object} PluginMetadata
 * @property {string} id - Plugin ID
 * @property {string} name - Plugin name
 * @property {string} version - Plugin version
 * @property {string} description - Plugin description
 * @property {string[]} authors - Plugin authors
 * @property {string} license - Plugin license
 * @property {string[]} dependencies - Required plugin IDs
 * @property {string[]} conflicts - Conflicting plugin IDs
 * @property {Object} configSchema - Configuration schema
 * @property {boolean} [enabled=true] - Whether plugin is enabled by default
 */

/**
 * Plugin state
 * @enum {string}
 */
export const PluginState = {
  DISCOVERED: 'discovered',
  LOADED: 'loaded',
  INITIALIZED: 'initialized',
  ENABLED: 'enabled',
  DISABLED: 'disabled',
  ERROR: 'error'
};

/**
 * Quantum Plugin
 * @class QuantumPlugin
 * @extends QuantumModule
 */
export class QuantumPlugin extends QuantumModule {
  /**
   * Create a new plugin
   * @param {string} pluginId - Plugin ID
   * @param {QuantumEngine} engine - Engine instance
   * @param {PluginMetadata} metadata - Plugin metadata
   * @param {Object} [options] - Plugin options
   */
  constructor(pluginId, engine, metadata, options = {}) {
    super(pluginId, engine, options);
    
    this._metadata = this._validateMetadata(metadata);
    this._state = PluginState.DISCOVERED;
    this._config = this._createConfig(options.config || {});
    this._hooks = new Map();
    this._commands = new Map();
    this._extensions = new Map();
    
    // Plugin-specific services
    this._services = new Map();
  }
  
  /**
   * Get plugin metadata
   * @returns {PluginMetadata} Plugin metadata
   */
  getMetadata() {
    return { ...this._metadata };
  }
  
  /**
   * Get plugin configuration
   * @returns {Object} Plugin configuration
   */
  getConfig() {
    return { ...this._config };
  }
  
  /**
   * Get plugin state
   * @returns {PluginState} Plugin state
   */
  getPluginState() {
    return this._state;
  }
  
  /**
   * Enable plugin
   * @async
   */
  async enable() {
    if (this._state === PluginState.ENABLED) {
      return;
    }
    
    try {
      await this._enableHooks();
      this._state = PluginState.ENABLED;
      
      this._engine.emit('pluginEnabled', { plugin: this });
      this._logger.info(`Plugin enabled: ${this._metadata.name}`);
      
    } catch (error) {
      this._state = PluginState.ERROR;
      this._logger.error(`Failed to enable plugin ${this._metadata.name}:`, error);
      throw error;
    }
  }
  
  /**
   * Disable plugin
   * @async
   */
  async disable() {
    if (this._state === PluginState.DISABLED) {
      return;
    }
    
    try {
      await this._disableHooks();
      this._state = PluginState.DISABLED;
      
      this._engine.emit('pluginDisabled', { plugin: this });
      this._logger.info(`Plugin disabled: ${this._metadata.name}`);
      
    } catch (error) {
      this._state = PluginState.ERROR;
      this._logger.error(`Failed to disable plugin ${this._metadata.name}:`, error);
      throw error;
    }
  }
  
  /**
   * Register a hook
   * @param {string} hookName - Hook name
   * @param {Function} handler - Hook handler
   * @param {number} [priority=10] - Hook priority
   */
  registerHook(hookName, handler, priority = 10) {
    if (!this._hooks.has(hookName)) {
      this._hooks.set(hookName, []);
    }
    
    const hooks = this._hooks.get(hookName);
    hooks.push({ handler, priority });
    
    // Sort by priority
    hooks.sort((a, b) => a.priority - b.priority);
    
    this._logger.debug(`Hook registered: ${hookName} (priority: ${priority})`);
  }
  
  /**
   * Execute hooks
   * @param {string} hookName - Hook name
   * @param {...any} args - Hook arguments
   * @returns {Promise<any>} Hook result
   */
  async executeHooks(hookName, ...args) {
    const hooks = this._hooks.get(hookName) || [];
    let result = null;
    
    for (const hook of hooks) {
      try {
        result = await hook.handler(...args, result);
      } catch (error) {
        this._logger.error(`Error in hook ${hookName}:`, error);
        throw error;
      }
    }
    
    return result;
  }
  
  /**
   * Register a command
   * @param {string} command - Command name
   * @param {Function} handler - Command handler
   * @param {string} [description] - Command description
   */
  registerCommand(command, handler, description = '') {
    this._commands.set(command, { handler, description });
    this._logger.debug(`Command registered: ${command}`);
  }
  
  /**
   * Execute command
   * @param {string} command - Command name
   * @param {...any} args - Command arguments
   * @returns {Promise<any>} Command result
   */
  async executeCommand(command, ...args) {
    const cmd = this._commands.get(command);
    if (!cmd) {
      throw new QuantumError(
        `Command '${command}' not found`,
        'COMMAND_NOT_FOUND',
        { plugin: this._metadata.name }
      );
    }
    
    try {
      return await cmd.handler(...args);
    } catch (error) {
      this._logger.error(`Error executing command ${command}:`, error);
      throw error;
    }
  }
  
  /**
   * Register extension
   * @param {string} extensionPoint - Extension point name
   * @param {Object} extension - Extension object
   */
  registerExtension(extensionPoint, extension) {
    if (!this._extensions.has(extensionPoint)) {
      this._extensions.set(extensionPoint, []);
    }
    
    this._extensions.get(extensionPoint).push(extension);
    this._logger.debug(`Extension registered: ${extensionPoint}`);
  }
  
  /**
   * Get extensions for extension point
   * @param {string} extensionPoint - Extension point name
   * @returns {Object[]} Extensions
   */
  getExtensions(extensionPoint) {
    return this._extensions.get(extensionPoint) || [];
  }
  
  // Lifecycle overrides
  
  async onLoad() {
    await super.onLoad();
    this._state = PluginState.LOADED;
    
    // Load plugin resources
    await this._loadPluginResources();
  }
  
  async onInitialize() {
    await super.onInitialize();
    this._state = PluginState.INITIALIZED;
    
    // Initialize plugin services
    await this._initializePluginServices();
    
    // Register default hooks and commands
    this._registerDefaults();
  }
  
  async onStart() {
    await super.onStart();
    
    // Enable if configured
    if (this._metadata.enabled !== false) {
      await this.enable();
    }
  }
  
  async onStop() {
    await this.disable();
    await super.onStop();
  }
  
  async onDestroy() {
    await super.onDestroy();
    
    // Cleanup plugin resources
    this._hooks.clear();
    this._commands.clear();
    this._extensions.clear();
    this._services.clear();
  }
  
  // Internal methods
  
  /**
   * Validate plugin metadata
   * @private
   * @param {PluginMetadata} metadata - Plugin metadata
   * @returns {PluginMetadata} Validated metadata
   */
  _validateMetadata(metadata) {
    const required = ['id', 'name', 'version'];
    
    for (const field of required) {
      if (!metadata[field]) {
        throw new QuantumError(
          `Plugin metadata missing required field: ${field}`,
          'INVALID_PLUGIN_METADATA'
        );
      }
    }
    
    return {
      enabled: true,
      dependencies: [],
      conflicts: [],
      configSchema: {},
      ...metadata
    };
  }
  
  /**
   * Create plugin configuration
   * @private
   * @param {Object} userConfig - User configuration
   * @returns {Object} Plugin configuration
   */
  _createConfig(userConfig) {
    const schema = this._metadata.configSchema || {};
    const defaultConfig = this._extractDefaults(schema);
    
    return this._mergeConfig(defaultConfig, userConfig);
  }
  
  /**
   * Extract defaults from schema
   * @private
   * @param {Object} schema - Configuration schema
   * @returns {Object} Default configuration
   */
  _extractDefaults(schema) {
    const defaults = {};
    
    for (const [key, field] of Object.entries(schema)) {
      if (field.default !== undefined) {
        defaults[key] = field.default;
      } else if (field.type === 'object' && field.properties) {
        defaults[key] = this._extractDefaults(field.properties);
      }
    }
    
    return defaults;
  }
  
  /**
   * Merge configurations
   * @private
   */
  _mergeConfig(defaults, user) {
    const result = { ...defaults };
    
    for (const [key, value] of Object.entries(user)) {
      if (value && typeof value === 'object' && !Array.isArray(value)) {
        result[key] = this._mergeConfig(result[key] || {}, value);
      } else {
        result[key] = value;
      }
    }
    
    return result;
  }
  
  /**
   * Load plugin resources
   * @private
   * @async
   */
  async _loadPluginResources() {
    // Override in plugin implementation
  }
  
  /**
   * Initialize plugin services
   * @private
   * @async
   */
  async _initializePluginServices() {
    // Initialize registered services
    for (const [name, service] of this._services) {
      if (typeof service.initialize === 'function') {
        await service.initialize();
      }
    }
  }
  
  /**
   * Enable plugin hooks
   * @private
   * @async
   */
  async _enableHooks() {
    // Execute enable hooks
    await this.executeHooks('onEnable');
  }
  
  /**
   * Disable plugin hooks
   * @private
   * @async
   */
  async _disableHooks() {
    // Execute disable hooks
    await this.executeHooks('onDisable');
  }
  
  /**
   * Register default hooks and commands
   * @private
   */
  _registerDefaults() {
    // Default help command
    this.registerCommand('help', () => this._getHelpText(), 'Show plugin help');
    
    // Default info command
    this.registerCommand('info', () => this.getMetadata(), 'Show plugin information');
  }
  
  /**
   * Get help text
   * @private
   * @returns {string} Help text
   */
  _getHelpText() {
    const meta = this.getMetadata();
    let help = `Plugin: ${meta.name} v${meta.version}\n`;
    help += `Description: ${meta.description || 'No description'}\n\n`;
    help += 'Available commands:\n';
    
    for (const [cmd, info] of this._commands) {
      help += `  ${cmd} - ${info.description || 'No description'}\n`;
    }
    
    return help;
  }
}

/**
 * Plugin Manager
 * @class QuantumPluginManager
 */
export class QuantumPluginManager extends EventEmitter {
  constructor(engine) {
    super();
    
    this._engine = engine;
    this._plugins = new Map();
    this._pluginRegistry = new QuantumRegistry();
    this._extensionPoints = new Map();
    this._hookRegistry = new Map();
    this._commandRegistry = new Map();
    this._pluginLoaders = new Map();
    
    // Default plugin directories
    this._pluginDirs = [
      './plugins',
      './node_modules',
      '../plugins'
    ];
    
    // Initialize
    this._initialize();
  }
  
  /**
   * Register plugin loader
   * @param {string} type - Plugin type
   * @param {Function} loader - Plugin loader function
   */
  registerLoader(type, loader) {
    if (typeof loader !== 'function') {
      throw new QuantumError('Plugin loader must be a function', 'INVALID_LOADER');
    }
    this._pluginLoaders.set(type, loader);
  }
  
  /**
   * Load plugin
   * @async
   * @param {string} pluginPath - Plugin path or URL
   * @param {Object} [options] - Load options
   * @returns {Promise<QuantumPlugin>} Loaded plugin
   */
  async loadPlugin(pluginPath, options = {}) {
    const pluginId = this._generatePluginId(pluginPath);
    
    // Check if already loaded
    if (this._plugins.has(pluginId)) {
      this._logger.warn(`Plugin ${pluginId} already loaded`);
      return this._plugins.get(pluginId);
    }
    
    try {
      // Detect plugin type
      const pluginType = this._detectPluginType(pluginPath);
      
      // Get loader
      const loader = this._pluginLoaders.get(pluginType);
      if (!loader) {
        throw new QuantumError(
          `No loader registered for plugin type: ${pluginType}`,
          'PLUGIN_LOADER_NOT_FOUND'
        );
      }
      
      // Load plugin
      const plugin = await loader(pluginPath, this._engine, options);
      
      if (!(plugin instanceof QuantumPlugin)) {
        throw new QuantumError(
          'Plugin loader must return QuantumPlugin instance',
          'INVALID_PLUGIN'
        );
      }
      
      // Check conflicts
      await this._checkConflicts(plugin);
      
      // Register plugin
      this._plugins.set(pluginId, plugin);
      this._pluginRegistry.register(pluginId, plugin, {
        path: pluginPath,
        type: pluginType,
        state: plugin.getPluginState()
      });
      
      // Register extensions
      this._registerPluginExtensions(plugin);
      
      // Register hooks
      this._registerPluginHooks(plugin);
      
      // Register commands
      this._registerPluginCommands(plugin);
      
      this.emit('pluginLoaded', { pluginId, plugin });
      this._logger.info(`Plugin loaded: ${plugin.getMetadata().name}`);
      
      return plugin;
      
    } catch (error) {
      this._logger.error(`Failed to load plugin ${pluginPath}:`, error);
      throw error;
    }
  }
  
  /**
   * Load plugins from directory
   * @async
   * @param {string} directory - Directory path
   * @returns {Promise<QuantumPlugin[]>} Loaded plugins
   */
  async loadPluginsFromDirectory(directory) {
    const plugins = [];
    
    try {
      // This would require Node.js fs module
      // For simplicity, we'll return empty array in browser
      
      if (typeof process !== 'undefined' && process.versions && process.versions.node) {
        const fs = require('fs').promises;
        const path = require('path');
        
        const files = await fs.readdir(directory);
        
        for (const file of files) {
          const filePath = path.join(directory, file);
          const stat = await fs.stat(filePath);
          
          if (stat.isDirectory() || file.endsWith('.js') || file.endsWith('.json')) {
            try {
              const plugin = await this.loadPlugin(filePath);
              plugins.push(plugin);
            } catch (error) {
              this._logger.warn(`Failed to load plugin from ${filePath}:`, error.message);
            }
          }
        }
      }
      
    } catch (error) {
      this._logger.error(`Failed to load plugins from directory ${directory}:`, error);
    }
    
    return plugins;
  }
  
  /**
   * Enable plugin
   * @async
   * @param {string} pluginId - Plugin ID
   */
  async enablePlugin(pluginId) {
    const plugin = this._plugins.get(pluginId);
    if (!plugin) {
      throw new QuantumError(`Plugin ${pluginId} not found`, 'PLUGIN_NOT_FOUND');
    }
    
    // Check dependencies
    await this._checkDependencies(plugin);
    
    await plugin.enable();
    this._updatePluginState(pluginId, PluginState.ENABLED);
  }
  
  /**
   * Disable plugin
   * @async
   * @param {string} pluginId - Plugin ID
   */
  async disablePlugin(pluginId) {
    const plugin = this._plugins.get(pluginId);
    if (!plugin) {
      throw new QuantumError(`Plugin ${pluginId} not found`, 'PLUGIN_NOT_FOUND');
    }
    
    await plugin.disable();
    this._updatePluginState(pluginId, PluginState.DISABLED);
  }
  
  /**
   * Unload plugin
   * @async
   * @param {string} pluginId - Plugin ID
   */
  async unloadPlugin(pluginId) {
    const plugin = this._plugins.get(pluginId);
    if (!plugin) {
      throw new QuantumError(`Plugin ${pluginId} not found`, 'PLUGIN_NOT_FOUND');
    }
    
    // Disable first
    if (plugin.getPluginState() === PluginState.ENABLED) {
      await plugin.disable();
    }
    
    // Destroy plugin
    await plugin.destroy();
    
    // Unregister
    this._plugins.delete(pluginId);
    this._pluginRegistry.remove(pluginId);
    this._unregisterPluginExtensions(plugin);
    this._unregisterPluginHooks(plugin);
    this._unregisterPluginCommands(plugin);
    
    this.emit('pluginUnloaded', { pluginId });
    this._logger.info(`Plugin unloaded: ${plugin.getMetadata().name}`);
  }
  
  /**
   * Get plugin by ID
   * @param {string} pluginId - Plugin ID
   * @returns {QuantumPlugin|null} Plugin instance
   */
  getPlugin(pluginId) {
    return this._plugins.get(pluginId) || null;
  }
  
  /**
   * Get all plugins
   * @returns {QuantumPlugin[]} All plugins
   */
  getAllPlugins() {
    return Array.from(this._plugins.values());
  }
  
  /**
   * Get plugins by state
   * @param {PluginState} state - Plugin state
   * @returns {QuantumPlugin[]} Filtered plugins
   */
  getPluginsByState(state) {
    return this.getAllPlugins().filter(plugin => plugin.getPluginState() === state);
  }
  
  /**
   * Register extension point
   * @param {string} pointName - Extension point name
   * @param {Object} [options] - Extension point options
   */
  registerExtensionPoint(pointName, options = {}) {
    if (this._extensionPoints.has(pointName)) {
      throw new QuantumError(
        `Extension point ${pointName} already registered`,
        'EXTENSION_POINT_EXISTS'
      );
    }
    
    this._extensionPoints.set(pointName, {
      name: pointName,
      description: options.description || '',
      schema: options.schema || {},
      extensions: []
    });
    
    this._logger.debug(`Extension point registered: ${pointName}`);
  }
  
  /**
   * Get extensions for point
   * @param {string} pointName - Extension point name
   * @returns {Object[]} Extensions
   */
  getExtensions(pointName) {
    const point = this._extensionPoints.get(pointName);
    return point ? [...point.extensions] : [];
  }
  
  /**
   * Execute hooks
   * @async
   * @param {string} hookName - Hook name
   * @param {...any} args - Hook arguments
   * @returns {Promise<any>} Hook result
   */
  async executeHooks(hookName, ...args) {
    const hooks = this._hookRegistry.get(hookName) || [];
    let result = null;
    
    // Sort by priority
    const sortedHooks = hooks.sort((a, b) => a.priority - b.priority);
    
    for (const hook of sortedHooks) {
      try {
        result = await hook.handler(...args, result);
      } catch (error) {
        this._logger.error(`Error in hook ${hookName} from plugin ${hook.pluginId}:`, error);
        // Continue with other hooks
      }
    }
    
    return result;
  }
  
  /**
   * Execute command
   * @async
   * @param {string} command - Command name
   * @param {...any} args - Command arguments
   * @returns {Promise<any>} Command result
   */
  async executeCommand(command, ...args) {
    const cmd = this._commandRegistry.get(command);
    if (!cmd) {
      throw new QuantumError(
        `Command '${command}' not found`,
        'COMMAND_NOT_FOUND'
      );
    }
    
    try {
      return await cmd.handler(...args);
    } catch (error) {
      this._logger.error(`Error executing command ${command}:`, error);
      throw error;
    }
  }
  
  /**
   * Get all commands
   * @returns {Object} All registered commands
   */
  getAllCommands() {
    const commands = {};
    for (const [cmd, info] of this._commandRegistry) {
      commands[cmd] = info.description;
    }
    return commands;
  }
  
  /**
   * Scan for plugins
   * @async
   * @returns {Promise<string[]>} Discovered plugin paths
   */
  async scanForPlugins() {
    const discovered = [];
    
    for (const dir of this._pluginDirs) {
      try {
        const plugins = await this.loadPluginsFromDirectory(dir);
        discovered.push(...plugins.map(p => p.getMetadata().id));
      } catch (error) {
        // Directory might not exist
        continue;
      }
    }
    
    return discovered;
  }
  
  /**
   * Initialize plugin manager
   * @private
   */
  _initialize() {
    // Get logger from engine
    this._logger = this._engine.getLogger();
    
    // Register default loaders
    this._registerDefaultLoaders();
    
    // Register default extension points
    this._registerDefaultExtensionPoints();
  }
  
  /**
   * Register default loaders
   * @private
   */
  _registerDefaultLoaders() {
    // JavaScript module loader
    this.registerLoader('js', async (path, engine, options) => {
      // Dynamic import
      const module = await import(path);
      
      if (!module.default || typeof module.default !== 'function') {
        throw new QuantumError('Plugin module must export a default class', 'INVALID_PLUGIN_MODULE');
      }
      
      const PluginClass = module.default;
      const metadata = module.metadata || {};
      
      return new PluginClass(metadata.id || path, engine, metadata, options);
    });
    
    // JSON manifest loader
    this.registerLoader('json', async (path, engine, options) => {
      // Load JSON file
      const response = await fetch(path);
      const manifest = await response.json();
      
      if (!manifest.main) {
        throw new QuantumError('Plugin manifest must have "main" field', 'INVALID_MANIFEST');
      }
      
      // Load main module
      return this.loadPlugin(manifest.main, { ...options, manifest });
    });
  }
  
  /**
   * Register default extension points
   * @private
   */
  _registerDefaultExtensionPoints() {
    this.registerExtensionPoint('engine:startup', {
      description: 'Engine startup extensions',
      schema: {
        type: 'object',
        properties: {
          initialize: { type: 'function' },
          startup: { type: 'function' }
        }
      }
    });
    
    this.registerExtensionPoint('ui:widgets', {
      description: 'UI widget extensions',
      schema: {
        type: 'object',
        properties: {
          type: { type: 'string' },
          component: { type: 'function' }
        }
      }
    });
    
    this.registerExtensionPoint('assets:loaders', {
      description: 'Asset loader extensions',
      schema: {
        type: 'object',
        properties: {
          extensions: { type: 'array' },
          loader: { type: 'function' }
        }
      }
    });
  }
  
  /**
   * Generate plugin ID
   * @private
   * @param {string} path - Plugin path
   * @returns {string} Plugin ID
   */
  _generatePluginId(path) {
    // Simple hash from path
    let hash = 0;
    for (let i = 0; i < path.length; i++) {
      hash = ((hash << 5) - hash) + path.charCodeAt(i);
      hash |= 0; // Convert to 32bit integer
    }
    return `plugin_${Math.abs(hash).toString(16)}`;
  }
  
  /**
   * Detect plugin type
   * @private
   * @param {string} path - Plugin path
   * @returns {string} Plugin type
   */
  _detectPluginType(path) {
    if (path.endsWith('.js')) return 'js';
    if (path.endsWith('.json')) return 'json';
    if (path.endsWith('.wasm')) return 'wasm';
    
    // Check if it's a directory
    if (path.indexOf('.') === -1 || path.endsWith('/')) {
      return 'directory';
    }
    
    return 'unknown';
  }
  
  /**
   * Check plugin conflicts
   * @private
   * @async
   * @param {QuantumPlugin} plugin - Plugin to check
   */
  async _checkConflicts(plugin) {
    const metadata = plugin.getMetadata();
    
    for (const conflictId of metadata.conflicts || []) {
      if (this._plugins.has(conflictId)) {
        throw new QuantumError(
          `Plugin ${metadata.name} conflicts with ${conflictId}`,
          'PLUGIN_CONFLICT'
        );
      }
    }
  }
  
  /**
   * Check plugin dependencies
   * @private
   * @async
   * @param {QuantumPlugin} plugin - Plugin to check
   */
  async _checkDependencies(plugin) {
    const metadata = plugin.getMetadata();
    
    for (const depId of metadata.dependencies || []) {
      const depPlugin = this._plugins.get(depId);
      if (!depPlugin) {
        throw new QuantumError(
          `Plugin ${metadata.name} requires ${depId} but it's not loaded`,
          'MISSING_DEPENDENCY'
        );
      }
      
      if (depPlugin.getPluginState() !== PluginState.ENABLED) {
        throw new QuantumError(
          `Plugin ${metadata.name} requires ${depId} to be enabled`,
          'DEPENDENCY_NOT_ENABLED'
        );
      }
    }
  }
  
  /**
   * Register plugin extensions
   * @private
   * @param {QuantumPlugin} plugin - Plugin
   */
  _registerPluginExtensions(plugin) {
    for (const [pointName, extensions] of plugin._extensions) {
      const point = this._extensionPoints.get(pointName);
      if (point) {
        point.extensions.push(...extensions.map(ext => ({
          ...ext,
          pluginId: plugin.getName()
        })));
        this._logger.debug(`Extensions registered for ${pointName} from ${plugin.getName()}`);
      } else {
        this._logger.warn(`Extension point ${pointName} not found, skipping extensions from ${plugin.getName()}`);
      }
    }
  }
  
  /**
   * Unregister plugin extensions
   * @private
   * @param {QuantumPlugin} plugin - Plugin
   */
  _unregisterPluginExtensions(plugin) {
    for (const [pointName] of plugin._extensions) {
      const point = this._extensionPoints.get(pointName);
      if (point) {
        point.extensions = point.extensions.filter(ext => ext.pluginId !== plugin.getName());
      }
    }
  }
  
  /**
   * Register plugin hooks
   * @private
   * @param {QuantumPlugin} plugin - Plugin
   */
  _registerPluginHooks(plugin) {
    for (const [hookName, hooks] of plugin._hooks) {
      if (!this._hookRegistry.has(hookName)) {
        this._hookRegistry.set(hookName, []);
      }
      
      const registryHooks = this._hookRegistry.get(hookName);
      registryHooks.push(...hooks.map(hook => ({
        ...hook,
        pluginId: plugin.getName()
      })));
    }
  }
  
  /**
   * Unregister plugin hooks
   * @private
   * @param {QuantumPlugin} plugin - Plugin
   */
  _unregisterPluginHooks(plugin) {
    for (const [hookName, hooks] of this._hookRegistry) {
      this._hookRegistry.set(
        hookName,
        hooks.filter(hook => hook.pluginId !== plugin.getName())
      );
    }
  }
  
  /**
   * Register plugin commands
   * @private
   * @param {QuantumPlugin} plugin - Plugin
   */
  _registerPluginCommands(plugin) {
    for (const [command, info] of plugin._commands) {
      if (this._commandRegistry.has(command)) {
        this._logger.warn(`Command ${command} already registered, overwriting`);
      }
      
      this._commandRegistry.set(command, {
        ...info,
        pluginId: plugin.getName()
      });
    }
  }
  
  /**
   * Unregister plugin commands
   * @private
   * @param {QuantumPlugin} plugin - Plugin
   */
  _unregisterPluginCommands(plugin) {
    for (const [command, info] of this._commandRegistry) {
      if (info.pluginId === plugin.getName()) {
        this._commandRegistry.delete(command);
      }
    }
  }
  
  /**
   * Update plugin state in registry
   * @private
   * @param {string} pluginId - Plugin ID
   * @param {PluginState} state - New state
   */
  _updatePluginState(pluginId, state) {
    const entry = this._pluginRegistry.getEntry(pluginId);
    if (entry) {
      entry.metadata.state = state;
      this._pluginRegistry.update(pluginId, entry.value, { state });
    }
  }
}

// Default export
export default QuantumPluginManager;
