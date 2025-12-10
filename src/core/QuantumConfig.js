/**
 * Quantum Engine Configuration Manager
 * @module QuantumConfig
 */

import { QuantumError } from './QuantumError.js';

/**
 * Configuration manager for Quantum Engine
 * @class QuantumConfig
 */
export class QuantumConfig {
  /**
   * Create a new configuration manager
   * @param {Object} userConfig - User configuration
   */
  constructor(userConfig = {}) {
    this._defaultConfig = this._getDefaultConfig();
    this._config = this._mergeConfigs(this._defaultConfig, userConfig);
    this._validators = new Map();
    this._setupValidators();
    
    // Validate initial config
    this._validateConfig(this._config);
  }
  
  /**
   * Get configuration value
   * @param {string} key - Configuration key (dot notation supported)
   * @param {any} defaultValue - Default value if key doesn't exist
   * @returns {any} Configuration value
   */
  get(key, defaultValue = undefined) {
    const keys = key.split('.');
    let value = this._config;
    
    for (const k of keys) {
      if (value && typeof value === 'object' && k in value) {
        value = value[k];
      } else {
        return defaultValue;
      }
    }
    
    return value !== undefined ? value : defaultValue;
  }
  
  /**
   * Set configuration value
   * @param {string} key - Configuration key (dot notation supported)
   * @param {any} value - Value to set
   * @param {boolean} validate - Whether to validate the value
   */
  set(key, value, validate = true) {
    const keys = key.split('.');
    const lastKey = keys.pop();
    let config = this._config;
    
    // Navigate to the nested object
    for (const k of keys) {
      if (!config[k] || typeof config[k] !== 'object') {
        config[k] = {};
      }
      config = config[k];
    }
    
    // Validate if requested
    if (validate) {
      this._validateValue(key, value);
    }
    
    // Set the value
    config[lastKey] = value;
    
    // Emit change event
    this.emit('configChanged', { key, value });
  }
  
  /**
   * Merge additional configuration
   * @param {Object} newConfig - Configuration to merge
   */
  merge(newConfig) {
    this._config = this._mergeConfigs(this._config, newConfig);
    this._validateConfig(this._config);
    this.emit('configMerged', { newConfig });
  }
  
  /**
   * Reset configuration to defaults
   */
  reset() {
    this._config = { ...this._defaultConfig };
    this.emit('configReset');
  }
  
  /**
   * Get entire configuration object
   * @returns {Object} Complete configuration
   */
  getAll() {
    return JSON.parse(JSON.stringify(this._config));
  }
  
  /**
   * Check if configuration has a key
   * @param {string} key - Configuration key
   * @returns {boolean} True if key exists
   */
  has(key) {
    const keys = key.split('.');
    let config = this._config;
    
    for (const k of keys) {
      if (config && typeof config === 'object' && k in config) {
        config = config[k];
      } else {
        return false;
      }
    }
    
    return true;
  }
  
  /**
   * Register a custom validator
   * @param {string} key - Configuration key
   * @param {Function} validator - Validation function
   */
  registerValidator(key, validator) {
    if (typeof validator !== 'function') {
      throw new QuantumError('Validator must be a function');
    }
    this._validators.set(key, validator);
  }
  
  /**
   * Validate entire configuration
   * @returns {Object} Validation results
   */
  validateAll() {
    const results = {
      valid: true,
      errors: []
    };
    
    for (const [key, validator] of this._validators) {
      if (this.has(key)) {
        const value = this.get(key);
        try {
          const validationResult = validator(value, key);
          if (validationResult !== true) {
            results.valid = false;
            results.errors.push({
              key,
              value,
              message: validationResult || `Invalid value for ${key}`
            });
          }
        } catch (error) {
          results.valid = false;
          results.errors.push({
            key,
            value,
            message: error.message
          });
        }
      }
    }
    
    return results;
  }
  
  /**
   * Get default configuration
   * @private
   * @returns {Object} Default configuration
   */
  _getDefaultConfig() {
    return {
      // Core settings
      name: 'Quantum Engine Application',
      version: '1.0.0',
      autoStart: true,
      debug: false,
      profiling: false,
      
      // Rendering settings
      rendering: {
        enabled: true,
        antialias: true,
        alpha: false,
        depth: true,
        stencil: false,
        preserveDrawingBuffer: false,
        powerPreference: 'high-performance',
        maxLights: 8,
        shadowQuality: 'medium',
        textureQuality: 'high',
        anisotropy: 4,
        msaaSamples: 4
      },
      
      // Canvas settings
      canvas: {
        id: 'quantum-canvas',
        width: 1280,
        height: 720,
        parent: document.body,
        style: {
          display: 'block'
        }
      },
      
      // Physics settings
      physics: {
        enabled: true,
        gravity: { x: 0, y: -9.81, z: 0 },
        solverIterations: 10,
        broadphase: 'sweep-and-prune',
        fps: 60,
        fixedTimeStep: 1 / 60
      },
      
      // Audio settings
      audio: {
        enabled: true,
        volume: 1.0,
        spatialAudio: true,
        maxChannels: 32,
        reverb: false
      },
      
      // Input settings
      input: {
        enabled: true,
        keyboard: true,
        mouse: true,
        touch: true,
        gamepad: false,
        sensitivity: 1.0
      },
      
      // Network settings
      network: {
        enabled: false,
        serverUrl: 'ws://localhost:8080',
        reconnectAttempts: 5,
        reconnectDelay: 1000,
        timeout: 30000
      },
      
      // Asset settings
      assets: {
        basePath: './assets/',
        maxParallelLoads: 4,
        cache: true,
        cacheSize: 1024 * 1024 * 100 // 100MB
      },
      
      // Scene settings
      scene: {
        maxEntities: 10000,
        culling: true,
        frustumCulling: true,
        occlusionCulling: false
      },
      
      // UI settings
      ui: {
        enabled: true,
        scaleMode: 'scale-with-screen-size',
        referenceResolution: { width: 1920, height: 1080 },
        pixelPerfect: false
      },
      
      // Scripting settings
      scripting: {
        enabled: true,
        hotReload: false,
        typeChecking: true,
        sandbox: true
      },
      
      // Memory settings
      memory: {
        poolSizes: {
          entities: 1000,
          components: 10000,
          vectors: 5000,
          matrices: 1000
        },
        garbageCollection: {
          enabled: true,
          interval: 60000 // 60 seconds
        }
      },
      
      // Logging settings
      logging: {
        level: 'info',
        colors: true,
        timestamps: true,
        file: null,
        maxFileSize: 1024 * 1024 * 10 // 10MB
      },
      
      // Performance settings
      performance: {
        maxFPS: 120,
        vsync: true,
        throttleWhenHidden: true,
        showStats: false
      }
    };
  }
  
  /**
   * Merge configurations
   * @private
   * @param {Object} target - Target configuration
   * @param {Object} source - Source configuration
   * @returns {Object} Merged configuration
   */
  _mergeConfigs(target, source) {
    const result = { ...target };
    
    for (const key in source) {
      if (source[key] && typeof source[key] === 'object' && !Array.isArray(source[key])) {
        result[key] = this._mergeConfigs(result[key] || {}, source[key]);
      } else {
        result[key] = source[key];
      }
    }
    
    return result;
  }
  
  /**
   * Setup default validators
   * @private
   */
  _setupValidators() {
    // Canvas validators
    this.registerValidator('canvas.width', (value) => {
      if (typeof value !== 'number' || value <= 0) {
        return 'Canvas width must be a positive number';
      }
      if (value > 8192) {
        return 'Canvas width cannot exceed 8192';
      }
      return true;
    });
    
    this.registerValidator('canvas.height', (value) => {
      if (typeof value !== 'number' || value <= 0) {
        return 'Canvas height must be a positive number';
      }
      if (value > 8192) {
        return 'Canvas height cannot exceed 8192';
      }
      return true;
    });
    
    // Rendering validators
    this.registerValidator('rendering.maxLights', (value) => {
      if (!Number.isInteger(value) || value < 1 || value > 256) {
        return 'Max lights must be an integer between 1 and 256';
      }
      return true;
    });
    
    // Physics validators
    this.registerValidator('physics.fps', (value) => {
      if (!Number.isInteger(value) || value < 1 || value > 1000) {
        return 'Physics FPS must be an integer between 1 and 1000';
      }
      return true;
    });
    
    // Audio validators
    this.registerValidator('audio.volume', (value) => {
      if (typeof value !== 'number' || value < 0 || value > 1) {
        return 'Volume must be a number between 0 and 1';
      }
      return true;
    });
    
    // Performance validators
    this.registerValidator('performance.maxFPS', (value) => {
      if (!Number.isInteger(value) || value < 1 || value > 1000) {
        return 'Max FPS must be an integer between 1 and 1000';
      }
      return true;
    });
  }
  
  /**
   * Validate a single value
   * @private
   * @param {string} key - Configuration key
   * @param {any} value - Value to validate
   */
  _validateValue(key, value) {
    const validator = this._validators.get(key);
    if (validator) {
      const result = validator(value, key);
      if (result !== true) {
        throw new QuantumError(`Configuration validation failed for ${key}: ${result}`);
      }
    }
  }
  
  /**
   * Validate entire configuration
   * @private
   * @param {Object} config - Configuration to validate
   */
  _validateConfig(config) {
    const validation = this.validateAll();
    if (!validation.valid) {
      const errors = validation.errors.map(e => `${e.key}: ${e.message}`).join(', ');
      throw new QuantumError(`Configuration validation failed: ${errors}`);
    }
  }
  
  /**
   * Event emitter compatibility
   * @private
   */
  emit(event, data) {
    // This would be connected to an event emitter
    // For simplicity, we're not implementing full event system here
    if (this._eventListeners && this._eventListeners[event]) {
      this._eventListeners[event].forEach(listener => listener(data));
    }
  }
}

// Export as default
export default QuantumConfig;
