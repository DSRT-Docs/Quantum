/**
 * @file Entity.h
 * @brief Entity class for Entity-Component-System
 * @ingroup Scene
 * 
 * @details Entities are lightweight identifiers that aggregate components.
 *          They have no logic of their own - all behavior comes from components.
 */

#pragma once
#include "../Export.h"
#include "Component.h"
#include "../Core/Types.h"
#include <memory>
#include <vector>
#include <string>

namespace DSRT {
namespace Scene {

/**
 * @class Entity
 * @brief Container for components with unique identifier
 * @ingroup Scene
 * 
 * @details Entities are the fundamental objects in the ECS architecture.
 *          They consist of an ID and a collection of components.
 * 
 * @example Creating and using an entity:
 * @code
 * World& world = GetWorld();
 * Ref<Entity> entity = world.CreateEntity("Player");
 * 
 * auto transform = entity->AddComponent<TransformComponent>();
 * transform->position = Vector3(0, 0, 0);
 * 
 * auto renderable = entity->AddComponent<RenderableComponent>();
 * renderable->meshId = LoadMesh("player.obj");
 * @endcode
 * 
 * @performance Entity handles are lightweight (just an ID).
 *              Actual component storage is managed by World.
 */
class DSRT_API Entity {
public:
    /**
     * @brief Construct a new Entity
     * @param world Reference to owning world
     * @param id Unique entity identifier
     * @param name Optional display name
     */
    Entity(class World& world, uint64_t id, const std::string& name = "");
    
    virtual ~Entity();
    
    /**
     * @brief Get entity unique identifier
     * @return Entity ID
     */
    uint64_t GetID() const { return m_Id; }
    
    /**
     * @brief Set entity display name
     * @param name Human-readable name
     * 
     * @details Used for debugging and editor display.
     *          Does not affect entity behavior.
     */
    void SetName(const std::string& name) { m_Name = name; }
    
    /**
     * @brief Get entity display name
     * @return Name string (empty if unnamed)
     */
    const std::string& GetName() const { return m_Name; }
    
    /**
     * @brief Add component to entity
     * @tparam T Component type (must inherit from Component)
     * @tparam Args Constructor argument types
     * @param args Arguments forwarded to component constructor
     * @return Shared pointer to new component
     * 
     * @example
     * @code
     * auto health = entity->AddComponent<HealthComponent>(100.0f);
     * @endcode
     */
    template<typename T, typename... Args>
    Ref<T> AddComponent(Args&&... args);
    
    /**
     * @brief Get component by type
     * @tparam T Component type
     * @return Shared pointer to component, or nullptr if not found
     */
    template<typename T>
    Ref<T> GetComponent() const;
    
    /**
     * @brief Check if entity has component type
     * @tparam T Component type
     * @return true if component exists
     */
    template<typename T>
    bool HasComponent() const;
    
    /**
     * @brief Remove component by type
     * @tparam T Component type
     * @return true if component was removed
     */
    template<typename T>
    bool RemoveComponent();
    
    /**
     * @brief Get all components
     * @return Vector of component shared pointers
     */
    const std::vector<Ref<Component>>& GetComponents() const { return m_Components; }
    
    /**
     * @brief Get number of components
     * @return Component count
     */
    size_t GetComponentCount() const { return m_Components.size(); }
    
    /**
     * @brief Check if entity is active
     * @return true if entity is active and should be processed
     */
    bool IsActive() const { return m_Active; }
    
    /**
     * @brief Set entity active state
     * @param active New active state
     * 
     * @details Inactive entities are skipped by all systems.
     */
    void SetActive(bool active) { m_Active = active; }
    
    /**
     * @brief Get owning world
     * @return Reference to world
     */
    World& GetWorld() { return m_World; }
    const World& GetWorld() const { return m_World; }
    
private:
    World& m_World;                         /**< Owning world */
    uint64_t m_Id;                          /**< Unique identifier */
    std::string m_Name;                     /**< Display name */
    std::vector<Ref<Component>> m_Components; /**< Attached components */
    bool m_Active = true;                   /**< Active state */
    
    // Template implementations
    template<typename T, typename... Args>
    Ref<T> AddComponentImpl(Args&&... args);
    
    template<typename T>
    Ref<T> GetComponentImpl() const;
    
    template<typename T>
    bool HasComponentImpl() const;
    
    template<typename T>
    bool RemoveComponentImpl();
};

// Template implementations (in header for inline expansion)

template<typename T, typename... Args>
inline Ref<T> Entity::AddComponent(Args&&... args) {
    static_assert(std::is_base_of<Component, T>::value, 
                  "T must inherit from Component");
    return AddComponentImpl<T>(std::forward<Args>(args)...);
}

template<typename T>
inline Ref<T> Entity::GetComponent() const {
    static_assert(std::is_base_of<Component, T>::value, 
                  "T must inherit from Component");
    return GetComponentImpl<T>();
}

template<typename T>
inline bool Entity::HasComponent() const {
    static_assert(std::is_base_of<Component, T>::value, 
                  "T must inherit from Component");
    return HasComponentImpl<T>();
}

template<typename T>
inline bool Entity::RemoveComponent() {
    static_assert(std::is_base_of<Component, T>::value, 
                  "T must inherit from Component");
    return RemoveComponentImpl<T>();
}

} // namespace Scene
} // namespace DSRT
