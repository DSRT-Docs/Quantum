#pragma once

/**
 * @file DSRTTime.h
 * @brief Time and timing utilities
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"

namespace DSRT {
    
    /**
     * @brief High-resolution time measurement
     */
    class HighResolutionTimer {
    public:
        /**
         * @brief Constructs a high-resolution timer
         */
        HighResolutionTimer();
        
        /**
         * @brief Starts the timer
         */
        void Start();
        
        /**
         * @brief Stops the timer
         */
        void Stop();
        
        /**
         * @brief Resets the timer
         */
        void Reset();
        
        /**
         * @brief Gets elapsed time in seconds
         * @return Elapsed time in seconds
         */
        float64 GetElapsedSeconds() const;
        
        /**
         * @brief Gets elapsed time in milliseconds
         * @return Elapsed time in milliseconds
         */
        float64 GetElapsedMilliseconds() const;
        
        /**
         * @brief Gets elapsed time in microseconds
         * @return Elapsed time in microseconds
         */
        float64 GetElapsedMicroseconds() const;
        
        /**
         * @brief Checks if timer is running
         * @return True if timer is running
         */
        bool IsRunning() const;
        
    private:
        uint64 m_startTime;
        uint64 m_stopTime;
        bool m_isRunning;
        static uint64 s_frequency;
        static bool s_frequencyInitialized;
    };
    
    /**
     * @brief Frame timer for game timing
     */
    class FrameTimer {
    public:
        /**
         * @brief Constructs a frame timer
         * @param targetFPS Target frames per second
         */
        explicit FrameTimer(float32 targetFPS = 60.0f);
        
        /**
         * @brief Begins a new frame
         */
        void BeginFrame();
        
        /**
         * @brief Ends the current frame
         */
        void EndFrame();
        
        /**
         * @brief Gets delta time in seconds
         * @return Delta time in seconds
         */
        float32 GetDeltaTime() const;
        
        /**
         * @brief Gets smoothed delta time in seconds
         * @return Smoothed delta time in seconds
         */
        float32 GetSmoothedDeltaTime() const;
        
        /**
         * @brief Gets frame rate (FPS)
         * @return Current frame rate
         */
        float32 GetFrameRate() const;
        
        /**
         * @brief Gets smoothed frame rate
         * @return Smoothed frame rate
         */
        float32 GetSmoothedFrameRate() const;
        
        /**
         * @brief Gets total elapsed time
         * @return Total elapsed time in seconds
         */
        float64 GetTotalTime() const;
        
        /**
         * @brief Gets frame count
         * @return Total frame count
         */
        uint64 GetFrameCount() const;
        
        /**
         * @brief Sets target frame rate
         * @param targetFPS Target frames per second
         */
        void SetTargetFrameRate(float32 targetFPS);
        
        /**
         * @brief Gets target frame rate
         * @return Target frames per second
         */
        float32 GetTargetFrameRate() const;
        
        /**
         * @brief Gets target delta time
         * @return Target delta time in seconds
         */
        float32 GetTargetDeltaTime() const;
        
        /**
         * @brief Sleeps to maintain target frame rate
         */
        void SleepForFrameRate() const;
        
        /**
         * @brief Checks if frame rate is stable
         * @return True if frame rate is within tolerance
         */
        bool IsFrameRateStable() const;
        
    private:
        HighResolutionTimer m_timer;
        float32 m_deltaTime;
        float32 m_smoothedDeltaTime;
        float32 m_frameRate;
        float32 m_smoothedFrameRate;
        float64 m_totalTime;
        uint64 m_frameCount;
        float32 m_targetFPS;
        float32 m_targetDeltaTime;
        float32 m_lastFrameTime;
        float32 m_frameTimes[60];
        uint32 m_frameTimeIndex;
    };
    
    /**
     * @brief Date and time utilities
     */
    class DateTime {
    public:
        /**
         * @brief Date structure
         */
        struct Date {
            uint16 year;    // Full year (e.g., 2024)
            uint8 month;    // 1-12
            uint8 day;      // 1-31
            uint8 weekday;  // 0-6 (Sunday=0)
        };
        
        /**
         * @brief Time structure
         */
        struct Time {
            uint8 hour;     // 0-23
            uint8 minute;   // 0-59
            uint8 second;   // 0-59
            uint16 millisecond; // 0-999
        };
        
        /**
         * @brief Gets current date
         * @return Current date
         */
        static Date GetCurrentDate();
        
        /**
         * @brief Gets current time
         * @return Current time
         */
        static Time GetCurrentTime();
        
        /**
         * @brief Gets current date and time as string
         * @param format Format string
         * @return Formatted date/time string
         */
        static const char* GetCurrentDateTimeString(const char* format = "%Y-%m-%d %H:%M:%S");
        
        /**
         * @brief Gets system time in milliseconds since epoch
         * @return Milliseconds since epoch
         */
        static uint64 GetSystemTimeMilliseconds();
        
        /**
         * @brief Gets system time in microseconds since epoch
         * @return Microseconds since epoch
         */
        static uint64 GetSystemTimeMicroseconds();
        
        /**
         * @brief Gets time zone offset in minutes
         * @return Time zone offset from UTC in minutes
         */
        static int32 GetTimeZoneOffset();
        
        /**
         * @brief Checks if daylight saving time is active
         * @return True if DST is active
         */
        static bool IsDaylightSavingTime();
        
        /**
         * @brief Gets system uptime in milliseconds
         * @return Uptime in milliseconds
         */
        static uint64 GetUptimeMilliseconds();
        
        /**
         * @brief Sleeps for specified milliseconds
         * @param milliseconds Time to sleep
         */
        static void Sleep(uint32 milliseconds);
        
        /**
         * @brief Sleeps for specified microseconds
         * @param microseconds Time to sleep
         */
        static void SleepMicroseconds(uint32 microseconds);
        
        /**
         * @brief Gets high-resolution counter value
         * @return Counter value
         */
        static uint64 GetPerformanceCounter();
        
        /**
         * @brief Gets high-resolution counter frequency
         * @return Counter frequency in Hz
         */
        static uint64 GetPerformanceFrequency();
        
    private:
        DateTime() = delete;
        ~DateTime() = delete;
    };
    
    /**
     * @brief Timer utility class
     */
    class Timer {
    public:
        /**
         * @brief Constructs a timer
         * @param duration Timer duration in seconds
         * @param autoReset Whether to auto-reset when expired
         */
        explicit Timer(float32 duration = 1.0f, bool autoReset = false);
        
        /**
         * @brief Starts the timer
         */
        void Start();
        
        /**
         * @brief Stops the timer
         */
        void Stop();
        
        /**
         * @brief Resets the timer
         */
        void Reset();
        
        /**
         * @brief Updates the timer
         * @param deltaTime Time delta in seconds
         */
        void Update(float32 deltaTime);
        
        /**
         * @brief Checks if timer has expired
         * @return True if timer has expired
         */
        bool HasExpired() const;
        
        /**
         * @brief Gets elapsed time
         * @return Elapsed time in seconds
         */
        float32 GetElapsed() const;
        
        /**
         * @brief Gets remaining time
         * @return Remaining time in seconds
         */
        float32 GetRemaining() const;
        
        /**
         * @brief Gets normalized progress (0 to 1)
         * @return Progress from 0.0 to 1.0
         */
        float32 GetProgress() const;
        
        /**
         * @brief Sets timer duration
         * @param duration Duration in seconds
         */
        void SetDuration(float32 duration);
        
        /**
         * @brief Gets timer duration
         * @return Duration in seconds
         */
        float32 GetDuration() const;
        
        /**
         * @brief Sets auto-reset behavior
         * @param autoReset Whether to auto-reset
         */
        void SetAutoReset(bool autoReset);
        
        /**
         * @brief Gets auto-reset behavior
         * @return True if auto-reset is enabled
         */
        bool GetAutoReset() const;
        
    private:
        float32 m_duration;
        float32 m_elapsed;
        bool m_isRunning;
        bool m_hasExpired;
        bool m_autoReset;
    };
    
} // namespace DSRT
