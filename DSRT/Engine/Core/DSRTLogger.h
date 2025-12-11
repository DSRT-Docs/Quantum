#pragma once

/**
 * @file DSRTLogger.h
 * @brief Logging system
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"
#include <cstdarg>

namespace DSRT {
    
    /**
     * @brief Log severity levels
     */
    enum class LogLevel : uint8 {
        Trace = 0,      // Detailed debugging information
        Debug = 1,      // General debugging information
        Info = 2,       // Informational messages
        Warning = 3,    // Warning messages
        Error = 4,      // Error messages
        Critical = 5,   // Critical error messages
        Off = 6         // No logging
    };
    
    /**
     * @brief Log category for filtering
     */
    enum class LogCategory : uint32 {
        Core = DSRT_BIT(0),
        Memory = DSRT_BIT(1),
        Render = DSRT_BIT(2),
        Audio = DSRT_BIT(3),
        Physics = DSRT_BIT(4),
        Script = DSRT_BIT(5),
        Network = DSRT_BIT(6),
        Editor = DSRT_BIT(7),
        Game = DSRT_BIT(8),
        Asset = DSRT_BIT(9),
        Input = DSRT_BIT(10),
        All = 0xFFFFFFFF
    };
    
    /**
     * @brief Log message structure
     */
    struct LogMessage {
        LogLevel level;
        LogCategory category;
        const char* file;
        int line;
        const char* function;
        const char* message;
        uint64 timestamp;
        uint32 threadId;
    };
    
    /**
     * @brief Log sink interface
     */
    class ILogSink {
    public:
        virtual ~ILogSink() = default;
        
        /**
         * @brief Called when a log message is generated
         * @param message Log message
         */
        virtual void OnLogMessage(const LogMessage& message) = 0;
        
        /**
         * @brief Called when the sink should flush its output
         */
        virtual void Flush() = 0;
    };
    
    /**
     * @brief Console log sink
     */
    class ConsoleLogSink : public ILogSink {
    public:
        void OnLogMessage(const LogMessage& message) override;
        void Flush() override;
        
    private:
        static const char* GetLevelString(LogLevel level);
        static const char* GetCategoryString(LogCategory category);
        static const char* GetLevelColor(LogLevel level);
        static void ResetColor();
    };
    
    /**
     * @brief File log sink
     */
    class FileLogSink : public ILogSink {
    public:
        /**
         * @brief Constructs a file log sink
         * @param filename Log file path
         */
        explicit FileLogSink(const char* filename);
        ~FileLogSink();
        
        void OnLogMessage(const LogMessage& message) override;
        void Flush() override;
        
    private:
        void* m_fileHandle;
    };
    
    /**
     * @brief Main logger class
     */
    class Logger {
    public:
        /**
         * @brief Initializes the logger
         * @param minLevel Minimum log level to output
         * @param enabledCategories Categories to log
         * @return True if initialization succeeded
         */
        static bool Initialize(LogLevel minLevel = LogLevel::Info,
                              uint32 enabledCategories = static_cast<uint32>(LogCategory::All));
        
        /**
         * @brief Shuts down the logger
         */
        static void Shutdown();
        
        /**
         * @brief Adds a log sink
         * @param sink Log sink to add
         */
        static void AddSink(ILogSink* sink);
        
        /**
         * @brief Removes a log sink
         * @param sink Log sink to remove
         */
        static void RemoveSink(ILogSink* sink);
        
        /**
         * @brief Sets the minimum log level
         * @param level Minimum level to log
         */
        static void SetMinLevel(LogLevel level);
        
        /**
         * @brief Sets enabled log categories
         * @param categories Bitmask of categories to enable
         */
        static void SetEnabledCategories(uint32 categories);
        
        /**
         * @brief Logs a message
         * @param level Log level
         * @param category Log category
         * @param file Source file
         * @param line Source line
         * @param function Function name
         * @param format Message format string
         * @param ... Format arguments
         */
        static void Log(LogLevel level, LogCategory category,
                       const char* file, int line, const char* function,
                       const char* format, ...);
        
        /**
         * @brief Logs a message with va_list
         * @param level Log level
         * @param category Log category
         * @param file Source file
         * @param line Source line
         * @param function Function name
         * @param format Message format string
         * @param args Format arguments
         */
        static void LogV(LogLevel level, LogCategory category,
                        const char* file, int line, const char* function,
                        const char* format, va_list args);
        
        /**
         * @brief Flushes all log sinks
         */
        static void Flush();
        
    private:
        Logger() = delete;
        ~Logger() = delete;
        
        static uint64 GetTimestamp();
        static uint32 GetThreadId();
        
        static LogLevel s_minLevel;
        static uint32 s_enabledCategories;
        static ILogSink** s_sinks;
        static uint32 s_sinkCount;
        static uint32 s_sinkCapacity;
    };
    
    // Logging macros
    #define DSRT_LOG_TRACE(category, format, ...) \
        DSRT::Logger::Log(DSRT::LogLevel::Trace, category, \
                         __FILE__, __LINE__, __FUNCTION__, \
                         format, ##__VA_ARGS__)
    
    #define DSRT_LOG_DEBUG(category, format, ...) \
        DSRT::Logger::Log(DSRT::LogLevel::Debug, category, \
                         __FILE__, __LINE__, __FUNCTION__, \
                         format, ##__VA_ARGS__)
    
    #define DSRT_LOG_INFO(category, format, ...) \
        DSRT::Logger::Log(DSRT::LogLevel::Info, category, \
                         __FILE__, __LINE__, __FUNCTION__, \
                         format, ##__VA_ARGS__)
    
    #define DSRT_LOG_WARNING(category, format, ...) \
        DSRT::Logger::Log(DSRT::LogLevel::Warning, category, \
                         __FILE__, __LINE__, __FUNCTION__, \
                         format, ##__VA_ARGS__)
    
    #define DSRT_LOG_ERROR(category, format, ...) \
        DSRT::Logger::Log(DSRT::LogLevel::Error, category, \
                         __FILE__, __LINE__, __FUNCTION__, \
                         format, ##__VA_ARGS__)
    
    #define DSRT_LOG_CRITICAL(category, format, ...) \
        DSRT::Logger::Log(DSRT::LogLevel::Critical, category, \
                         __FILE__, __LINE__, __FUNCTION__, \
                         format, ##__VA_ARGS__)
    
} // namespace DSRT
