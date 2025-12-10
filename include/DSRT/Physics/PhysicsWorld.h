/**
 * @file PhysicsWorld.h
 * @brief 3D physics simulation world
 * @ingroup Physics
 * 
 * @details Provides rigid body dynamics, collision detection,
 *          raycasting, and physics queries.
 */

#pragma once
#include "../Export.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Core/Types.h"
#include <memory>
#include <vector>

namespace DSRT {
namespace Physics {

/**
 * @enum BodyType
 * @brief Rigid body type
 * @ingroup Physics
 */
enum class BodyType {
    Static,     /**< Immovable object (terrain, walls) */
    Dynamic,    /**< Fully simulated object */
    Kinematic   /**< Movable but not affected by forces */
};

/**
 * @enum ShapeType
 * @brief Collision shape type
 * @ingroup Physics
 */
enum class ShapeType {
    Sphere,     /**< Sphere shape */
    Box,        /**< Axis-aligned bounding box */
    Capsule,    /**< Capsule shape */
    Cylinder,   /**< Cylinder shape */
    Mesh,       /**< Triangle mesh */
    Heightfield,/**< Heightfield terrain */
    Compound    /**< Compound of multiple shapes */
};

/**
 * @struct RaycastHit
 * @brief Raycast hit information
 * @ingroup Physics
 */
struct DSRT_API RaycastHit {
    bool hit = false;                      /**< True if ray hit something */
    Math::Vector3 point;                   /**< Hit point in world space */
    Math::Vector3 normal;                  /**< Surface normal at hit point */
    float distance = 0.0f;                 /**< Distance from ray origin */
    uint64_t bodyId = 0;                   /**< Hit body ID */
    void* userData = nullptr;              /**< User data attached to body */
    
    /**
     * @brief Check if hit is valid
     * @return true if hit occurred
     */
    bool IsValid() const { return hit && bodyId != 0; }
};

/**
 * @struct SweepResult
 * @brief Shape sweep test result
 * @ingroup Physics
 */
struct DSRT_API SweepResult {
    bool hit = false;                      /**< True if collision occurred */
    Math::Vector3 point;                   /**< Contact point */
    Math::Vector3 normal;                  /**< Contact normal */
    float fraction = 1.0f;                 /**< Fraction of movement before hit */
    uint64_t bodyId = 0;                   /**< Hit body ID */
};

/**
 * @struct PhysicsStats
 * @brief Physics simulation statistics
 * @ingroup Physics
 */
struct DSRT_API PhysicsStats {
    uint32_t bodyCount = 0;                /**< Number of rigid bodies */
    uint32_t activeBodyCount = 0;          /**< Number of active bodies */
    uint32_t collisionPairCount = 0;       /**< Number of collision pairs */
    uint32_t constraintCount = 0;          /**< Number of constraints */
    float simulationTime = 0.0f;           /**< Time spent on simulation */
    float collisionTime = 0.0f;            /**< Time spent on collision detection */
    float solverTime = 0.0f;               /**< Time spent on constraint solving */
};

/**
 * @class PhysicsWorld
 * @brief 3D physics simulation world
 * @ingroup Physics
 * 
 * @details Manages rigid bodies, collision detection, and physics simulation.
 *          Uses bullet3 or similar physics engine internally.
 * 
 * @example Creating a physics world:
 * @code
 * auto physics = PhysicsWorld::Create();
 * physics->SetGravity(Math::Vector3(0, -9.81f, 0));
 * 
 * // Add ground plane
 * uint64_t ground = physics->CreateStaticBody(
 *     Math::Vector3(0, 0, 0),
 *     Math::Quaternion::Identity(),
 *     ShapeType::Box,
 *     Math::Vector3(100, 1, 100)
 * );
 * 
 * // Add dynamic box
 * uint64_t box = physics->CreateDynamicBody(
 *     Math::Vector3(0, 10, 0),
 *     Math::Quaternion::Identity(),
 *     ShapeType::Box,
 *     Math::Vector3(1, 1, 1),
 *     1.0f  // mass
 * );
 * 
 * // Simulation loop
 * physics->StepSimulation(deltaTime);
 * 
 * Math::Vector3 boxPos = physics->GetBodyPosition(box);
 * Math::Quaternion boxRot = physics->GetBodyRotation(box);
 * @endcode
 */
class DSRT_API PhysicsWorld {
public:
    /**
     * @brief Create physics world
     * @return Shared pointer to physics world
     */
    static Ref<PhysicsWorld> Create();
    
    virtual ~PhysicsWorld() = default;
    
    /**
     * @brief Initialize physics world
     * @return true if initialization succeeded
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Shutdown physics world
     */
    virtual void Shutdown() = 0;
    
    // World configuration
    
    /**
     * @brief Set gravity vector
     * @param gravity Gravity acceleration
     */
    virtual void SetGravity(const Math::Vector3& gravity) = 0;
    
    /**
     * @brief Get gravity vector
     * @return Current gravity setting
     */
    virtual Math::Vector3 GetGravity() const = 0;
    
    /**
     * @brief Set simulation substeps
     * @param substeps Number of substeps per frame
     */
    virtual void SetSubsteps(uint32_t substeps) = 0;
    
    /**
     * @brief Get simulation substeps
     * @return Current substep count
     */
    virtual uint32_t GetSubsteps() const = 0;
    
    // Body management
    
    /**
     * @brief Create static rigid body
     * @param position Initial position
     * @param rotation Initial rotation
     * @param shapeType Collision shape type
     * @param shapeData Shape-specific data
     * @param userData Optional user data pointer
     * @return Body ID, or 0 on failure
     */
    virtual uint64_t CreateStaticBody(
        const Math::Vector3& position,
        const Math::Quaternion& rotation,
        ShapeType shapeType,
        const void* shapeData,
        void* userData = nullptr
    ) = 0;
    
    /**
     * @brief Create dynamic rigid body
     * @param position Initial position
     * @param rotation Initial rotation
     * @param shapeType Collision shape type
     * @param shapeData Shape-specific data
     * @param mass Mass in kilograms
     * @param userData Optional user data pointer
     * @return Body ID, or 0 on failure
     */
    virtual uint64_t CreateDynamicBody(
        const Math::Vector3& position,
        const Math::Quaternion& rotation,
        ShapeType shapeType,
        const void* shapeData,
        float mass = 1.0f,
        void* userData = nullptr
    ) = 0;
    
    /**
     * @brief Create kinematic rigid body
     * @param position Initial position
     * @param rotation Initial rotation
     * @param shapeType Collision shape type
     * @param shapeData Shape-specific data
     * @param userData Optional user data pointer
     * @return Body ID, or 0 on failure
     */
    virtual uint64_t CreateKinematicBody(
        const Math::Vector3& position,
        const Math::Quaternion& rotation,
        ShapeType shapeType,
        const void* shapeData,
        void* userData = nullptr
    ) = 0;
    
    /**
     * @brief Destroy rigid body
     * @param bodyId Body ID to destroy
     * @return true if body was found and destroyed
     */
    virtual bool DestroyBody(uint64_t bodyId) = 0;
    
    /**
     * @brief Get number of bodies in world
     * @return Body count
     */
    virtual uint32_t GetBodyCount() const = 0;
    
    // Body properties
    
    /**
     * @brief Get body position
     * @param bodyId Body ID
     * @return World position
     */
    virtual Math::Vector3 GetBodyPosition(uint64_t bodyId) const = 0;
    
    /**
     * @brief Get body rotation
     * @param bodyId Body ID
     * @return World rotation
     */
    virtual Math::Quaternion GetBodyRotation(uint64_t bodyId) const = 0;
    
    /**
     * @brief Get body transform
     * @param bodyId Body ID
     * @param[out] position World position
     * @param[out] rotation World rotation
     * @return true if body exists
     */
    virtual bool GetBodyTransform(uint64_t bodyId, Math::Vector3& position, Math::Quaternion& rotation) const = 0;
    
    /**
     * @brief Set body position
     * @param bodyId Body ID
     * @param position New position
     * @return true if body exists and was moved
     */
    virtual bool SetBodyPosition(uint64_t bodyId, const Math::Vector3& position) = 0;
    
    /**
     * @brief Set body rotation
     * @param bodyId Body ID
     * @param rotation New rotation
     * @return true if body exists and was rotated
     */
    virtual bool SetBodyRotation(uint64_t bodyId, const Math::Quaternion& rotation) = 0;
    
    /**
     * @brief Set body transform
     * @param bodyId Body ID
     * @param position New position
     * @param rotation New rotation
     * @return true if body exists and was transformed
     */
    virtual bool SetBodyTransform(uint64_t bodyId, const Math::Vector3& position, const Math::Quaternion& rotation) = 0;
    
    /**
     * @brief Get body linear velocity
     * @param bodyId Body ID
     * @return Linear velocity vector
     */
    virtual Math::Vector3 GetBodyLinearVelocity(uint64_t bodyId) const = 0;
    
    /**
     * @brief Get body angular velocity
     * @param bodyId Body ID
     * @return Angular velocity vector
     */
    virtual Math::Vector3 GetBodyAngularVelocity(uint64_t bodyId) const = 0;
    
    /**
     * @brief Set body linear velocity
     * @param bodyId Body ID
     * @param velocity New linear velocity
     * @return true if body exists and velocity was set
     */
    virtual bool SetBodyLinearVelocity(uint64_t bodyId, const Math::Vector3& velocity) = 0;
    
    /**
     * @brief Set body angular velocity
     * @param bodyId Body ID
     * @param velocity New angular velocity
     * @return true if body exists and velocity was set
     */
    virtual bool SetBodyAngularVelocity(uint64_t bodyId, const Math::Vector3& velocity) = 0;
    
    /**
     * @brief Apply impulse to body
     * @param bodyId Body ID
     * @param impulse Impulse vector
     * @param point Application point in world space
     * @return true if impulse was applied
     */
    virtual bool ApplyImpulse(uint64_t bodyId, const Math::Vector3& impulse, const Math::Vector3& point = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Apply force to body
     * @param bodyId Body ID
     * @param force Force vector
     * @param point Application point in world space
     * @return true if force was applied
     */
    virtual bool ApplyForce(uint64_t bodyId, const Math::Vector3& force, const Math::Vector3& point = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Apply torque to body
     * @param bodyId Body ID
     * @param torque Torque vector
     * @return true if torque was applied
     */
    virtual bool ApplyTorque(uint64_t bodyId, const Math::Vector3& torque) = 0;
    
    /**
     * @brief Set body damping
     * @param bodyId Body ID
     * @param linearDamp Linear damping coefficient
     * @param angularDamp Angular damping coefficient
     * @return true if damping was set
     */
    virtual bool SetBodyDamping(uint64_t bodyId, float linearDamp, float angularDamp) = 0;
    
    /**
     * @brief Set body restitution
     * @param bodyId Body ID
     * @param restitution Bounciness coefficient [0.0, 1.0]
     * @return true if restitution was set
     */
    virtual bool SetBodyRestitution(uint64_t bodyId, float restitution) = 0;
    
    /**
     * @brief Set body friction
     * @param bodyId Body ID
     * @param friction Friction coefficient
     * @return true if friction was set
     */
    virtual bool SetBodyFriction(uint64_t bodyId, float friction) = 0;
    
/**
 * @file PhysicsWorld.h
 * @brief 3D physics simulation world
 * @ingroup Physics
 * 
 * @details Provides rigid body dynamics, collision detection,
 *          raycasting, and physics queries.
 */

#pragma once
#include "../Export.h"
#include "../Math/Vector3.h"
#include "../Math/Quaternion.h"
#include "../Core/Types.h"
#include <memory>
#include <vector>

namespace DSRT {
namespace Physics {

/**
 * @enum BodyType
 * @brief Rigid body type
 * @ingroup Physics
 */
enum class BodyType {
    Static,     /**< Immovable object (terrain, walls) */
    Dynamic,    /**< Fully simulated object */
    Kinematic   /**< Movable but not affected by forces */
};

/**
 * @enum ShapeType
 * @brief Collision shape type
 * @ingroup Physics
 */
enum class ShapeType {
    Sphere,     /**< Sphere shape */
    Box,        /**< Axis-aligned bounding box */
    Capsule,    /**< Capsule shape */
    Cylinder,   /**< Cylinder shape */
    Mesh,       /**< Triangle mesh */
    Heightfield,/**< Heightfield terrain */
    Compound    /**< Compound of multiple shapes */
};

/**
 * @struct RaycastHit
 * @brief Raycast hit information
 * @ingroup Physics
 */
struct DSRT_API RaycastHit {
    bool hit = false;                      /**< True if ray hit something */
    Math::Vector3 point;                   /**< Hit point in world space */
    Math::Vector3 normal;                  /**< Surface normal at hit point */
    float distance = 0.0f;                 /**< Distance from ray origin */
    uint64_t bodyId = 0;                   /**< Hit body ID */
    void* userData = nullptr;              /**< User data attached to body */
    
    /**
     * @brief Check if hit is valid
     * @return true if hit occurred
     */
    bool IsValid() const { return hit && bodyId != 0; }
};

/**
 * @struct SweepResult
 * @brief Shape sweep test result
 * @ingroup Physics
 */
struct DSRT_API SweepResult {
    bool hit = false;                      /**< True if collision occurred */
    Math::Vector3 point;                   /**< Contact point */
    Math::Vector3 normal;                  /**< Contact normal */
    float fraction = 1.0f;                 /**< Fraction of movement before hit */
    uint64_t bodyId = 0;                   /**< Hit body ID */
};

/**
 * @struct PhysicsStats
 * @brief Physics simulation statistics
 * @ingroup Physics
 */
struct DSRT_API PhysicsStats {
    uint32_t bodyCount = 0;                /**< Number of rigid bodies */
    uint32_t activeBodyCount = 0;          /**< Number of active bodies */
    uint32_t collisionPairCount = 0;       /**< Number of collision pairs */
    uint32_t constraintCount = 0;          /**< Number of constraints */
    float simulationTime = 0.0f;           /**< Time spent on simulation */
    float collisionTime = 0.0f;            /**< Time spent on collision detection */
    float solverTime = 0.0f;               /**< Time spent on constraint solving */
};

/**
 * @class PhysicsWorld
 * @brief 3D physics simulation world
 * @ingroup Physics
 * 
 * @details Manages rigid bodies, collision detection, and physics simulation.
 *          Uses bullet3 or similar physics engine internally.
 * 
 * @example Creating a physics world:
 * @code
 * auto physics = PhysicsWorld::Create();
 * physics->SetGravity(Math::Vector3(0, -9.81f, 0));
 * 
 * // Add ground plane
 * uint64_t ground = physics->CreateStaticBody(
 *     Math::Vector3(0, 0, 0),
 *     Math::Quaternion::Identity(),
 *     ShapeType::Box,
 *     Math::Vector3(100, 1, 100)
 * );
 * 
 * // Add dynamic box
 * uint64_t box = physics->CreateDynamicBody(
 *     Math::Vector3(0, 10, 0),
 *     Math::Quaternion::Identity(),
 *     ShapeType::Box,
 *     Math::Vector3(1, 1, 1),
 *     1.0f  // mass
 * );
 * 
 * // Simulation loop
 * physics->StepSimulation(deltaTime);
 * 
 * Math::Vector3 boxPos = physics->GetBodyPosition(box);
 * Math::Quaternion boxRot = physics->GetBodyRotation(box);
 * @endcode
 */
class DSRT_API PhysicsWorld {
public:
    /**
     * @brief Create physics world
     * @return Shared pointer to physics world
     */
    static Ref<PhysicsWorld> Create();
    
    virtual ~PhysicsWorld() = default;
    
    /**
     * @brief Initialize physics world
     * @return true if initialization succeeded
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Shutdown physics world
     */
    virtual void Shutdown() = 0;
    
    // World configuration
    
    /**
     * @brief Set gravity vector
     * @param gravity Gravity acceleration
     */
    virtual void SetGravity(const Math::Vector3& gravity) = 0;
    
    /**
     * @brief Get gravity vector
     * @return Current gravity setting
     */
    virtual Math::Vector3 GetGravity() const = 0;
    
    /**
     * @brief Set simulation substeps
     * @param substeps Number of substeps per frame
     */
    virtual void SetSubsteps(uint32_t substeps) = 0;
    
    /**
     * @brief Get simulation substeps
     * @return Current substep count
     */
    virtual uint32_t GetSubsteps() const = 0;
    
    // Body management
    
    /**
     * @brief Create static rigid body
     * @param position Initial position
     * @param rotation Initial rotation
     * @param shapeType Collision shape type
     * @param shapeData Shape-specific data
     * @param userData Optional user data pointer
     * @return Body ID, or 0 on failure
     */
    virtual uint64_t CreateStaticBody(
        const Math::Vector3& position,
        const Math::Quaternion& rotation,
        ShapeType shapeType,
        const void* shapeData,
        void* userData = nullptr
    ) = 0;
    
    /**
     * @brief Create dynamic rigid body
     * @param position Initial position
     * @param rotation Initial rotation
     * @param shapeType Collision shape type
     * @param shapeData Shape-specific data
     * @param mass Mass in kilograms
     * @param userData Optional user data pointer
     * @return Body ID, or 0 on failure
     */
    virtual uint64_t CreateDynamicBody(
        const Math::Vector3& position,
        const Math::Quaternion& rotation,
        ShapeType shapeType,
        const void* shapeData,
        float mass = 1.0f,
        void* userData = nullptr
    ) = 0;
    
    /**
     * @brief Create kinematic rigid body
     * @param position Initial position
     * @param rotation Initial rotation
     * @param shapeType Collision shape type
     * @param shapeData Shape-specific data
     * @param userData Optional user data pointer
     * @return Body ID, or 0 on failure
     */
    virtual uint64_t CreateKinematicBody(
        const Math::Vector3& position,
        const Math::Quaternion& rotation,
        ShapeType shapeType,
        const void* shapeData,
        void* userData = nullptr
    ) = 0;
    
    /**
     * @brief Destroy rigid body
     * @param bodyId Body ID to destroy
     * @return true if body was found and destroyed
     */
    virtual bool DestroyBody(uint64_t bodyId) = 0;
    
    /**
     * @brief Get number of bodies in world
     * @return Body count
     */
    virtual uint32_t GetBodyCount() const = 0;
    
    // Body properties
    
    /**
     * @brief Get body position
     * @param bodyId Body ID
     * @return World position
     */
    virtual Math::Vector3 GetBodyPosition(uint64_t bodyId) const = 0;
    
    /**
     * @brief Get body rotation
     * @param bodyId Body ID
     * @return World rotation
     */
    virtual Math::Quaternion GetBodyRotation(uint64_t bodyId) const = 0;
    
    /**
     * @brief Get body transform
     * @param bodyId Body ID
     * @param[out] position World position
     * @param[out] rotation World rotation
     * @return true if body exists
     */
    virtual bool GetBodyTransform(uint64_t bodyId, Math::Vector3& position, Math::Quaternion& rotation) const = 0;
    
    /**
     * @brief Set body position
     * @param bodyId Body ID
     * @param position New position
     * @return true if body exists and was moved
     */
    virtual bool SetBodyPosition(uint64_t bodyId, const Math::Vector3& position) = 0;
    
    /**
     * @brief Set body rotation
     * @param bodyId Body ID
     * @param rotation New rotation
     * @return true if body exists and was rotated
     */
    virtual bool SetBodyRotation(uint64_t bodyId, const Math::Quaternion& rotation) = 0;
    
    /**
     * @brief Set body transform
     * @param bodyId Body ID
     * @param position New position
     * @param rotation New rotation
     * @return true if body exists and was transformed
     */
    virtual bool SetBodyTransform(uint64_t bodyId, const Math::Vector3& position, const Math::Quaternion& rotation) = 0;
    
    /**
     * @brief Get body linear velocity
     * @param bodyId Body ID
     * @return Linear velocity vector
     */
    virtual Math::Vector3 GetBodyLinearVelocity(uint64_t bodyId) const = 0;
    
    /**
     * @brief Get body angular velocity
     * @param bodyId Body ID
     * @return Angular velocity vector
     */
    virtual Math::Vector3 GetBodyAngularVelocity(uint64_t bodyId) const = 0;
    
    /**
     * @brief Set body linear velocity
     * @param bodyId Body ID
     * @param velocity New linear velocity
     * @return true if body exists and velocity was set
     */
    virtual bool SetBodyLinearVelocity(uint64_t bodyId, const Math::Vector3& velocity) = 0;
    
    /**
     * @brief Set body angular velocity
     * @param bodyId Body ID
     * @param velocity New angular velocity
     * @return true if body exists and velocity was set
     */
    virtual bool SetBodyAngularVelocity(uint64_t bodyId, const Math::Vector3& velocity) = 0;
    
    /**
     * @brief Apply impulse to body
     * @param bodyId Body ID
     * @param impulse Impulse vector
     * @param point Application point in world space
     * @return true if impulse was applied
     */
    virtual bool ApplyImpulse(uint64_t bodyId, const Math::Vector3& impulse, const Math::Vector3& point = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Apply force to body
     * @param bodyId Body ID
     * @param force Force vector
     * @param point Application point in world space
     * @return true if force was applied
     */
    virtual bool ApplyForce(uint64_t bodyId, const Math::Vector3& force, const Math::Vector3& point = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Apply torque to body
     * @param bodyId Body ID
     * @param torque Torque vector
     * @return true if torque was applied
     */
    virtual bool ApplyTorque(uint64_t bodyId, const Math::Vector3& torque) = 0;
    
    /**
     * @brief Set body damping
     * @param bodyId Body ID
     * @param linearDamp Linear damping coefficient
     * @param angularDamp Angular damping coefficient
     * @return true if damping was set
     */
    virtual bool SetBodyDamping(uint64_t bodyId, float linearDamp, float angularDamp) = 0;
    
    /**
     * @brief Set body restitution
     * @param bodyId Body ID
     * @param restitution Bounciness coefficient [0.0, 1.0]
     * @return true if restitution was set
     */
    virtual bool SetBodyRestitution(uint64_t bodyId, float restitution) = 0;
    
    /**
     * @brief Set body friction
     * @param bodyId Body ID
     * @param friction Friction coefficient
     * @return true if friction was set
     */
    virtual bool SetBodyFriction(uint64_t bodyId, float friction) = 0;
    
    /**
     * @brief Activate or deactivate body
     * @param bodyId Body ID
     * @param active true to activate
     * @return true if activation state was changed
     */
    virtual bool SetBodyActive(uint64_t bodyId, bool active) = 0;
    
    // Simulation
    
    /**
     * @brief Step physics simulation
     * @param deltaTime Time step in seconds
     * 
     * @details Advances simulation by deltaTime seconds.
     *          Includes collision detection and constraint solving.
     */
    virtual void StepSimulation(float deltaTime) = 0;
    
    /**
     * @brief Debug draw physics world
     * @param debugDrawer Debug drawer implementation
     */
    virtual void DebugDraw(class DebugDrawer* debugDrawer) = 0;
    
    // Queries
    
    /**
     * @brief Raycast into physics world
     * @param origin Ray origin
     * @param direction Ray direction (normalized)
     * @param maxDistance Maximum ray distance
     * @param mask Collision filter mask
     * @return Raycast hit information
     */
    virtual RaycastHit Raycast(const Math::Vector3& origin, const Math::Vector3& direction, 
                              float maxDistance, uint32_t mask = 0xFFFFFFFF) const = 0;
    
    /**
     * @brief Raycast all hits along ray
     * @param origin Ray origin
     * @param direction Ray direction (normalized)
     * @param maxDistance Maximum ray distance
     * @param mask Collision filter mask
     * @return Vector of all hits sorted by distance
     */
    virtual std::vector<RaycastHit> RaycastAll(const Math::Vector3& origin, const Math::Vector3& direction,
                                              float maxDistance, uint32_t mask = 0xFFFFFFFF) const = 0;
    
    /**
     * @brief Sweep test for shape
     * @param shapeType Shape type
     * @param shapeData Shape-specific data
     * @param start Start transform
     * @param end End transform
     * @param mask Collision filter mask
     * @return Sweep result
     */
    virtual SweepResult SweepTest(ShapeType shapeType, const void* shapeData,
                                 const Math::Vector3& start, const Math::Vector3& end,
                                 uint32_t mask = 0xFFFFFFFF) const = 0;
    
    /**
     * @brief Check overlap between shapes
     * @param shapeType1 First shape type
     * @param shapeData1 First shape data
     * @param pos1 First shape position
     * @param rot1 First shape rotation
     * @param shapeType2 Second shape type
     * @param shapeData2 Second shape data
     * @param pos2 Second shape position
     * @param rot2 Second shape rotation
     * @return true if shapes overlap
     */
    virtual bool CheckOverlap(ShapeType shapeType1, const void* shapeData1,
                             const Math::Vector3& pos1, const Math::Quaternion& rot1,
                             ShapeType shapeType2, const void* shapeData2,
                             const Math::Vector3& pos2, const Math::Quaternion& rot2) const = 0;
    
    // Constraints
    
    /**
     * @brief Create point-to-point constraint
     * @param bodyA First body ID
     * @param pivotA Pivot point in body A local space
     * @param bodyB Second body ID (or 0 for world)
     * @param pivotB Pivot point in body B local space
     * @return Constraint ID, or 0 on failure
     */
    virtual uint64_t CreatePointConstraint(uint64_t bodyA, const Math::Vector3& pivotA,
                                          uint64_t bodyB = 0, const Math::Vector3& pivotB = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Create hinge constraint
     * @param bodyA First body ID
     * @param pivotA Pivot point in body A local space
     * @param axisA Hinge axis in body A local space
     * @param bodyB Second body ID (or 0 for world)
     * @param pivotB Pivot point in body B local space
     * @param axisB Hinge axis in body B local space
     * @return Constraint ID, or 0 on failure
     */
    virtual uint64_t CreateHingeConstraint(uint64_t bodyA, const Math::Vector3& pivotA, const Math::Vector3& axisA,
                                          uint64_t bodyB = 0, const Math::Vector3& pivotB = Math::Vector3::Zero(),
                                          const Math::Vector3& axisB = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Create slider constraint
     * @param bodyA First body ID
     * @param frameA Local frame in body A
     * @param bodyB Second body ID (or 0 for world)
     * @param frameB Local frame in body B
     * @return Constraint ID, or 0 on failure
     */
    virtual uint64_t CreateSliderConstraint(uint64_t bodyA, const Math::Vector3& frameA,
                                           uint64_t bodyB = 0, const Math::Vector3& frameB = Math::Vector3::Zero()) = 0;
    
    /**
     * @brief Destroy constraint
     * @param constraintId Constraint ID
     * @return true if constraint was found and destroyed
     */
    virtual bool DestroyConstraint(uint64_t constraintId) = 0;
    
    // Statistics
    
    /**
     * @brief Get physics statistics
     * @return Current simulation statistics
     */
    virtual PhysicsStats GetStats() const = 0;
    
    /**
     * @brief Reset statistics counters
     */
    virtual void ResetStats() = 0;
    
protected:
    PhysicsWorld() = default;
};

} // namespace Physics
} // namespace DSRT
