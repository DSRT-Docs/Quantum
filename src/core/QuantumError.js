/**
 * Quantum Engine Error System
 * @module QuantumError
 */

/**
 * Base Quantum Engine Error
 * @class QuantumError
 * @extends Error
 */
export class QuantumError extends Error {
  /**
   * Create a new Quantum error
   * @param {string} message - Error message
   * @param {string} [code] - Error code
   * @param {Object} [details] - Additional error details
   */
  constructor(message, code = 'QUANTUM_ERROR', details = {}) {
    super(message);
    this.name = 'QuantumError';
    this.code = code;
    this.details = details;
    this.timestamp = new Date().toISOString();
    this.stackTrace = this._captureStackTrace();
    
    // Preserve original stack trace
    if (Error.captureStackTrace) {
      Error.captureStackTrace(this, QuantumError);
    }
  }
  
  /**
   * Convert error to JSON
   * @returns {Object} JSON representation
   */
  toJSON() {
    return {
      name: this.name,
      message: this.message,
      code: this.code,
      details: this.details,
      timestamp: this.timestamp,
      stack: this.stack
    };
  }
  
  /**
   * Capture stack trace
   * @private
   * @returns {Array} Stack trace frames
   */
  _captureStackTrace() {
    const stack = this.stack || '';
    return stack.split('\n').slice(1).map(line => line.trim());
  }
}

/**
 * Initialization Error
 * @class QuantumInitializationError
 * @extends QuantumError
 */
export class QuantumInitializationError extends QuantumError {
  constructor(message, details = {}) {
    super(message, 'INITIALIZATION_ERROR', details);
    this.name = 'QuantumInitializationError';
  }
}

/**
 * Configuration Error
 * @class QuantumConfigurationError
 * @extends QuantumError
 */
export class QuantumConfigurationError extends QuantumError {
  constructor(message, details = {}) {
    super(message, 'CONFIGURATION_ERROR', details);
    this.name = 'QuantumConfigurationError';
  }
}

/**
 * Resource Loading Error
 * @class QuantumResourceError
 * @extends QuantumError
 */
export class QuantumResourceError extends QuantumError {
  constructor(message, resourceType, resourcePath, details = {}) {
    super(message, 'RESOURCE_ERROR', { resourceType, resourcePath, ...details });
    this.name = 'QuantumResourceError';
    this.resourceType = resourceType;
    this.resourcePath = resourcePath;
  }
}

/**
 * Runtime Error
 * @class QuantumRuntimeError
 * @extends QuantumError
 */
export class QuantumRuntimeError extends QuantumError {
  constructor(message, system, details = {}) {
    super(message, 'RUNTIME_ERROR', { system, ...details });
    this.name = 'QuantumRuntimeError';
    this.system = system;
  }
}

/**
 * Validation Error
 * @class QuantumValidationError
 * @extends QuantumError
 */
export class QuantumValidationError extends QuantumError {
  constructor(message, field, value, details = {}) {
    super(message, 'VALIDATION_ERROR', { field, value, ...details });
    this.name = 'QuantumValidationError';
    this.field = field;
    this.value = value;
  }
}

/**
 * Network Error
 * @class QuantumNetworkError
 * @extends QuantumError
 */
export class QuantumNetworkError extends QuantumError {
  constructor(message, url, status, details = {}) {
    super(message, 'NETWORK_ERROR', { url, status, ...details });
    this.name = 'QuantumNetworkError';
    this.url = url;
    this.status = status;
  }
}

/**
 * Scripting Error
 * @class QuantumScriptingError
 * @extends QuantumError
 */
export class QuantumScriptingError extends QuantumError {
  constructor(message, scriptName, line, column, details = {}) {
    super(message, 'SCRIPTING_ERROR', { scriptName, line, column, ...details });
    this.name = 'QuantumScriptingError';
    this.scriptName = scriptName;
    this.line = line;
    this.column = column;
  }
}

/**
 * Physics Error
 * @class QuantumPhysicsError
 * @extends QuantumError
 */
export class QuantumPhysicsError extends QuantumError {
  constructor(message, physicsObject, details = {}) {
    super(message, 'PHYSICS_ERROR', { physicsObject, ...details });
    this.name = 'QuantumPhysicsError';
    this.physicsObject = physicsObject;
  }
}

/**
 * Rendering Error
 * @class QuantumRenderingError
 * @extends QuantumError
 */
export class QuantumRenderingError extends QuantumError {
  constructor(message, renderContext, details = {}) {
    super(message, 'RENDERING_ERROR', { renderContext, ...details });
    this.name = 'QuantumRenderingError';
    this.renderContext = renderContext;
  }
}

/**
 * Asset Error
 * @class QuantumAssetError
 * @extends QuantumError
 */
export class QuantumAssetError extends QuantumError {
  constructor(message, assetType, assetId, details = {}) {
    super(message, 'ASSET_ERROR', { assetType, assetId, ...details });
    this.name = 'QuantumAssetError';
    this.assetType = assetType;
    this.assetId = assetId;
  }
}

/**
 * Entity Error
 * @class QuantumEntityError
 * @extends QuantumError
 */
export class QuantumEntityError extends QuantumError {
  constructor(message, entityId, component, details = {}) {
    super(message, 'ENTITY_ERROR', { entityId, component, ...details });
    this.name = 'QuantumEntityError';
    this.entityId = entityId;
    this.component = component;
  }
}

/**
 * Memory Error
 * @class QuantumMemoryError
 * @extends QuantumError
 */
export class QuantumMemoryError extends QuantumError {
  constructor(message, allocationSize, memoryType, details = {}) {
    super(message, 'MEMORY_ERROR', { allocationSize, memoryType, ...details });
    this.name = 'QuantumMemoryError';
    this.allocationSize = allocationSize;
    this.memoryType = memoryType;
  }
}

/**
 * Input Error
 * @class QuantumInputError
 * @extends QuantumError
 */
export class QuantumInputError extends QuantumError {
  constructor(message, inputDevice, inputEvent, details = {}) {
    super(message, 'INPUT_ERROR', { inputDevice, inputEvent, ...details });
    this.name = 'QuantumInputError';
    this.inputDevice = inputDevice;
    this.inputEvent = inputEvent;
  }
}

/**
 * Audio Error
 * @class QuantumAudioError
 * @extends QuantumError
 */
export class QuantumAudioError extends QuantumError {
  constructor(message, audioContext, audioNode, details = {}) {
    super(message, 'AUDIO_ERROR', { audioContext, audioNode, ...details });
    this.name = 'QuantumAudioError';
    this.audioContext = audioContext;
    this.audioNode = audioNode;
  }
}

/**
 * Error Manager
 * @class QuantumErrorManager
 */
export class QuantumErrorManager {
  constructor(engine) {
    this._engine = engine;
    this._errorHandlers = new Map();
    this._errorHistory = [];
    this._maxErrorHistory = 1000;
    this._setupDefaultHandlers();
  }
  
  /**
   * Handle an error
   * @param {Error} error - Error to handle
   * @param {Object} [context] - Additional context
   */
  handleError(error, context = {}) {
    // Add to history
    this._addToHistory(error, context);
    
    // Find appropriate handler
    const handler = this._findHandler(error);
    if (handler) {
      try {
        handler(error, context);
      } catch (handlerError) {
        // Fallback to default handler if custom handler fails
        console.error('Error handler failed:', handlerError);
        this._defaultErrorHandler(error, context);
      }
    } else {
      this._defaultErrorHandler(error, context);
    }
    
    // Emit error event
    this._engine.emit('error', { error, context });
  }
  
  /**
   * Register error handler
   * @param {string} errorType - Error type/class name
   * @param {Function} handler - Error handler function
   */
  registerHandler(errorType, handler) {
    if (typeof handler !== 'function') {
      throw new QuantumError('Handler must be a function', 'HANDLER_ERROR');
    }
    this._errorHandlers.set(errorType, handler);
  }
  
  /**
   * Unregister error handler
   * @param {string} errorType - Error type to remove
   */
  unregisterHandler(errorType) {
    this._errorHandlers.delete(errorType);
  }
  
  /**
   * Get error history
   * @param {number} [limit=100] - Maximum errors to return
   * @returns {Array} Error history
   */
  getErrorHistory(limit = 100) {
    return this._errorHistory.slice(-limit);
  }
  
  /**
   * Clear error history
   */
  clearErrorHistory() {
    this._errorHistory = [];
  }
  
  /**
   * Create and throw a typed error
   * @param {string} type - Error type
   * @param {string} message - Error message
   * @param {Object} [details] - Error details
   * @throws {QuantumError}
   */
  throwError(type, message, details = {}) {
    const ErrorClass = this._getErrorClass(type);
    const error = new ErrorClass(message, details);
    this.handleError(error, { thrower: 'errorManager' });
    throw error;
  }
  
  /**
   * Setup default error handlers
   * @private
   */
  _setupDefaultHandlers() {
    // Default handler for all Quantum errors
    this.registerHandler('QuantumError', (error, context) => {
      const logger = this._engine.getLogger();
      logger.error(`[${error.code}] ${error.message}`, error.details);
      
      if (this._engine.getConfig().get('debug', false)) {
        console.error('Stack trace:', error.stack);
      }
    });
    
    // Handler for initialization errors
    this.registerHandler('QuantumInitializationError', (error, context) => {
      const logger = this._engine.getLogger();
      logger.error(`Initialization failed: ${error.message}`, error.details);
      
      // Try to recover or shut down gracefully
      this._engine.emit('initializationFailed', { error, context });
    });
    
    // Handler for resource errors
    this.registerHandler('QuantumResourceError', (error, context) => {
      const logger = this._engine.getLogger();
      logger.error(`Resource error [${error.resourceType}]: ${error.message}`, {
        path: error.resourcePath,
        ...error.details
      });
      
      // Try to load fallback resource
      this._engine.emit('resourceError', { error, context });
    });
    
    // Handler for runtime errors
    this.registerHandler('QuantumRuntimeError', (error, context) => {
      const logger = this._engine.getLogger();
      logger.error(`Runtime error in ${error.system}: ${error.message}`, error.details);
      
      // Attempt to recover
      this._engine.emit('runtimeError', { error, context });
    });
  }
  
  /**
   * Find appropriate handler for error
   * @private
   * @param {Error} error - Error to handle
   * @returns {Function|null} Handler function
   */
  _findHandler(error) {
    // Try exact class name
    if (this._errorHandlers.has(error.name)) {
      return this._errorHandlers.get(error.name);
    }
    
    // Try parent class (QuantumError)
    if (error instanceof QuantumError && this._errorHandlers.has('QuantumError')) {
      return this._errorHandlers.get('QuantumError');
    }
    
    // Try generic Error
    if (this._errorHandlers.has('Error')) {
      return this._errorHandlers.get('Error');
    }
    
    return null;
  }
  
  /**
   * Default error handler
   * @private
   * @param {Error} error - Error to handle
   * @param {Object} context - Error context
   */
  _defaultErrorHandler(error, context) {
    console.error('Unhandled error:', error);
    console.error('Context:', context);
    
    // In debug mode, break execution
    if (this._engine.getConfig().get('debug', false)) {
      debugger;
    }
  }
  
  /**
   * Add error to history
   * @private
   * @param {Error} error - Error to add
   * @param {Object} context - Error context
   */
  _addToHistory(error, context) {
    const entry = {
      error: {
        name: error.name,
        message: error.message,
        code: error.code,
        stack: error.stack
      },
      context,
      timestamp: new Date().toISOString()
    };
    
    this._errorHistory.push(entry);
    
    // Limit history size
    if (this._errorHistory.length > this._maxErrorHistory) {
      this._errorHistory.shift();
    }
  }
  
  /**
   * Get error class by type
   * @private
   * @param {string} type - Error type
   * @returns {Function} Error class
   */
  _getErrorClass(type) {
    const errorClasses = {
      'QuantumError': QuantumError,
      'QuantumInitializationError': QuantumInitializationError,
      'QuantumConfigurationError': QuantumConfigurationError,
      'QuantumResourceError': QuantumResourceError,
      'QuantumRuntimeError': QuantumRuntimeError,
      'QuantumValidationError': QuantumValidationError,
      'QuantumNetworkError': QuantumNetworkError,
      'QuantumScriptingError': QuantumScriptingError,
      'QuantumPhysicsError': QuantumPhysicsError,
      'QuantumRenderingError': QuantumRenderingError,
      'QuantumAssetError': QuantumAssetError,
      'QuantumEntityError': QuantumEntityError,
      'QuantumMemoryError': QuantumMemoryError,
      'QuantumInputError': QuantumInputError,
      'QuantumAudioError': QuantumAudioError
    };
    
    return errorClasses[type] || QuantumError;
  }
  
  /**
   * Destroy error manager
   */
  destroy() {
    this._errorHandlers.clear();
    this.clearErrorHistory();
  }
}

// Error utilities
export const ErrorUtils = {
  /**
   * Check if error is a Quantum error
   * @param {Error} error - Error to check
   * @returns {boolean} True if Quantum error
   */
  isQuantumError(error) {
    return error instanceof QuantumError;
  },
  
  /**
   * Create error from unknown value
   * @param {any} error - Error value
   * @returns {QuantumError} Standardized error
   */
  normalizeError(error) {
    if (error instanceof QuantumError) {
      return error;
    }
    
    if (error instanceof Error) {
      return new QuantumError(
        error.message,
        'EXTERNAL_ERROR',
        { originalError: error.name, stack: error.stack }
      );
    }
    
    return new QuantumError(
      String(error),
      'UNKNOWN_ERROR',
      { originalValue: error }
    );
  },
  
  /**
   * Wrap async function with error handling
   * @param {Function} fn - Async function to wrap
   * @param {Object} [context] - Error context
   * @returns {Function} Wrapped function
   */
  wrapAsync(fn, context = {}) {
    return async (...args) => {
      try {
        return await fn(...args);
      } catch (error) {
        const normalizedError = this.normalizeError(error);
        throw normalizedError;
      }
    };
  },
  
  /**
   * Create error with stack trace
   * @param {string} message - Error message
   * @returns {Error} Error with stack trace
   */
  createErrorWithStack(message) {
    const error = new Error(message);
    Error.captureStackTrace(error, this.createErrorWithStack);
    return error;
  }
};

// Default export
export default QuantumError;
