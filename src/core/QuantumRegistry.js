/**
 * Quantum Engine Registry System
 * @module QuantumRegistry
 */

import { QuantumError } from './QuantumError.js';
import { EventEmitter } from 'events';

/**
 * Registry entry
 * @typedef {Object} RegistryEntry
 * @property {string} id - Entry ID
 * @property {any} value - Entry value
 * @property {Object} metadata - Entry metadata
 * @property {number} timestamp - Creation timestamp
 * @property {number} lastAccess - Last access timestamp
 * @property {number} accessCount - Access count
 */

/**
 * Registry options
 * @typedef {Object} RegistryOptions
 * @property {boolean} [caseSensitive=true] - Whether IDs are case-sensitive
 * @property {number} [maxEntries=10000] - Maximum number of entries
 * @property {boolean} [autoCleanup=false] - Automatically cleanup unused entries
 * @property {number} [cleanupInterval=60000] - Cleanup interval in milliseconds
 * @property {boolean} [emitEvents=true] - Whether to emit events
 */

/**
 * Quantum Registry - Central registry for engine resources
 * @class QuantumRegistry
 * @extends EventEmitter
 */
export class QuantumRegistry extends EventEmitter {
  /**
   * Create a new registry
   * @param {RegistryOptions} [options] - Registry options
   */
  constructor(options = {}) {
    super();
    
    this._options = {
      caseSensitive: true,
      maxEntries: 10000,
      autoCleanup: false,
      cleanupInterval: 60000, // 1 minute
      emitEvents: true,
      ...options
    };
    
    this._entries = new Map();
    this._entryCount = 0;
    this._namespaces = new Map();
    this._indexes = new Map();
    this._cleanupInterval = null;
    
    // Performance tracking
    this._stats = {
      totalRegistrations: 0,
      totalLookups: 0,
      totalRemovals: 0,
      currentSize: 0,
      hitRate: 0,
      memoryUsage: 0
    };
    
    // Start auto-cleanup if enabled
    if (this._options.autoCleanup) {
      this._startAutoCleanup();
    }
  }
  
  /**
   * Register a value
   * @param {string} id - Entry ID
   * @param {any} value - Entry value
   * @param {Object} [metadata] - Entry metadata
   * @returns {RegistryEntry} Registered entry
   */
  register(id, value, metadata = {}) {
    const normalizedId = this._normalizeId(id);
    
    // Check if already registered
    if (this._entries.has(normalizedId)) {
      throw new QuantumError(
        `Entry with ID '${id}' already registered`,
        'REGISTRY_DUPLICATE',
        { id: normalizedId }
      );
    }
    
    // Check max entries
    if (this._entryCount >= this._options.maxEntries) {
      this._cleanupOldEntries(100); // Make room
    }
    
    const entry = {
      id: normalizedId,
      value,
      metadata: {
        type: typeof value,
        isObject: typeof value === 'object',
        isFunction: typeof value === 'function',
        className: value?.constructor?.name,
        registrationTime: Date.now(),
        ...metadata
      },
      timestamp: Date.now(),
      lastAccess: Date.now(),
      accessCount: 0,
      namespace: this._getNamespaceFromId(normalizedId)
    };
    
    // Store entry
    this._entries.set(normalizedId, entry);
    this._entryCount++;
    
    // Update namespace
    this._addToNamespace(entry.namespace, normalizedId);
    
    // Update indexes
    this._updateIndexes(normalizedId, entry);
    
    // Update stats
    this._stats.totalRegistrations++;
    this._stats.currentSize = this._entryCount;
    this._updateMemoryUsage();
    
    // Emit event
    if (this._options.emitEvents) {
      this.emit('registered', { id: normalizedId, entry });
    }
    
    return entry;
  }
  
  /**
   * Get a registered value
   * @param {string} id - Entry ID
   * @returns {any} Registered value
   */
  get(id) {
    const normalizedId = this._normalizeId(id);
    const entry = this._entries.get(normalizedId);
    
    if (!entry) {
      this._stats.totalLookups++;
      this._updateHitRate();
      throw new QuantumError(
        `Entry with ID '${id}' not found`,
        'REGISTRY_NOT_FOUND',
        { id: normalizedId }
      );
    }
    
    // Update access tracking
    entry.lastAccess = Date.now();
    entry.accessCount++;
    
    // Update stats
    this._stats.totalLookups++;
    this._updateHitRate();
    
    // Emit event
    if (this._options.emitEvents) {
      this.emit('accessed', { id: normalizedId, entry });
    }
    
    return entry.value;
  }
  
  /**
   * Check if entry exists
   * @param {string} id - Entry ID
   * @returns {boolean} True if exists
   */
  has(id) {
    const normalizedId = this._normalizeId(id);
    return this._entries.has(normalizedId);
  }
  
  /**
   * Get entry metadata
   * @param {string} id - Entry ID
   * @returns {Object} Entry metadata
   */
  getMetadata(id) {
    const normalizedId = this._normalizeId(id);
    const entry = this._entries.get(normalizedId);
    
    if (!entry) {
      throw new QuantumError(
        `Entry with ID '${id}' not found`,
        'REGISTRY_NOT_FOUND',
        { id: normalizedId }
      );
    }
    
    return { ...entry.metadata };
  }
  
  /**
   * Get complete entry
   * @param {string} id - Entry ID
   * @returns {RegistryEntry} Complete entry
   */
  getEntry(id) {
    const normalizedId = this._normalizeId(id);
    const entry = this._entries.get(normalizedId);
    
    if (!entry) {
      throw new QuantumError(
        `Entry with ID '${id}' not found`,
        'REGISTRY_NOT_FOUND',
        { id: normalizedId }
      );
    }
    
    return { ...entry };
  }
  
  /**
   * Update entry value
   * @param {string} id - Entry ID
   * @param {any} value - New value
   * @param {Object} [metadataUpdates] - Metadata updates
   * @returns {RegistryEntry} Updated entry
   */
  update(id, value, metadataUpdates = {}) {
    const normalizedId = this._normalizeId(id);
    const entry = this._entries.get(normalizedId);
    
    if (!entry) {
      throw new QuantumError(
        `Entry with ID '${id}' not found`,
        'REGISTRY_NOT_FOUND',
        { id: normalizedId }
      );
    }
    
    // Update value
    const oldValue = entry.value;
    entry.value = value;
    
    // Update metadata
    entry.metadata = {
      ...entry.metadata,
      ...metadataUpdates,
      updateTime: Date.now(),
      previousValue: oldValue
    };
    
    entry.lastAccess = Date.now();
    entry.accessCount++;
    
    // Update indexes
    this._updateIndexes(normalizedId, entry);
    
    // Emit event
    if (this._options.emitEvents) {
      this.emit('updated', { id: normalizedId, entry, oldValue });
    }
    
    return { ...entry };
  }
  
  /**
   * Remove an entry
   * @param {string} id - Entry ID
   * @returns {boolean} True if removed
   */
  remove(id) {
    const normalizedId = this._normalizeId(id);
    const entry = this._entries.get(normalizedId);
    
    if (!entry) {
      return false;
    }
    
    // Remove from namespace
    this._removeFromNamespace(entry.namespace, normalizedId);
    
    // Remove from indexes
    this._removeFromIndexes(normalizedId);
    
    // Remove entry
    this._entries.delete(normalizedId);
    this._entryCount--;
    
    // Update stats
    this._stats.totalRemovals++;
    this._stats.currentSize = this._entryCount;
    this._updateMemoryUsage();
    
    // Emit event
    if (this._options.emitEvents) {
      this.emit('removed', { id: normalizedId, entry });
    }
    
    return true;
  }
  
  /**
   * Clear all entries
   * @param {string} [namespace] - Optional namespace to clear
   */
  clear(namespace) {
    if (namespace) {
      const normalizedNs = this._normalizeId(namespace);
      const entries = this._namespaces.get(normalizedNs) || new Set();
      
      for (const id of entries) {
        this.remove(id);
      }
      
      this._namespaces.delete(normalizedNs);
    } else {
      this._entries.clear();
      this._namespaces.clear();
      this._indexes.clear();
      this._entryCount = 0;
      this._stats.currentSize = 0;
      this._updateMemoryUsage();
      
      // Emit event
      if (this._options.emitEvents) {
        this.emit('cleared', { namespace });
      }
    }
  }
  
  /**
   * Find entries by criteria
   * @param {Object} criteria - Search criteria
   * @returns {RegistryEntry[]} Matching entries
   */
  find(criteria = {}) {
    const results = [];
    
    for (const [id, entry] of this._entries) {
      if (this._matchesCriteria(entry, criteria)) {
        results.push({ ...entry });
      }
    }
    
    return results;
  }
  
  /**
   * Find one entry by criteria
   * @param {Object} criteria - Search criteria
   * @returns {RegistryEntry|null} Matching entry
   */
  findOne(criteria = {}) {
    for (const [id, entry] of this._entries) {
      if (this._matchesCriteria(entry, criteria)) {
        return { ...entry };
      }
    }
    return null;
  }
  
  /**
   * Get all entries
   * @returns {RegistryEntry[]} All entries
   */
  getAll() {
    return Array.from(this._entries.values()).map(entry => ({ ...entry }));
  }
  
  /**
   * Get entries by namespace
   * @param {string} namespace - Namespace
   * @returns {RegistryEntry[]} Entries in namespace
   */
  getByNamespace(namespace) {
    const normalizedNs = this._normalizeId(namespace);
    const entryIds = this._namespaces.get(normalizedNs) || new Set();
    const entries = [];
    
    for (const id of entryIds) {
      const entry = this._entries.get(id);
      if (entry) {
        entries.push({ ...entry });
      }
    }
    
    return entries;
  }
  
  /**
   * Get all namespaces
   * @returns {string[]} All namespaces
   */
  getNamespaces() {
    return Array.from(this._namespaces.keys());
  }
  
  /**
   * Create index for faster lookups
   * @param {string} indexName - Index name
   * @param {Function} keyExtractor - Function to extract index key from entry
   */
  createIndex(indexName, keyExtractor) {
    if (this._indexes.has(indexName)) {
      throw new QuantumError(
        `Index '${indexName}' already exists`,
        'REGISTRY_INDEX_EXISTS'
      );
    }
    
    const index = new Map();
    
    // Build index from existing entries
    for (const [id, entry] of this._entries) {
      const key = keyExtractor(entry);
      if (key !== undefined) {
        if (!index.has(key)) {
          index.set(key, new Set());
        }
        index.get(key).add(id);
      }
    }
    
    this._indexes.set(indexName, { keyExtractor, index });
  }
  
  /**
   * Query using index
   * @param {string} indexName - Index name
   * @param {any} key - Index key
   * @returns {RegistryEntry[]} Matching entries
   */
  queryIndex(indexName, key) {
    const indexData = this._indexes.get(indexName);
    if (!indexData) {
      throw new QuantumError(
        `Index '${indexName}' not found`,
        'REGISTRY_INDEX_NOT_FOUND'
      );
    }
    
    const entryIds = indexData.index.get(key);
    if (!entryIds) {
      return [];
    }
    
    const entries = [];
    for (const id of entryIds) {
      const entry = this._entries.get(id);
      if (entry) {
        entries.push({ ...entry });
      }
    }
    
    return entries;
  }
  
  /**
   * Get registry statistics
   * @returns {Object} Registry statistics
   */
  getStats() {
    return {
      ...this._stats,
      hitRate: this._stats.hitRate,
      namespaces: this._namespaces.size,
      indexes: this._indexes.size
    };
  }
  
  /**
   * Cleanup old/unused entries
   * @param {number} [maxAge=3600000] - Maximum age in milliseconds
   * @param {number} [maxUnused=10000] - Maximum time unused in milliseconds
   * @returns {number} Number of entries removed
   */
  cleanup(maxAge = 3600000, maxUnused = 10000) {
    const now = Date.now();
    let removedCount = 0;
    
    for (const [id, entry] of this._entries) {
      const age = now - entry.timestamp;
      const unusedTime = now - entry.lastAccess;
      
      if (age > maxAge || unusedTime > maxUnused) {
        this.remove(id);
        removedCount++;
      }
    }
    
    return removedCount;
  }
  
  /**
   * Destroy registry
   */
  destroy() {
    this.clear();
    
    if (this._cleanupInterval) {
      clearInterval(this._cleanupInterval);
      this._cleanupInterval = null;
    }
    
    this._indexes.clear();
    this.removeAllListeners();
  }
  
  // Internal methods
  
  /**
   * Normalize ID
   * @private
   * @param {string} id - Original ID
   * @returns {string} Normalized ID
   */
  _normalizeId(id) {
    if (!this._options.caseSensitive) {
      return id.toLowerCase();
    }
    return id;
  }
  
  /**
   * Get namespace from ID
   * @private
   * @param {string} id - Entry ID
   * @returns {string} Namespace
   */
  _getNamespaceFromId(id) {
    const parts = id.split(':');
    return parts.length > 1 ? parts[0] : 'global';
  }
  
  /**
   * Add entry to namespace
   * @private
   * @param {string} namespace - Namespace
   * @param {string} id - Entry ID
   */
  _addToNamespace(namespace, id) {
    if (!this._namespaces.has(namespace)) {
      this._namespaces.set(namespace, new Set());
    }
    this._namespaces.get(namespace).add(id);
  }
  
  /**
   * Remove entry from namespace
   * @private
   * @param {string} namespace - Namespace
   * @param {string} id - Entry ID
   */
  _removeFromNamespace(namespace, id) {
    const namespaceEntries = this._namespaces.get(namespace);
    if (namespaceEntries) {
      namespaceEntries.delete(id);
      if (namespaceEntries.size === 0) {
        this._namespaces.delete(namespace);
      }
    }
  }
  
  /**
   * Update indexes for entry
   * @private
   * @param {string} id - Entry ID
   * @param {RegistryEntry} entry - Entry
   */
  _updateIndexes(id, entry) {
    for (const [indexName, indexData] of this._indexes) {
      const key = indexData.keyExtractor(entry);
      const index = indexData.index;
      
      // Remove old key if exists
      for (const [existingKey, ids] of index) {
        ids.delete(id);
        if (ids.size === 0) {
          index.delete(existingKey);
        }
      }
      
      // Add new key
      if (key !== undefined) {
        if (!index.has(key)) {
          index.set(key, new Set());
        }
        index.get(key).add(id);
      }
    }
  }
  
  /**
   * Remove entry from indexes
   * @private
   * @param {string} id - Entry ID
   */
  _removeFromIndexes(id) {
    for (const [indexName, indexData] of this._indexes) {
      const index = indexData.index;
      
      for (const [key, ids] of index) {
        ids.delete(id);
        if (ids.size === 0) {
          index.delete(key);
        }
      }
    }
  }
  
  /**
   * Check if entry matches criteria
   * @private
   * @param {RegistryEntry} entry - Entry to check
   * @param {Object} criteria - Search criteria
   * @returns {boolean} True if matches
   */
  _matchesCriteria(entry, criteria) {
    for (const [key, value] of Object.entries(criteria)) {
      if (key === 'id') {
        if (entry.id !== value) return false;
      } else if (key in entry.metadata) {
        if (entry.metadata[key] !== value) return false;
      } else if (key in entry) {
        if (entry[key] !== value) return false;
      } else {
        // Try deep match in metadata or value
        if (typeof entry.value === 'object' && entry.value !== null) {
          if (entry.value[key] !== value) return false;
        } else {
          return false;
        }
      }
    }
    return true;
  }
  
  /**
   * Start auto-cleanup
   * @private
   */
  _startAutoCleanup() {
    this._cleanupInterval = setInterval(() => {
      this.cleanup();
    }, this._options.cleanupInterval);
  }
  
  /**
   * Cleanup old entries to make space
   * @private
   * @param {number} count - Number of entries to cleanup
   */
  _cleanupOldEntries(count) {
    const entries = Array.from(this._entries.entries())
      .sort((a, b) => a[1].lastAccess - b[1].lastAccess)
      .slice(0, count);
    
    for (const [id] of entries) {
      this.remove(id);
    }
  }
  
  /**
   * Update hit rate
   * @private
   */
  _updateHitRate() {
    if (this._stats.totalLookups > 0) {
      const hits = this._stats.totalLookups - (this._entryCount - this._stats.currentSize);
      this._stats.hitRate = Math.min(1, Math.max(0, hits / this._stats.totalLookups));
    }
  }
  
  /**
   * Update memory usage estimate
   * @private
   */
  _updateMemoryUsage() {
    // Rough estimate: average 1KB per entry
    this._stats.memoryUsage = this._entryCount * 1024;
  }
}

/**
 * Global Registry Manager
 * @class QuantumRegistryManager
 */
export class QuantumRegistryManager {
  constructor() {
    this._registries = new Map();
    this._defaultRegistry = new QuantumRegistry();
    this._registryTypes = new Map();
  }
  
  /**
   * Create a new registry
   * @param {string} name - Registry name
   * @param {RegistryOptions} [options] - Registry options
   * @returns {QuantumRegistry} Created registry
   */
  createRegistry(name, options = {}) {
    if (this._registries.has(name)) {
      throw new QuantumError(
        `Registry '${name}' already exists`,
        'REGISTRY_EXISTS'
      );
    }
    
    const registry = new QuantumRegistry(options);
    this._registries.set(name, registry);
    
    return registry;
  }
  
  /**
   * Get registry by name
   * @param {string} name - Registry name
   * @returns {QuantumRegistry} Registry instance
   */
  getRegistry(name) {
    const registry = this._registries.get(name);
    if (!registry) {
      throw new QuantumError(
        `Registry '${name}' not found`,
        'REGISTRY_NOT_FOUND'
      );
    }
    return registry;
  }
  
  /**
   * Get default registry
   * @returns {QuantumRegistry} Default registry
   */
  getDefaultRegistry() {
    return this._defaultRegistry;
  }
  
  /**
   * Check if registry exists
   * @param {string} name - Registry name
   * @returns {boolean} True if exists
   */
  hasRegistry(name) {
    return this._registries.has(name);
  }
  
  /**
   * Remove registry
   * @param {string} name - Registry name
   * @returns {boolean} True if removed
   */
  removeRegistry(name) {
    const registry = this._registries.get(name);
    if (registry) {
      registry.destroy();
      this._registries.delete(name);
      return true;
    }
    return false;
  }
  
  /**
   * Get all registry names
   * @returns {string[]} Registry names
   */
  getRegistryNames() {
    return Array.from(this._registries.keys());
  }
  
  /**
   * Register a registry type
   * @param {string} typeName - Type name
   * @param {Function} factory - Registry factory function
   */
  registerRegistryType(typeName, factory) {
    this._registryTypes.set(typeName, factory);
  }
  
  /**
   * Create registry of specific type
   * @param {string} name - Registry name
   * @param {string} typeName - Registry type
   * @param {Object} [options] - Registry options
   * @returns {QuantumRegistry} Created registry
   */
  createTypedRegistry(name, typeName, options = {}) {
    const factory = this._registryTypes.get(typeName);
    if (!factory) {
      throw new QuantumError(
        `Registry type '${typeName}' not found`,
        'REGISTRY_TYPE_NOT_FOUND'
      );
    }
    
    const registry = factory(options);
    if (!(registry instanceof QuantumRegistry)) {
      throw new QuantumError(
        `Registry factory for type '${typeName}' must return QuantumRegistry instance`,
        'INVALID_REGISTRY'
      );
    }
    
    this._registries.set(name, registry);
    return registry;
  }
  
  /**
   * Clear all registries
   */
  clearAll() {
    for (const registry of this._registries.values()) {
      registry.destroy();
    }
    this._registries.clear();
    this._defaultRegistry.clear();
  }
  
  /**
   * Destroy registry manager
   */
  destroy() {
    this.clearAll();
    this._registryTypes.clear();
  }
}

// Pre-defined registry types
export const RegistryTypes = {
  /**
   * Component registry factory
   */
  COMPONENT: (options) => new QuantumRegistry({
    caseSensitive: false,
    maxEntries: 50000,
    autoCleanup: true,
    cleanupInterval: 30000,
    ...options
  }),
  
  /**
   * Asset registry factory
   */
  ASSET: (options) => new QuantumRegistry({
    caseSensitive: false,
    maxEntries: 10000,
    autoCleanup: false,
    ...options
  }),
  
  /**
   * Entity registry factory
   */
  ENTITY: (options) => new QuantumRegistry({
    caseSensitive: true,
    maxEntries: 100000,
    autoCleanup: true,
    cleanupInterval: 60000,
    ...options
  }),
  
  /**
   * Service registry factory
   */
  SERVICE: (options) => new QuantumRegistry({
    caseSensitive: false,
    maxEntries: 1000,
    autoCleanup: false,
    ...options
  }),
  
  /**
   * Event registry factory
   */
  EVENT: (options) => new QuantumRegistry({
    caseSensitive: false,
    maxEntries: 5000,
    autoCleanup: true,
    cleanupInterval: 15000,
    ...options
  })
};

// Default export
export default QuantumRegistry;
