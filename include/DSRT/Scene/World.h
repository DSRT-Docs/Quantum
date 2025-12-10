/**
 * @file World.h
 * @brief World/Scene container for ECS architecture
 * @ingroup Scene
 * 
 * @details Manages all entities, components, and systems in a scene.
 *          Provides entity creation, component storage, and system execution.
 */

#pragma once
#include "../Export.h"
#include "Entity.h"
#include "System.h"
#include "../Core/Types.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

namespace DSRT {
namespace Scene {

/**
 * @class World
 * @brief Container for all ECS elements in a scene
 * @ingroup Scene
 * 
 * @details The World is the central manager for the Entity-Component-System.
 *          It handles entity lifecycle, component storage, and system updates.
 * 
 * @example Basic world usage:
 * @code
 * World world;
 * world.SetName("MainScene");
 * world.Initialize();
 * 
 * // Create entities
 * auto player = world.CreateEntity("Player");
 * auto enemy = world.CreateEntity("Enemy");
 * 
 * // Add systems
 * world.AddSystem<RenderSystem>();
 * world.AddSystem<PhysicsSystem>();
 * 
 * // Main loop
 * while (running) {
 *     world.Update(deltaTime);
 *     world.Render();
 * }
 * @endcode
 * 
 * @performance Uses archetype-based component storage for cache efficiency.
 *              Systems process components in contiguous memory blocks.
 */
class DSRT_API World {
public:
    /**
     * @brief Construct a new World
     * @param name Optional world name
     */
    explicit World(const std::string& name = "");
    
    virtual ~World();
    
    // Entity management
    
    /**
     * @brief Create new entity
     * @param name Optional entity name
     * @return Shared pointer to new entity
     * 
     * @details Generates unique ID and registers entity in world.
     */
    Ref<Entity> CreateEntity(const std::string& name = "");
    
    /**
     * @brief Create child entity
     * @param parentId Parent entity ID
     * @param name Optional entity name
     * @return Shared pointer to new entity
     * 
     * @details Creates entity with parent-child relationship.
     *          Child transforms are relative to parent.
     */
    Ref<Entity> CreateEntity(uint64_t parentId, const std::string& name = "");
    
    /**
     * @brief Get entity by ID
     * @param entityId Entity identifier
     * @return Shared pointer to entity, or nullptr if not found
     */
    Ref<Entity> GetEntity(uint64_t entityId);
    
    /**
     * @brief Get entity by ID (const)
     * @param entityId Entity identifier
     * @return Const shared pointer to entity, or nullptr
     */
    const Ref<Entity> GetEntity(uint64_t entityId) const;
    
    /**
     * @brief Destroy entity and all components
     * @param entityId Entity identifier
     * @return true if entity was found and destroyed
     * 
     * @details Recursively destroys child entities.
     *          Calls OnDetach() on all components.
     */
    bool DestroyEntity(uint64_t entityId);
    
    /**
     * @brief Destroy all entities in world
     * 
     * @details Use with caution. Clears entire world state.
     */
    void DestroyAllEntities();
    
    // Entity queries
    
    /**
     * @brief Get all entities in world
     * @return Vector of entity shared pointers
     */
    std::vector<Ref<Entity>> GetEntities() const;
    
    /**
     * @brief Get entities with specific component type
     * @tparam T Component type
     * @return Vector of entities with component
     */
    template<typename T>
    std::vector<Ref<Entity>> GetEntitiesWithComponent() const;
    
    /**
     * @brief Find entity by name
     * @param name Entity name to search for
     * @return First entity with matching name, or nullptr
     */
    Ref<Entity> FindEntityByName(const std::string& name) const;
    
    /**
     * @brief Find entity by tag
     * @param tag Tag to search for
     * @return First entity with matching tag, or nullptr
     * 
     * @note Requires TagComponent or similar system.
     */
    Ref<Entity> FindEntityByTag(const std::string& tag) const;
    
    // System management
    
    /**
     * @brief Add system to world
     * @tparam T System type
     * @tparam Args Constructor argument types
     * @param args Arguments forwarded to system constructor
     * @return Shared pointer to new system
     * 
     * @details Systems are executed in order of addition.
     */
    template<typename T, typename... Args>
    Ref<T> AddSystem(Args&&... args);
    
    /**
     * @brief Remove system by type
     * @tparam T System type
     * @return true if system was found and removed
     */
    template<typename T>
    bool RemoveSystem();
    
    /**
     * @brief Get system by type
     * @tparam T System type
     * @return Shared pointer to system, or nullptr
     */
    template<typename T>
    Ref<T> GetSystem() const;
    
    // World lifecycle
    
    /**
     * @brief Initialize world and all systems
     * 
     * @details Calls Initialize() on all registered systems.
     *          Must be called before Update() or Render().
     */
    void Initialize();
    
    /**
     * @brief Update world state
     * @param deltaTime Time since last update in seconds
     * 
     * @details Calls Update() on all systems in registration order.
     */
    void Update(float deltaTime);
    
    /**
     * @brief Render world
     * 
     * @details Calls Render() on all systems.
     *          Typically called after Update().
     */
    void Render();
    
    /**
     * @brief Shutdown world and cleanup resources
     * 
     * @details Destroys all entities and systems.
     *          Calls Shutdown() on all systems.
     */
    void Shutdown();
    
    // Statistics
    
    /**
     * @brief Get entity count
     * @return Number of entities in world
     */
    size_t GetEntityCount() const { return m_Entities.size(); }
    
    /**
     * @brief Get component count
     * @return Total number of components across all entities
     */
    size_t GetComponentCount() const;
    
    /**
     * @brief Get system count
     * @return Number of registered systems
     */
    size_t GetSystemCount() const { return m_Systems.size(); }
    
    // World properties
    
    /**
     * @brief Set world name
     * @param name New world name
     */
    void SetName(const std::string& name) { m_Name = name; }
    
    /**
     * @brief Get world name
     * @return World name
     */
    const std::string& GetName() const { return m_Name; }
    
    /**
     * @brief Set world active state
     * @param active New active state
     * 
     * @details Inactive worlds don't update or render.
     */
    void SetActive(bool active) { m_Active = active; }
    
    /**
     * @brief Check if world is active
     * @return true if world is active
     */
    bool IsActive() const { return m_Active; }
    
private:
    std::string m_Name;                                 /**< World name */
    bool m_Active = true;                              /**< Active state */
    bool m_Initialized = false;                        /**< Initialization flag */
    
    // Entity storage
    std::unordered_map<uint64_t, Ref<Entity>> m_Entities; /**< Entity map by ID */
    uint64_t m_NextEntityId = 1;                        /**< Next available entity ID */
    
    // Component storage (archetype-based)
    struct Archetype {
        std::vector<uint32_t> componentTypes;           /**< Component type IDs */
        std::vector<std::vector<uint8_t>> componentData; /**< Raw component data */
        std::vector<uint64_t> entityIds;                /**< Entity IDs in this archetype */
    };
    std::vector<Archetype> m_Archetypes;               /**< Archetype storage */
    
    // System storage
    std::vector<Ref<System>> m_Systems;                /**< Registered systems */
    
    // Entity hierarchy
    std::unordered_map<uint64_t, std::vector<uint64_t>> m_Children; /**< Child entities */
    std::unordered_map<uint64_t, uint64_t> m_Parents;              /**< Parent entities */
    
    // Internal methods
    uint64_t GenerateEntityId();
    void DestroyEntityRecursive(uint64_t entityId);
    Archetype* FindOrCreateArchetype(const std::vector<uint32_t>& componentTypes);
    
    friend class Entity;
    
    // Template implementations
    template<typename T>
    std::vector<Ref<Entity>> GetEntitiesWithComponentImpl() const;
    
    template<typename T, typename... Args>
    Ref<T> AddSystemImpl(Args&&... args);
    
    template<typename T>
    bool RemoveSystemImpl();
    
    template<typename T>
    Ref<T> GetSystemImpl() const;
};

// Template implementations

template<typename T>
inline std::vector<Ref<Entity>> World::GetEntitiesWithComponent() const {
    static_assert(std::is_base_of<Component, T>::value, 
                  "T must inherit from Component");
    return GetEntitiesWithComponentImpl<T>();
}

template<typename T, typename... Args>
inline Ref<T> World::AddSystem(Args&&... args) {
    static_assert(std::is_base_of<System, T>::value, 
                  "T must inherit from System");
    return AddSystemImpl<T>(std::forward<Args>(args)...);
}

template<typename T>
inline bool World::RemoveSystem() {
    static_assert(std::is_base_of<System, T>::value, 
                  "T must inherit from System");
    return RemoveSystemImpl<T>();
}

template<typename T>
inline Ref<T> World::GetSystem() const {
    static_assert(std::is_base_of<System, T>::value, 
                  "T must inherit from System");
    return GetSystemImpl<T>();
}

} // namespace Scene
} // namespace DSRT
