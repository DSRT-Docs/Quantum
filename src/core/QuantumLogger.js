/**
 * Quantum Engine Logger
 * @module QuantumLogger
 */

import { QuantumConfig } from './QuantumConfig.js';

/**
 * Log levels
 * @enum {string}
 */
export const LogLevel = {
  ERROR: 'error',
  WARN: 'warn',
  INFO: 'info',
  DEBUG: 'debug',
  TRACE: 'trace'
};

/**
 * Log level priorities
 * @constant
 */
const LOG_LEVEL_PRIORITY = {
  [LogLevel.ERROR]: 0,
  [LogLevel.WARN]: 1,
  [LogLevel.INFO]: 2,
  [LogLevel.DEBUG]: 3,
  [LogLevel.TRACE]: 4
};

/**
 * ANSI color codes for console output
 * @constant
 */
const COLORS = {
  reset: '\x1b[0m',
  red: '\x1b[31m',
  green: '\x1b[32m',
  yellow: '\x1b[33m',
  blue: '\x1b[34m',
  magenta: '\x1b[35m',
  cyan: '\x1b[36m',
  white: '\x1b[37m',
  gray: '\x1b[90m'
};

/**
 * Quantum Engine Logger
 * @class QuantumLogger
 */
export class QuantumLogger {
  /**
   * Create a new logger
   * @param {QuantumConfig} config - Engine configuration
   */
  constructor(config) {
    this._config = config;
    this._logLevel = this._getLogLevel();
    this._colorsEnabled = this._config.get('logging.colors', true);
    this._timestampsEnabled = this._config.get('logging.timestamps', true);
    
    // Log file handling
    this._logFile = null;
    this._maxFileSize = this._config.get('logging.maxFileSize', 10 * 1024 * 1024);
    this._logFilePath = this._config.get('logging.file', null);
    
    // Initialize log file if specified
    if (this._logFilePath) {
      this._initLogFile();
    }
    
    // Log history (for debugging)
    this._history = [];
    this._maxHistorySize = 1000;
    
    // Custom log handlers
    this._handlers = [];
    
    // Log initialization
    this.info('Quantum Logger initialized');
  }
  
  /**
   * Log an error message
   * @param {string} message - Message to log
   * @param {...any} args - Additional arguments
   */
  error(message, ...args) {
    this._log(LogLevel.ERROR, message, args);
  }
  
  /**
   * Log a warning message
   * @param {string} message - Message to log
   * @param {...any} args - Additional arguments
   */
  warn(message, ...args) {
    this._log(LogLevel.WARN, message, args);
  }
  
  /**
   * Log an info message
   * @param {string} message - Message to log
   * @param {...any} args - Additional arguments
   */
  info(message, ...args) {
    this._log(LogLevel.INFO, message, args);
  }
  
  /**
   * Log a debug message
   * @param {string} message - Message to log
   * @param {...any} args - Additional arguments
   */
  debug(message, ...args) {
    this._log(LogLevel.DEBUG, message, args);
  }
  
  /**
   * Log a trace message
   * @param {string} message - Message to log
   * @param {...any} args - Additional arguments
   */
  trace(message, ...args) {
    this._log(LogLevel.TRACE, message, args);
  }
  
  /**
   * Log performance data
   * @param {string} label - Performance label
   * @param {number} time - Time in milliseconds
   * @param {Object} [metadata] - Additional metadata
   */
  perf(label, time, metadata = {}) {
    if (this._config.get('profiling', false)) {
      const color = time < 16.67 ? COLORS.green : time < 33.33 ? COLORS.yellow : COLORS.red;
      const reset = this._colorsEnabled ? COLORS.reset : '';
      const coloredTime = this._colorsEnabled ? `${color}${time.toFixed(2)}ms${reset}` : `${time.toFixed(2)}ms`;
      
      this._log(LogLevel.INFO, `⏱️ ${label}: ${coloredTime}`, [metadata], false);
    }
  }
  
  /**
   * Add a custom log handler
   * @param {Function} handler - Handler function
   */
  addHandler(handler) {
    if (typeof handler !== 'function') {
      throw new Error('Log handler must be a function');
    }
    this._handlers.push(handler);
  }
  
  /**
   * Remove a log handler
   * @param {Function} handler - Handler to remove
   */
  removeHandler(handler) {
    const index = this._handlers.indexOf(handler);
    if (index > -1) {
      this._handlers.splice(index, 1);
    }
  }
  
  /**
   * Clear all log handlers
   */
  clearHandlers() {
    this._handlers = [];
  }
  
  /**
   * Get log history
   * @param {number} [limit=100] - Maximum number of entries to return
   * @returns {Array} Log history
   */
  getHistory(limit = 100) {
    return this._history.slice(-limit);
  }
  
  /**
   * Clear log history
   */
  clearHistory() {
    this._history = [];
  }
  
  /**
   * Enable or disable colors
   * @param {boolean} enabled - Whether colors are enabled
   */
  setColors(enabled) {
    this._colorsEnabled = enabled;
  }
  
  /**
   * Enable or disable timestamps
   * @param {boolean} enabled - Whether timestamps are enabled
   */
  setTimestamps(enabled) {
    this._timestampsEnabled = enabled;
  }
  
  /**
   * Set log level
   * @param {LogLevel} level - New log level
   */
  setLevel(level) {
    if (!Object.values(LogLevel).includes(level)) {
      throw new Error(`Invalid log level: ${level}`);
    }
    this._logLevel = level;
    this._config.set('logging.level', level, false);
  }
  
  /**
   * Get current log level
   * @returns {LogLevel} Current log level
   */
  getLevel() {
    return this._logLevel;
  }
  
  /**
   * Destroy logger and cleanup
   */
  destroy() {
    if (this._logFile) {
      this._logFile.end();
      this._logFile = null;
    }
    this.clearHandlers();
    this.clearHistory();
  }
  
  /**
   * Internal log method
   * @private
   * @param {LogLevel} level - Log level
   * @param {string} message - Message to log
   * @param {Array} args - Additional arguments
   * @param {boolean} storeInHistory - Whether to store in history
   */
  _log(level, message, args, storeInHistory = true) {
    // Check if we should log this level
    if (LOG_LEVEL_PRIORITY[level] > LOG_LEVEL_PRIORITY[this._logLevel]) {
      return;
    }
    
    // Format the log entry
    const timestamp = this._timestampsEnabled ? new Date().toISOString() : null;
    const formattedMessage = this._formatMessage(level, message, timestamp);
    
    // Console output
    this._consoleLog(level, formattedMessage, args);
    
    // File output
    if (this._logFile) {
      this._fileLog(level, formattedMessage, args);
    }
    
    // Call custom handlers
    for (const handler of this._handlers) {
      try {
        handler(level, message, args, timestamp);
      } catch (error) {
        // Don't let handler errors break logging
        console.error('Log handler error:', error);
      }
    }
    
    // Store in history
    if (storeInHistory) {
      this._addToHistory(level, message, args, timestamp);
    }
  }
  
  /**
   * Format log message
   * @private
   * @param {LogLevel} level - Log level
   * @param {string} message - Message
   * @param {string|null} timestamp - Timestamp
   * @returns {string} Formatted message
   */
  _formatMessage(level, message, timestamp) {
    let prefix = '';
    
    if (timestamp) {
      prefix += `[${timestamp}] `;
    }
    
    // Add level prefix with colors if enabled
    if (this._colorsEnabled) {
      const color = this._getLevelColor(level);
      const reset = COLORS.reset;
      prefix += `${color}[${level.toUpperCase()}]${reset} `;
    } else {
      prefix += `[${level.toUpperCase()}] `;
    }
    
    return prefix + message;
  }
  
  /**
   * Get color for log level
   * @private
   * @param {LogLevel} level - Log level
   * @returns {string} ANSI color code
   */
  _getLevelColor(level) {
    switch (level) {
      case LogLevel.ERROR: return COLORS.red;
      case LogLevel.WARN: return COLORS.yellow;
      case LogLevel.INFO: return COLORS.green;
      case LogLevel.DEBUG: return COLORS.blue;
      case LogLevel.TRACE: return COLORS.gray;
      default: return COLORS.white;
    }
  }
  
  /**
   * Output to console
   * @private
   * @param {LogLevel} level - Log level
   * @param {string} formattedMessage - Formatted message
   * @param {Array} args - Additional arguments
   */
  _consoleLog(level, formattedMessage, args) {
    const consoleMethod = this._getConsoleMethod(level);
    
    if (args.length > 0) {
      consoleMethod(formattedMessage, ...args);
    } else {
      consoleMethod(formattedMessage);
    }
  }
  
  /**
   * Get console method for log level
   * @private
   * @param {LogLevel} level - Log level
   * @returns {Function} Console method
   */
  _getConsoleMethod(level) {
    switch (level) {
      case LogLevel.ERROR: return console.error;
      case LogLevel.WARN: return console.warn;
      case LogLevel.INFO: return console.info;
      case LogLevel.DEBUG: return console.debug;
      case LogLevel.TRACE: return console.trace || console.debug;
      default: return console.log;
    }
  }
  
  /**
   * Log to file
   * @private
   * @param {LogLevel} level - Log level
   * @param {string} formattedMessage - Formatted message
   * @param {Array} args - Additional arguments
   */
  _fileLog(level, formattedMessage, args) {
    if (!this._logFile) return;
    
    let logEntry = formattedMessage;
    
    // Add arguments if present
    if (args.length > 0) {
      const argsStr = args.map(arg => 
        typeof arg === 'object' ? JSON.stringify(arg) : String(arg)
      ).join(' ');
      logEntry += ' ' + argsStr;
    }
    
    logEntry += '\n';
    
    // Check file size and rotate if needed
    this._checkFileSize();
    
    // Write to file
    this._logFile.write(logEntry);
  }
  
  /**
   * Add entry to history
   * @private
   * @param {LogLevel} level - Log level
   * @param {string} message - Message
   * @param {Array} args - Arguments
   * @param {string|null} timestamp - Timestamp
   */
  _addToHistory(level, message, args, timestamp) {
    const entry = {
      level,
      message,
      args: args.length > 0 ? args : undefined,
      timestamp: timestamp || new Date().toISOString()
    };
    
    this._history.push(entry);
    
    // Limit history size
    if (this._history.length > this._maxHistorySize) {
      this._history.shift();
    }
  }
  
  /**
   * Get log level from config
   * @private
   * @returns {LogLevel} Log level
   */
  _getLogLevel() {
    const level = this._config.get('logging.level', 'info');
    
    // Validate level
    if (!Object.values(LogLevel).includes(level)) {
      console.warn(`Invalid log level "${level}", defaulting to "info"`);
      return LogLevel.INFO;
    }
    
    return level;
  }
  
  /**
   * Initialize log file
   * @private
   */
  _initLogFile() {
    try {
      // This would require Node.js fs module
      // For browser compatibility, we'll leave this as a stub
      // that can be extended for Node.js environments
      
      if (typeof process !== 'undefined' && process.versions && process.versions.node) {
        // Node.js environment
        const fs = require('fs');
        const path = require('path');
        
        const logDir = path.dirname(this._logFilePath);
        if (!fs.existsSync(logDir)) {
          fs.mkdirSync(logDir, { recursive: true });
        }
        
        this._logFile = fs.createWriteStream(this._logFilePath, { flags: 'a' });
        this.info(`Log file initialized: ${this._logFilePath}`);
      } else {
        // Browser environment - can't write to file directly
        this.warn('File logging not supported in browser environment');
      }
    } catch (error) {
      this.error(`Failed to initialize log file: ${error.message}`);
    }
  }
  
  /**
   * Check and rotate log file if needed
   * @private
   */
  _checkFileSize() {
    if (!this._logFile || !this._logFile.bytesWritten) return;
    
    if (this._logFile.bytesWritten >= this._maxFileSize) {
      this.info('Log file size limit reached, rotating...');
      
      try {
        this._logFile.end();
        
        // Rename current log file
        const fs = require('fs');
        const path = require('path');
        
        const dir = path.dirname(this._logFilePath);
        const ext = path.extname(this._logFilePath);
        const base = path.basename(this._logFilePath, ext);
        const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
        const newPath = path.join(dir, `${base}-${timestamp}${ext}`);
        
        fs.renameSync(this._logFilePath, newPath);
        
        // Create new log file
        this._logFile = fs.createWriteStream(this._logFilePath, { flags: 'a' });
        
        this.info(`Log file rotated: ${newPath}`);
      } catch (error) {
        this.error(`Failed to rotate log file: ${error.message}`);
      }
    }
  }
}

// Export default
export default QuantumLogger;
