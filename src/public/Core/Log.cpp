#include "DSRT/Core/Log.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace DSRT {
namespace Core {

static LogLevel s_MinLogLevel = LogLevel::Info;
static bool s_Initialized = false;

void Log::Init() {
    if (s_Initialized) return;
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::cout << "=== DSRT Engine Log Started ===" << std::endl;
    std::cout << "Time: " << std::ctime(&time);
    std::cout << "Log Level: " << static_cast<int>(s_MinLogLevel) << std::endl;
    std::cout << "==============================" << std::endl << std::endl;
    
    s_Initialized = true;
}

void Log::Info(const std::string& msg) {
    Write(msg, LogLevel::Info);
}

void Log::Warning(const std::string& msg) {
    Write(msg, LogLevel::Warning);
}

void Log::Error(const std::string& msg) {
    Write(msg, LogLevel::Error);
}

void Log::Debug(const std::string& msg) {
    Write(msg, LogLevel::Debug);
}

void Log::Write(const std::string& msg, LogLevel level) {
    if (level < s_MinLogLevel) {
        return;
    }
    
    if (!s_Initialized) {
        Init();
    }
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::string levelStr;
    switch (level) {
        case LogLevel::Info:    levelStr = "INFO"; break;
        case LogLevel::Warning: levelStr = "WARN"; break;
        case LogLevel::Error:   levelStr = "ERROR"; break;
        case LogLevel::Debug:   levelStr = "DEBUG"; break;
    }
    
    std::ostringstream timestamp;
    timestamp << std::put_time(std::localtime(&time), "%H:%M:%S");
    timestamp << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    std::cout << "[" << timestamp.str() << "] "
              << "[" << levelStr << "] "
              << msg << std::endl;
    
    // Flush for errors to ensure they're seen
    if (level == LogLevel::Error) {
        std::cout.flush();
    }
}

} // namespace Core
} // namespace DSRT
