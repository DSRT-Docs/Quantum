/**
 * @file Component.h
 * @brief Base class for all Entity-Component-System components
 * @ingroup Scene
 * 
 * @details Components are data-only containers that define attributes
 *          and behaviors of entities. They follow composition-over-inheritance
 *          principle.
 */

#pragma once
#include "../Export.h"
#include <cstdint>

namespace DSRT {
namespace Scene {

/**
 * @class Component
 * @brief Abstract base class for all ECS components
 * @ingroup Scene
 * 
 * @details Components are pure data containers with optional lifecycle methods.
 *          They are attached to entities and processed by systems.
 * 
 * @example Creating a custom component:
 * @code
 * class TransformComponent : public Component {
 * public:
 *     uint32_t GetTypeID() const override { return 1; }
 *     
 *     void OnAttach() override {
 *         position = Vector3::Zero();
 *         rotation = Quaternion::Identity();
 *         scale = Vector3::One();
 *     }
 *     
 *     Vector3 position;
 *     Quaternion rotation;
 *     Vector3 scale;
 * };
 * @endcode
 * 
 * @performance Components should be POD-like for cache efficiency.
 *              Virtual methods are only for lifecycle events.
 */
class DSRT_API Component {
public:
    Component() = default;
    virtual ~Component() = default;
    
    /**
     * @brief Called when component is attached to an entity
     * 
     * @details Use for initialization logic that requires
     *          knowledge of the owning entity.
     */
    virtual void OnAttach() {}
    
    /**
     * @brief Called when component is detached from entity
     * 
     * @details Use for cleanup logic. Called before entity
     *          or component destruction.
     */
    virtual void OnDetach() {}
    
    /**
     * @brief Called each frame if component is active
     * @param dt Delta time in seconds
     * 
     * @details Only called if component's entity is active
     *          and component is enabled.
     */
    virtual void OnUpdate(float dt) {}
    
    /**
     * @brief Get component type identifier
     * @return Unique type ID for this component class
     * 
     * @details Must be implemented by each component class.
     *          Should return a stable, unique integer.
     *          
     * @note Use compile-time type ID generation for safety:
     *       static uint32_t GetStaticTypeID() { return TypeID; }
     */
    virtual uint32_t GetTypeID() const = 0;
    
    /**
     * @brief Get ID of owning entity
     * @return Entity ID or InvalidHandle if not attached
     */
    uint64_t GetEntityId() const { return m_EntityId; }
    
    /**
     * @brief Set owning entity ID
     * @param id Entity ID
     * @internal Called by Entity class
     */
    void SetEntityId(uint64_t id) { m_EntityId = id; }
    
private:
    uint64_t m_EntityId = 0; /**< ID of owning entity */
};

} // namespace Scene
} // namespace DSRT
