/**
 * @file Sound.h
 * @brief Sound effect playback with 3D spatialization
 * @ingroup Audio
 * 
 * @details Loads and plays short sound effects with positional audio,
 *          volume control, and playback options.
 */

#pragma once
#include "../Export.h"
#include "../Math/Vector3.h"
#include "../Core/Types.h"
#include <string>
#include <memory>

namespace DSRT {
namespace Audio {

/**
 * @enum SoundState
 * @brief Sound playback state
 * @ingroup Audio
 */
enum class SoundState {
    Stopped,    /**< Not playing */
    Playing,    /**< Currently playing */
    Paused      /**< Playback paused */
};

/**
 * @class Sound
 * @brief Sound effect with 3D spatial audio
 * @ingroup Audio
 * 
 * @details Represents a short audio sample (effects, UI sounds, etc.)
 *          with support for 3D positioning, looping, and attenuation.
 * 
 * @example Playing a sound effect:
 * @code
 * auto sound = Sound::Create("gunshot.wav");
 * sound->SetPosition(playerPosition);
 * sound->SetVolume(0.7f);
 * sound->Play();
 * 
 * // Update position while playing
 * sound->SetPosition(updatedPosition);
 * @endcode
 * 
 * @performance Sounds are streamed from memory for quick playback.
 *              Use Music class for long audio files.
 */
class DSRT_API Sound {
public:
    /**
     * @brief Create sound from audio file
     * @param filepath Path to audio file (WAV, OGG, etc.)
     * @return Shared pointer to sound, or nullptr on failure
     * 
     * @supported_formats Platform-dependent, typically WAV and OGG Vorbis.
     */
    static Ref<Sound> Create(const std::string& filepath);
    
    virtual ~Sound() = default;
    
    /**
     * @brief Play sound from beginning
     * @param loop true to loop continuously
     * @return true if playback started successfully
     */
    virtual bool Play(bool loop = false) = 0;
    
    /**
     * @brief Pause playback
     * @return true if sound was playing and is now paused
     */
    virtual bool Pause() = 0;
    
    /**
     * @brief Resume playback from paused state
     * @return true if sound was paused and is now playing
     */
    virtual bool Resume() = 0;
    
    /**
     * @brief Stop playback and reset to beginning
     * @return true if sound was playing and is now stopped
     */
    virtual bool Stop() = 0;
    
    // Playback control
    
    /**
     * @brief Set playback volume
     * @param volume Volume [0.0 = silent, 1.0 = full]
     */
    virtual void SetVolume(float volume) = 0;
    
    /**
     * @brief Get current volume
     * @return Volume setting [0.0, 1.0]
     */
    virtual float GetVolume() const = 0;
    
    /**
     * @brief Set playback pitch
     * @param pitch Pitch multiplier [0.5 = half speed, 1.0 = normal, 2.0 = double speed]
     */
    virtual void SetPitch(float pitch) = 0;
    
    /**
     * @brief Get current pitch
     * @return Pitch multiplier
     */
    virtual float GetPitch() const = 0;
    
    /**
     * @brief Set looping state
     * @param loop true to loop indefinitely
     */
    virtual void SetLooping(bool loop) = 0;
    
    /**
     * @brief Check if sound is looping
     * @return true if sound is set to loop
     */
    virtual bool IsLooping() const = 0;
    
    // 3D audio properties
    
    /**
     * @brief Set sound position in 3D space
     * @param position World-space position
     * 
     * @details Affects volume and stereo panning based on listener position.
     */
    virtual void SetPosition(const Math::Vector3& position) = 0;
    
    /**
     * @brief Get sound position
     * @return Current world-space position
     */
    virtual Math::Vector3 GetPosition() const = 0;
    
    /**
     * @brief Set sound velocity for Doppler effect
     * @param velocity Velocity vector (units per second)
     */
    virtual void SetVelocity(const Math::Vector3& velocity) = 0;
    
    /**
     * @brief Get sound velocity
     * @return Current velocity vector
     */
    virtual Math::Vector3 GetVelocity() const = 0;
    
    /**
     * @brief Set minimum audible distance
     * @param distance Distance at which volume begins to attenuate
     * 
     * @details Within this distance, volume is at maximum.
     */
    virtual void SetMinDistance(float distance) = 0;
    
    /**
     * @brief Get minimum distance
     * @return Current minimum distance setting
     */
    virtual float GetMinDistance() const = 0;
    
    /**
     * @brief Set maximum audible distance
     * @param distance Distance beyond which sound is inaudible
     */
    virtual void SetMaxDistance(float distance) = 0;
    
    /**
     * @brief Get maximum distance
     * @return Current maximum distance setting
     */
    virtual float GetMaxDistance() const = 0;
    
    // Query methods
    
    /**
     * @brief Get current playback state
     * @return SoundState enum value
     */
    virtual SoundState GetState() const = 0;
    
    /**
     * @brief Check if sound is playing
     * @return true if sound state is Playing
     */
    virtual bool IsPlaying() const = 0;
    
    /**
     * @brief Check if sound is paused
     * @return true if sound state is Paused
     */
    virtual bool IsPaused() const = 0;
    
    /**
     * @brief Check if sound is stopped
     * @return true if sound state is Stopped
     */
    virtual bool IsStopped() const = 0;
    
    /**
     * @brief Get sound duration
     * @return Duration in seconds
     */
    virtual float GetDuration() const = 0;
    
    /**
     * @brief Get current playback position
     * @return Position in seconds from start
     */
    virtual float GetPlaybackPosition() const = 0;
    
    /**
     * @brief Set playback position
     * @param position Position in seconds
     * @return true if position was set successfully
     */
    virtual bool SetPlaybackPosition(float position) = 0;
    
    /**
     * @brief Get audio format
     * @return Format information
     */
    virtual AudioFormat GetFormat() const = 0;
    
    /**
     * @brief Get sample rate
     * @return Samples per second (Hz)
     */
    virtual int GetSampleRate() const = 0;
    
protected:
    Sound() = default;
};

} // namespace Audio
} // namespace DSRT
