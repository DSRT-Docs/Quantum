#include "DSRT/Core/Assert.h"
#include <iostream>
#include <cstdlib>

namespace DSRT {
namespace Core {

static AssertHandler s_AssertHandler = DefaultAssertHandler;

void SetAssertHandler(AssertHandler handler) {
    s_AssertHandler = handler ? handler : DefaultAssertHandler;
}

void DefaultAssertHandler(const char* expression, const char* file, int line, const char* message) {
    std::cerr << "Assertion failed: " << expression << "\n";
    std::cerr << "File: " << file << "\n";
    std::cerr << "Line: " << line << "\n";
    if (message) {
        std::cerr << "Message: " << message << "\n";
    }
    std::cerr << std::endl;
    
    // Trigger breakpoint if debugger is attached
    #ifdef _MSC_VER
        __debugbreak();
    #elif defined(__GNUC__) || defined(__clang__)
        __builtin_trap();
    #endif
    
    std::abort();
}

void AssertHandlerWrapper(const char* expression, const char* file, int line, const char* message) {
    if (s_AssertHandler) {
        s_AssertHandler(expression, file, line, message);
    }
}

} // namespace Core
} // namespace DSRT
