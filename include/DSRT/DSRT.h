/**
 * @file DSRT.h
 * @brief Master include file for DSRT Engine SDK
 * 
 * @details Include this single header to access all DSRT Engine functionality.
 *          For modular builds, include specific module headers instead.
 */

#pragma once

// Core module
#include "Core/Export.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Assert.h"
#include "Core/Config.h"
#include "Core/Platform.h"
#include "Core/Utils.h"

// Math module
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix3.h"
#include "Math/Matrix4.h"
#include "Math/Quaternion.h"
#include "Math/Transform.h"
#include "Math/MathConstants.h"

// Graphics module
#include "Graphics/Renderer.h"
#include "Graphics/Camera.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/GraphicsTypes.h"

// Scene module
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/Transform.h"
#include "Scene/Scene.h"
#include "Scene/World.h"
#include "Scene/System.h"

// Audio module
#include "Audio/AudioDevice.h"
#include "Audio/Sound.h"
#include "Audio/Music.h"

// Input module
#include "Input/InputManager.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Input/Gamepad.h"

// Resource module (optional)
#ifdef DSRT_ENABLE_RESOURCES
#include "Resource/ResourceManager.h"
#include "Resource/TextureLoader.h"
#include "Resource/MeshLoader.h"
#endif

// Physics module (optional)
#ifdef DSRT_ENABLE_PHYSICS
#include "Physics/PhysicsWorld.h"
#include "Physics/RigidBody.h"
#include "Physics/Collision.h"
#endif

// Engine main
#include "Engine.h"

/**
 * @mainpage DSRT Engine SDK Documentation
 * 
 * @section intro Introduction
 * 
 * DSRT Engine is a professional game engine SDK designed for modern C++ development.
 * It provides modular, extensible architecture with comprehensive documentation.
 * 
 * @section getting_started Getting Started
 * 
 * 1. Include DSRT.h in your project
 * 2. Initialize engine with Engine::Initialize()
 * 3. Create entities and components
 * 4. Implement game logic in systems
 * 5. Run main loop with Engine::Update() and Engine::Render()
 * 
 * @section modules Modules
 * 
 * - @ref Core: Fundamental types and utilities
 * - @ref Math: Vector and matrix mathematics
 * - @ref Graphics: Rendering and visual effects
 * - @ref Scene: Entity-Component-System architecture
 * - @ref Audio: Sound and music playback
 * - @ref Input: Keyboard, mouse, and gamepad input
 * - @ref Resource: Asset loading and management
 * - @ref Physics: Rigid body dynamics (optional)
 * 
 * @section examples Examples
 * 
 * See examples/ directory for complete usage examples.
 */
