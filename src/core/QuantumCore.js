// src/core/QuantumCore.js - INTEGRATED MASTER SYSTEM
import { QuantumModule } from './QuantumModule.js';
import { QuantumContext } from './QuantumContext.js';
import { QuantumError } from './QuantumError.js';
import { QuantumLogger } from './QuantumLogger.js';
import { QuantumConfig } from './QuantumConfig.js';
import { QuantumRegistry } from './QuantumRegistry.js';
import { QuantumLifecycle } from './QuantumLifecycle.js';
import { QuantumUtils } from './QuantumUtils.js';
import { QuantumConstants } from './QuantumConstants.js';
import { QuantumVersion } from './QuantumVersion.js';
import { QuantumSettings } from './QuantumSettings.js';

/**
 * Quantum Core - Fully Integrated Master System
 * @class QuantumCore
 */
export class QuantumCore {
  constructor(options = {}) {
    // ==================== CORE INTEGRATION SYSTEM ====================
    
    // Initialize shared registry FIRST
    this._globalRegistry = new QuantumRegistry();
    
    // Create CORE CONTEXT for engine operations
    this._coreContext = new QuantumContext(this, {
      contextId: 'quantum_core_root',
      contextType: 'engine_core',
      contextName: 'Quantum Core Root Context'
    });
    
    // Initialize module system WITH context integration
    this._moduleSystem = this._initializeIntegratedModuleSystem();
    
    // Initialize all subsystems WITH mutual references
    this._initializeIntegratedSubsystems();
    
    // Setup bidirectional communication channels
    this._setupBidirectionalIntegration();
    
    // Register core systems in global registry
    this._registerCoreSystems();
  }
  
  /** @private */
  _initializeIntegratedModuleSystem() {
    const moduleSystem = {
      modules: new Map(),
      moduleContexts: new Map(),
      moduleRegistry: this._globalRegistry.createSubRegistry('modules'),
      
      // CREATE MODULE with automatic context creation
      createModule: (moduleConfig) => {
        const module = new QuantumModule(this, moduleConfig);
        
        // Create dedicated context for this module
        const moduleContext = this._coreContext.createChildContext({
          contextId: `module_ctx_${module.moduleId}`,
          contextType: 'module_execution',
          contextName: `${module.moduleName} Execution Context`,
          moduleReference: module // Bidirectional reference
        });
        
        // Store bidirectional references
        module._executionContext = moduleContext;
        moduleContext._moduleReference = module;
        
        this.modules.set(module.moduleId, module);
        this.moduleContexts.set(module.moduleId, moduleContext);
        this.moduleRegistry.register(module.moduleId, module);
        
        // Setup event forwarding between module and context
        this._setupModuleContextEventBridge(module, moduleContext);
        
        return module;
      },
      
      // EXECUTE MODULE OPERATION through integrated context
      executeModuleOperation: async (moduleId, operation, ...args) => {
        const module = this.modules.get(moduleId);
        const context = this.moduleContexts.get(moduleId);
        
        if (!module || !context) {
          throw new QuantumError(`Module ${moduleId} not found`);
        }
        
        // Execute in module's dedicated context
        return await context.execute(() => {
          return module[operation](...args);
        });
      },
      
      // GET MODULE with context
      getModuleWithContext: (moduleId) => {
        return {
          module: this.modules.get(moduleId),
          context: this.moduleContexts.get(moduleId)
        };
      }
    };
    
    return moduleSystem;
  }
  
  /** @private */
  _initializeIntegratedSubsystems() {
    // Initialize ALL subsystems with CORE CONTEXT integration
    
    // 1. Config System with context awareness
    this._config = new QuantumConfig(this, {
      parentContext: this._coreContext,
      registry: this._globalRegistry
    });
    
    // 2. Lifecycle System with module/context coordination
    this._lifecycle = new QuantumLifecycle(this, {
      moduleSystem: this._moduleSystem,
      contextSystem: this._coreContext
    });
    
    // 3. Error System with contextual error handling
    this._errorSystem = this._initializeIntegratedErrorSystem();
    
    // 4. Performance System with cross-system monitoring
    this._performanceSystem = this._initializeIntegratedPerformanceSystem();
    
    // 5. Event System with module/context event routing
    this._eventSystem = this._initializeIntegratedEventSystem();
    
    // 6. Resource System with shared resource pool
    this._resourceSystem = this._initializeIntegratedResourceSystem();
    
    // 7. Security System with contextual permissions
    this._securitySystem = this._initializeIntegratedSecuritySystem();
    
    // 8. Service System with dependency injection
    this._serviceSystem = this._initializeIntegratedServiceSystem();
    
    // Setup cross-system integration hooks
    this._setupCrossSystemIntegrationHooks();
  }
  
  /** @private */
  _initializeIntegratedErrorSystem() {
    return {
      errors: new Map(),
      contextualHandlers: new Map(),
      
      // Handle error with context awareness
      handleError: (error, context) => {
        const errorId = `err_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
        
        const errorRecord = {
          id: errorId,
          error,
          context: {
            contextId: context?.contextId,
            contextType: context?.contextType,
            moduleId: context?.moduleReference?.moduleId,
            timestamp: Date.now(),
            stack: new Error().stack
          },
          resolution: null
        };
        
        this.errors.set(errorId, errorRecord);
        
        // Route error to appropriate handler based on context
        this._routeErrorToHandler(errorRecord);
        
        // Propagate to module if applicable
        if (context?.moduleReference) {
          context.moduleReference._errorHandler.handleError(error, {
            source: 'core_error_system',
            contextId: context.contextId
          });
        }
        
        return errorRecord;
      },
      
      _routeErrorToHandler: (errorRecord) => {
        // Find handler based on context type
        const handler = this.contextualHandlers.get(errorRecord.context.contextType);
        if (handler) {
          handler.handle(errorRecord);
        }
        
        // Also route to global handlers
        this._globalRegistry.get('error_handlers')?.forEach(handler => {
          handler.handle(errorRecord);
        });
      }
    };
  }
  
  /** @private */
  _initializeIntegratedPerformanceSystem() {
    return {
      metrics: new Map(),
      monitors: new Map(),
      
      // Track performance across modules and contexts
      trackPerformance: (metricName, value, metadata = {}) => {
        if (!this.metrics.has(metricName)) {
          this.metrics.set(metricName, []);
        }
        
        const metricRecord = {
          value,
          timestamp: Date.now(),
          metadata: {
            ...metadata,
            contextId: metadata.context?.contextId,
            moduleId: metadata.context?.moduleReference?.moduleId
          }
        };
        
        this.metrics.get(metricName).push(metricRecord);
        
        // Notify monitors
        this.monitors.forEach(monitor => {
          if (monitor.isInterested(metricName, metadata)) {
            monitor.record(metricRecord);
          }
        });
        
        // Update module performance if applicable
        if (metadata.context?.moduleReference) {
          metadata.context.moduleReference._performanceMonitor.recordMetric(
            `core_${metricName}`,
            value
          );
        }
      },
      
      // Get cross-system performance report
      getIntegratedReport: () => {
        const report = {
          coreMetrics: Array.from(this.metrics.entries()).map(([name, records]) => ({
            name,
            latest: records[records.length - 1],
            average: records.reduce((sum, r) => sum + r.value, 0) / records.length,
            count: records.length
          })),
          
          moduleMetrics: this._aggregateModuleMetrics(),
          contextMetrics: this._aggregateContextMetrics(),
          systemHealth: this._calculateSystemHealth()
        };
        
        return report;
      },
      
      _aggregateModuleMetrics: () => {
        const aggregated = {};
        
        this._moduleSystem.modules.forEach((module, moduleId) => {
          if (module._performanceMonitor) {
            aggregated[moduleId] = module._performanceMonitor.getMetrics();
          }
        });
        
        return aggregated;
      },
      
      _aggregateContextMetrics: () => {
        const aggregated = {};
        
        // Aggregate from all contexts including module contexts
        const allContexts = [
          this._coreContext,
          ...Array.from(this._moduleSystem.moduleContexts.values())
        ];
        
        allContexts.forEach(context => {
          if (context.getPerformanceMetrics) {
            aggregated[context.contextId] = context.getPerformanceMetrics();
          }
        });
        
        return aggregated;
      }
    };
  }
  
  /** @private */
  _initializeIntegratedEventSystem() {
    return {
      eventChannels: new Map(),
      eventRoutes: new Map(),
      
      // Publish event with routing to modules and contexts
      publish: (eventName, data, options = {}) => {
        const event = {
          name: eventName,
          data,
          timestamp: Date.now(),
          source: options.source || 'core',
          contextId: options.context?.contextId
        };
        
        // Route to global listeners
        this._routeEventToGlobalListeners(event);
        
        // Route to module-specific listeners
        if (options.targetModule) {
          this._routeEventToModule(options.targetModule, event);
        }
        
        // Route to context-specific listeners
        if (options.context) {
          this._routeEventToContext(options.context, event);
        }
        
        // Auto-route based on event type
        this._autoRouteEvent(event);
        
        return event;
      },
      
      _routeEventToGlobalListeners: (event) => {
        const listeners = this.eventChannels.get(event.name) || [];
        listeners.forEach(listener => {
          try {
            listener(event);
          } catch (error) {
            this._errorSystem.handleError(error, { source: 'event_system', event });
          }
        });
      },
      
      _routeEventToModule: (moduleId, event) => {
        const module = this._moduleSystem.modules.get(moduleId);
        if (module && module.emit) {
          module.emit(`core:${event.name}`, event.data);
        }
      },
      
      _routeEventToContext: (context, event) => {
        if (context.emit) {
          context.emit(`system:${event.name}`, event.data);
        }
      },
      
      _autoRouteEvent: (event) => {
        // Auto-route based on event name patterns
        if (event.name.startsWith('module:')) {
          const moduleId = event.name.split(':')[1];
          this._routeEventToModule(moduleId, event);
        }
        
        if (event.name.startsWith('context:')) {
          const contextId = event.name.split(':')[1];
          // Find and route to specific context
        }
      },
      
      // Subscribe with automatic context binding
      subscribe: (eventName, handler, options = {}) => {
        if (!this.eventChannels.has(eventName)) {
          this.eventChannels.set(eventName, []);
        }
        
        const wrappedHandler = (event) => {
          // Bind context to handler if provided
          if (options.context) {
            handler.call(options.context, event);
          } else {
            handler(event);
          }
        };
        
        this.eventChannels.get(eventName).push(wrappedHandler);
        
        // Return unsubscribe function
        return () => {
          const listeners = this.eventChannels.get(eventName);
          const index = listeners.indexOf(wrappedHandler);
          if (index > -1) listeners.splice(index, 1);
        };
      }
    };
  }
  
  /** @private */
  _initializeIntegratedResourceSystem() {
    return {
      resourcePools: new Map(),
      allocations: new Map(),
      
      // Allocate resource with context tracking
      allocate: (resourceType, spec, context) => {
        const allocationId = `alloc_${resourceType}_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
        
        const allocation = {
          id: allocationId,
          type: resourceType,
          spec,
          context: {
            contextId: context.contextId,
            contextType: context.contextType,
            moduleId: context.moduleReference?.moduleId
          },
          timestamp: Date.now(),
          status: 'allocated'
        };
        
        this.allocations.set(allocationId, allocation);
        
        // Track in context
        if (context.trackResourceAllocation) {
          context.trackResourceAllocation(allocation);
        }
        
        // Track in module if applicable
        if (context.moduleReference?._resourceManager) {
          context.moduleReference._resourceManager.allocateResource(resourceType, spec);
        }
        
        return allocation;
      },
      
      // Release resource with cleanup coordination
      release: (allocationId, context) => {
        const allocation = this.allocations.get(allocationId);
        if (!allocation) return false;
        
        allocation.status = 'released';
        allocation.releaseTime = Date.now();
        
        // Cleanup in context
        if (context?.releaseResourceAllocation) {
          context.releaseResourceAllocation(allocationId);
        }
        
        // Cleanup in module if applicable
        if (allocation.context.moduleId) {
          const module = this._moduleSystem.modules.get(allocation.context.moduleId);
          if (module?._resourceManager?.releaseResource) {
            module._resourceManager.releaseResource(allocationId);
          }
        }
        
        this.allocations.delete(allocationId);
        
        return true;
      },
      
      // Get resource usage by context/module
      getUsageReport: () => {
        const report = {
          totalAllocations: this.allocations.size,
          byContext: {},
          byModule: {},
          byType: {}
        };
        
        this.allocations.forEach(allocation => {
          // Group by context
          const contextId = allocation.context.contextId;
          if (!report.byContext[contextId]) {
            report.byContext[contextId] = [];
          }
          report.byContext[contextId].push(allocation);
          
          // Group by module
          const moduleId = allocation.context.moduleId;
          if (moduleId) {
            if (!report.byModule[moduleId]) {
              report.byModule[moduleId] = [];
            }
            report.byModule[moduleId].push(allocation);
          }
          
          // Group by type
          const type = allocation.type;
          if (!report.byType[type]) {
            report.byType[type] = [];
          }
          report.byType[type].push(allocation);
        });
        
        return report;
      }
    };
  }
  
  /** @private */
  _setupBidirectionalIntegration() {
    // 1. Setup Core → Module communication
    this._setupCoreToModuleIntegration();
    
    // 2. Setup Core → Context communication
    this._setupCoreToContextIntegration();
    
    // 3. Setup Module ↔ Context bidirectional communication
    this._setupModuleContextBidirectionalIntegration();
    
    // 4. Setup Cross-system event bridges
    this._setupCrossSystemEventBridges();
    
    // 5. Setup Shared state synchronization
    this._setupSharedStateSynchronization();
  }
  
  /** @private */
  _setupCoreToModuleIntegration() {
    // Core can directly call module methods through integration layer
    this.executeModuleOperation = async (moduleId, operation, ...args) => {
      return await this._moduleSystem.executeModuleOperation(moduleId, operation, ...args);
    };
    
    // Core can access module state
    this.getModuleState = (moduleId) => {
      const module = this._moduleSystem.modules.get(moduleId);
      return module ? module.getDiagnostics() : null;
    };
    
    // Core can control module lifecycle
    this.controlModuleLifecycle = async (moduleId, action, options = {}) => {
      const module = this._moduleSystem.modules.get(moduleId);
      if (!module) throw new QuantumError(`Module ${moduleId} not found`);
      
      switch (action) {
        case 'start':
          return await module.start();
        case 'stop':
          return await module.stop(options);
        case 'pause':
          return await module.pause(options);
        case 'resume':
          return await module.resume(options);
        case 'restart':
          return await module.restart(options);
        default:
          throw new QuantumError(`Unknown lifecycle action: ${action}`);
      }
    };
  }
  
  /** @private */
  _setupCoreToContextIntegration() {
    // Core can execute in any context
    this.executeInContext = async (contextId, operation, ...args) => {
      let context;
      
      if (contextId === 'core') {
        context = this._coreContext;
      } else {
        context = this._moduleSystem.moduleContexts.get(contextId);
      }
      
      if (!context) {
        throw new QuantumError(`Context ${contextId} not found`);
      }
      
      return await context.execute(operation, ...args);
    };
    
    // Core can monitor context performance
    this.getContextPerformance = (contextId) => {
      let context;
      
      if (contextId === 'core') {
        context = this._coreContext;
      } else {
        context = this._moduleSystem.moduleContexts.get(contextId);
      }
      
      return context ? context.getPerformanceMetrics() : null;
    };
    
    // Core can create/manage contexts
    this.createContext = (config) => {
      return this._coreContext.createChildContext(config);
    };
  }
  
  /** @private */
  _setupModuleContextBidirectionalIntegration() {
    // This is where the REAL integration happens
    
    // 1. Module can access its context
    QuantumModule.prototype.getExecutionContext = function() {
      return this._executionContext;
    };
    
    // 2. Module can execute operations in its context
    QuantumModule.prototype.executeInContext = async function(operation, ...args) {
      if (!this._executionContext) {
        throw new QuantumError('Module has no execution context');
      }
      
      return await this._executionContext.execute(() => {
        return this[operation](...args);
      });
    };
    
    // 3. Context can access its module
    QuantumContext.prototype.getModule = function() {
      return this._moduleReference;
    };
    
    // 4. Context can trigger module operations
    QuantumContext.prototype.triggerModuleOperation = async function(operation, ...args) {
      if (!this._moduleReference) {
        throw new QuantumError('Context has no associated module');
      }
      
      return await this._moduleReference[operation](...args);
    };
    
    // 5. Shared state synchronization
    QuantumModule.prototype._syncStateToContext = function() {
      if (this._executionContext) {
        this._executionContext._moduleState = this.getDiagnostics();
      }
    };
    
    QuantumContext.prototype._syncContextToModule = function() {
      if (this._moduleReference) {
        this._moduleReference._contextState = this.getDiagnostics();
      }
    };
  }
  
  /** @private */
  _setupModuleContextEventBridge(module, context) {
    // Module events → Context
    module.on('module:*', (eventData) => {
      context.emit(`module_event:${eventData.type}`, {
        ...eventData,
        sourceModule: module.moduleId
      });
    });
    
    // Context events → Module
    context.on('context:*', (eventData) => {
      module.emit(`context_event:${eventData.type}`, {
        ...eventData,
        sourceContext: context.contextId
      });
    });
    
    // Error propagation
    module.on('error', (errorData) => {
      this._errorSystem.handleError(errorData.error, context);
      context.emit('module_error', errorData);
    });
    
    context.on('error', (errorData) => {
      module._errorHandler.handleError(errorData.error, {
        source: 'context',
        contextId: context.contextId
      });
      module.emit('context_error', errorData);
    });
  }
  
  /** @private */
  _setupCrossSystemEventBridges() {
    // Performance events bridge
    this._performanceSystem.monitors.set('cross_system_bridge', {
      isInterested: (metricName) => metricName.includes('performance'),
      record: (metricRecord) => {
        // Broadcast performance events
        this._eventSystem.publish('performance:metric', metricRecord, {
          source: 'performance_system'
        });
      }
    });
    
    // Error events bridge
    this._errorSystem.contextualHandlers.set('module_execution', {
      handle: (errorRecord) => {
        this._eventSystem.publish('error:module', errorRecord, {
          source: 'error_system'
        });
      }
    });
    
    // Resource events bridge
    this._resourceSystem.allocations.on('change', (allocation) => {
      this._eventSystem.publish('resource:allocation', allocation, {
        source: 'resource_system'
      });
    });
  }
  
  /** @private */
  _setupSharedStateSynchronization() {
    // Shared configuration
    this._sharedState = {
      config: {},
      modules: {},
      contexts: {},
      performance: {},
      resources: {}
    };
    
    // Sync module states
    setInterval(() => {
      this._moduleSystem.modules.forEach((module, moduleId) => {
        this._sharedState.modules[moduleId] = module.getDiagnostics();
      });
    }, 1000);
    
    // Sync context states
    setInterval(() => {
      this._moduleSystem.moduleContexts.forEach((context, contextId) => {
        this._sharedState.contexts[contextId] = context.getDiagnostics();
      });
    }, 1000);
    
    // Sync performance data
    setInterval(() => {
      this._sharedState.performance = this._performanceSystem.getIntegratedReport();
    }, 2000);
    
    // Sync resource usage
    setInterval(() => {
      this._sharedState.resources = this._resourceSystem.getUsageReport();
    }, 3000);
  }
  
  /** @private */
  _setupCrossSystemIntegrationHooks() {
    // Module lifecycle hooks that affect contexts
    this._lifecycle.registerHook('module:beforeStart', async (module) => {
      // Ensure module has execution context
      if (!module._executionContext) {
        const context = this._coreContext.createChildContext({
          contextId: `module_ctx_${module.moduleId}`,
          contextType: 'module_execution',
          contextName: `${module.moduleName} Execution Context`,
          moduleReference: module
        });
        
        module._executionContext = context;
        this._moduleSystem.moduleContexts.set(module.moduleId, context);
        this._setupModuleContextEventBridge(module, context);
      }
      
      // Start module context
      await module._executionContext.execute(() => {
        // Context will be ready for module execution
      });
    });
    
    this._lifecycle.registerHook('module:afterStop', async (module) => {
      // Cleanup module context
      const context = this._moduleSystem.moduleContexts.get(module.moduleId);
      if (context) {
        await context.destroy();
        this._moduleSystem.moduleContexts.delete(module.moduleId);
      }
    });
    
    // Context lifecycle hooks that affect modules
    this._coreContext.on('context:created', (context) => {
      if (context._moduleReference) {
        // Notify module about context creation
        context._moduleReference.emit('execution_context:created', {
          contextId: context.contextId
        });
      }
    });
    
    this._coreContext.on('context:destroyed', (context) => {
      if (context._moduleReference) {
        // Notify module about context destruction
        context._moduleReference.emit('execution_context:destroyed', {
          contextId: context.contextId
        });
      }
    });
  }
  
  /** @private */
  _registerCoreSystems() {
    // Register all core systems in global registry
    this._globalRegistry.register('core', this);
    this._globalRegistry.register('module_system', this._moduleSystem);
    this._globalRegistry.register('context_system', this._coreContext);
    this._globalRegistry.register('error_system', this._errorSystem);
    this._globalRegistry.register('performance_system', this._performanceSystem);
    this._globalRegistry.register('event_system', this._eventSystem);
    this._globalRegistry.register('resource_system', this._resourceSystem);
    this._globalRegistry.register('lifecycle_system', this._lifecycle);
    this._globalRegistry.register('config_system', this._config);
  }
  
  // ==================== PUBLIC API ====================
  
  /**
   * Create integrated module with automatic context
   */
  async createModule(moduleConfig) {
    const module = this._moduleSystem.createModule(moduleConfig);
    
    // Initialize module through its context
    await module._executionContext.execute(async () => {
      await module.initialize();
    });
    
    return module;
  }
  
  /**
   * Execute operation across module-context boundary
   */
  async executeIntegrated(moduleId, operation, ...args) {
    const moduleContext = this._moduleSystem.getModuleWithContext(moduleId);
    
    if (!moduleContext.module || !moduleContext.context) {
      throw new QuantumError(`Module ${moduleId} not properly integrated`);
    }
    
    // Execute with full integration tracing
    const result = await moduleContext.context.execute(async () => {
      // Track performance
      const startTime = performance.now();
      
      try {
        const operationResult = await moduleContext.module[operation](...args);
        
        // Record performance
        const duration = performance.now() - startTime;
        this._performanceSystem.trackPerformance(
          `module_operation_${operation}`,
          duration,
          {
            moduleId,
            operation,
            success: true,
            context: moduleContext.context
          }
        );
        
        return operationResult;
        
      } catch (error) {
        // Record error with context
        const duration = performance.now() - startTime;
        this._performanceSystem.trackPerformance(
          `module_operation_${operation}_error`,
          duration,
          {
            moduleId,
            operation,
            success: false,
            context: moduleContext.context
          }
        );
        
        this._errorSystem.handleError(error, moduleContext.context);
        throw error;
      }
    });
    
    return result;
  }
  
  /**
   * Get integrated system diagnostics
   */
  getIntegratedDiagnostics() {
    return {
      core: {
        uptime: Date.now() - this._creationTimestamp,
        state: 'running'
      },
      modules: {
        total: this._moduleSystem.modules.size,
        active: Array.from(this._moduleSystem.modules.values())
          .filter(m => m.currentState === 'running').length,
        details: Array.from(this._moduleSystem.modules.values()).map(m => ({
          id: m.moduleId,
          name: m.moduleName,
          state: m.currentState,
          contextId: m._executionContext?.contextId
        }))
      },
      contexts: {
        total: this._moduleSystem.moduleContexts.size + 1, // +1 for core context
        details: Array.from(this._moduleSystem.moduleContexts.values()).map(c => ({
          id: c.contextId,
          type: c.contextType,
          state: c.currentState,
          moduleId: c._moduleReference?.moduleId
        }))
      },
      performance: this._sharedState.performance,
      resources: this._sharedState.resources,
      integration: {
        eventBridges: this._eventSystem.eventChannels.size,
        resourceAllocations: this._resourceSystem.allocations.size,
        errorHandlers: this._errorSystem.contextualHandlers.size
      }
    };
  }
  
  /**
   * Get cross-system trace (module → context → core)
   */
  getCrossSystemTrace(moduleId) {
    const module = this._moduleSystem.modules.get(moduleId);
    const context = this._moduleSystem.moduleContexts.get(moduleId);
    
    if (!module || !context) {
      return null;
    }
    
    return {
      module: {
        id: module.moduleId,
        name: module.moduleName,
        state: module.currentState,
        performance: module.getPerformanceMetrics(),
        errors: module._errorHandler.getErrorCount()
      },
      context: {
        id: context.contextId,
        name: context.contextName,
        state: context.currentState,
        performance: context.getPerformanceMetrics(),
        resources: context.getResourceUsage()
      },
      integration: {
        eventBridgeActive: true,
        stateSynchronized: module._executionContext === context,
        bidirectionalAccess: !!module._executionContext && !!context._moduleReference
      },
      crossSystemMetrics: {
        moduleToContextLatency: this._calculateIntegrationLatency(module, context),
        resourceSharing: this._checkResourceSharing(module, context),
        errorPropagation: this._checkErrorPropagation(module, context)
      }
    };
  }
  
  /** @private */
  _calculateIntegrationLatency(module, context) {
    // Simplified latency calculation
    return {
      eventPropagation: 'immediate',
      stateSync: '1s interval',
      errorPropagation: 'immediate'
    };
  }
  
  /** @private */
  _checkResourceSharing(module, context) {
    const moduleResources = module._resourceManager?.getResourceStats() || {};
    const contextResources = context.getResourceUsage() || {};
    
    return {
      shared: Object.keys(moduleResources).filter(key => 
        contextResources[key] !== undefined
      ).length,
      moduleOnly: Object.keys(moduleResources).filter(key => 
        contextResources[key] === undefined
      ).length,
      contextOnly: Object.keys(contextResources).filter(key => 
        moduleResources[key] === undefined
      ).length
    };
  }
  
  /** @private */
  _checkErrorPropagation(module, context) {
    const moduleErrors = module._errorHandler?.errors || [];
    const contextErrors = context._errorHandler?.errors || [];
    
    const propagatedErrors = moduleErrors.filter(me => 
      contextErrors.some(ce => 
        ce.error?.message === me.error?.message
      )
    );
    
    return {
      moduleErrors: moduleErrors.length,
      contextErrors: contextErrors.length,
      propagatedErrors: propagatedErrors.length,
      propagationRate: moduleErrors.length > 0 ? 
        (propagatedErrors.length / moduleErrors.length) * 100 : 0
    };
  }
}

// Export the fully integrated system
export default QuantumCore;
