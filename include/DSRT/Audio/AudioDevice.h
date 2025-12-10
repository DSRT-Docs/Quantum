/**
 * @file AudioDevice.h
 * @brief Audio device management and 3D audio spatialization
 * @ingroup Audio
 * 
 * @details Provides cross-platform audio device initialization,
 *          3D audio positioning, and global audio settings.
 */

#pragma once
#include "../Export.h"
#include "../Math/Vector3.h"
#include "../Core/Types.h"
#include <string>
#include <vector>

namespace DSRT {
namespace Audio {

/**
 * @enum AudioFormat
 * @brief Audio data format specification
 * @ingroup Audio
 */
enum class AudioFormat {
    Unknown,    /**< Unknown/unspecified format */
    Mono8,      /**< 8-bit mono (unsigned) */
    Mono16,     /**< 16-bit mono (signed) */
    Stereo8,    /**< 8-bit stereo (unsigned) */
    Stereo16    /**< 16-bit stereo (signed) */
};

/**
 * @struct AudioDeviceInfo
 * @brief Information about an audio device
 * @ingroup Audio
 */
struct DSRT_API AudioDeviceInfo {
    std::string name;     /**< Device name */
    std::string driver;   /**< Driver/backend name */
    int maxChannels;      /**< Maximum simultaneous channels */
    int sampleRate;       /**< Native sample rate (Hz) */
    bool isDefault;       /**< True if system default device */
};

/**
 * @class AudioDevice
 * @brief Abstract audio device interface
 * @ingroup Audio
 * 
 * @details Manages audio hardware, listener properties, and global settings.
 *          Implemented by platform-specific backends (OpenAL, XAudio2, etc.).
 * 
 * @example Basic audio device usage:
 * @code
 * auto device = AudioDevice::Create();
 * device->Initialize();
 * 
 * device->SetListenerPosition(cameraPos);
 * device->SetListenerOrientation(cameraForward, cameraUp);
 * device->SetMasterVolume(0.8f);
 * 
 * auto sound = Sound::Create("explosion.wav");
 * sound->Play();
 * @endcode
 */
class DSRT_API AudioDevice {
public:
    /**
     * @brief Create audio device instance
     * @return Shared pointer to audio device
     * 
     * @details Factory method that creates appropriate backend
     *          for current platform (OpenAL on desktop, etc.)
     */
    static Ref<AudioDevice> Create();
    
    virtual ~AudioDevice() = default;
    
    /**
     * @brief Initialize audio device
     * @return true if initialization succeeded
     * 
     * @details Opens audio device, creates context, sets up mixing.
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Shutdown audio device
     * 
     * @details Closes device, releases all resources.
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Get available audio devices
     * @return List of available devices
     */
    virtual std::vector<AudioDeviceInfo> GetAvailableDevices() const = 0;
    
    /**
     * @brief Set active audio device
     * @param deviceName Name of device to use
     * @return true if device was switched successfully
     * 
     * @note May cause audio interruption during switch.
     */
    virtual bool SetDevice(const std::string& deviceName) = 0;
    
    // Listener (camera) properties
    
    /**
     * @brief Set listener position in 3D space
     * @param position World-space position
     */
    virtual void SetListenerPosition(const Math::Vector3& position) = 0;
    
    /**
     * @brief Set listener orientation
     * @param forward Forward direction vector (normalized)
     * @param up Up direction vector (normalized)
     */
    virtual void SetListenerOrientation(const Math::Vector3& forward, 
                                       const Math::Vector3& up) = 0;
    
    /**
     * @brief Set listener velocity for Doppler effect
     * @param velocity Velocity vector (units per second)
     */
    virtual void SetListenerVelocity(const Math::Vector3& velocity) = 0;
    
    // Global audio settings
    
    /**
     * @brief Set master volume
     * @param volume Volume [0.0 = silent, 1.0 = full]
     */
    virtual void SetMasterVolume(float volume) = 0;
    
    /**
     * @brief Get master volume
     * @return Current master volume [0.0, 1.0]
     */
    virtual float GetMasterVolume() const = 0;
    
    /**
     * @brief Set Doppler effect factor
     * @param factor Doppler factor [0.0 = disabled, 1.0 = normal, >1.0 = exaggerated]
     */
    virtual void SetDopplerFactor(float factor) = 0;
    
    /**
     * @brief Get Doppler factor
     * @return Current Doppler factor
     */
    virtual float GetDopplerFactor() const = 0;
    
    /**
     * @brief Set speed of sound
     * @param speed Speed in world units per second
     * 
     * @note Default is 343.0 (meters per second in air).
     */
    virtual void SetSpeedOfSound(float speed) = 0;
    
    /**
     * @brief Get speed of sound
     * @return Current speed of sound setting
     */
    virtual float GetSpeedOfSound() const = 0;
    
    // Statistics
    
    /**
     * @brief Get number of active sound instances
     * @return Count of currently playing sounds
     */
    virtual size_t GetActiveSoundCount() const = 0;
    
    /**
     * @brief Get maximum simultaneous channels
     * @return Maximum sounds that can play concurrently
     */
    virtual size_t GetMaxChannels() const = 0;
    
    // Utility
    
    /**
     * @brief Check if device is initialized
     * @return true if device is ready for use
     */
    virtual bool IsInitialized() const = 0;
    
    /**
     * @brief Get backend name
     * @return Backend identifier string (e.g., "OpenAL", "XAudio2")
     */
    virtual const char* GetBackendName() const = 0;
    
protected:
    AudioDevice() = default;
};

} // namespace Audio
} // namespace DSRT
