class System {
    constructor() {
        this.modules = new Map();
        this.services = new Map();
        this.events = new Map();
        this.config = {};
    }

    register(moduleName, module) {
        this.modules.set(moduleName, module);
        return this;
    }

    get(moduleName) {
        return this.modules.get(moduleName);
    }

    has(moduleName) {
        return this.modules.has(moduleName);
    }

    remove(moduleName) {
        return this.modules.delete(moduleName);
    }

    setConfig(config) {
        this.config = { ...this.config, ...config };
        return this;
    }

    getConfig(key) {
        return key ? this.config[key] : this.config;
    }

    on(event, callback) {
        if (!this.events.has(event)) {
            this.events.set(event, []);
        }
        this.events.get(event).push(callback);
        return this;
    }

    emit(event, data) {
        if (this.events.has(event)) {
            this.events.get(event).forEach(callback => callback(data));
        }
        return this;
    }

    service(name, service) {
        this.services.set(name, service);
        return this;
    }

    use(name) {
        return this.services.get(name);
    }

    clear() {
        this.modules.clear();
        this.services.clear();
        this.events.clear();
        this.config = {};
        return this;
    }

    listModules() {
        return Array.from(this.modules.keys());
    }

    listServices() {
        return Array.from(this.services.keys());
    }
}

export default System;
