#pragma once

/**
 * @file Platform.h
 * @brief Platform abstraction layer
 */

#include "DSRTCore.h"
#include "DSRTTypes.h"

namespace DSRT {
    
    /**
     * @brief Window creation flags
     */
    struct WindowCreateInfo {
        const char* title;
        int32 width;
        int32 height;
        bool fullscreen;
        bool resizable;
        bool bordered;
        
        WindowCreateInfo()
            : title("DSRT Engine")
            , width(1280)
            , height(720)
            , fullscreen(false)
            , resizable(true)
            , bordered(true) {}
    };
    
    /**
     * @brief Window handle
     */
    using WindowHandle = void*;
    
    /**
     * @brief Platform window class
     */
    class PlatformWindow {
    public:
        /**
         * @brief Creates a window
         * @param info Window creation info
         * @return Window handle
         */
        static WindowHandle Create(const WindowCreateInfo& info);
        
        /**
         * @brief Destroys a window
         * @param handle Window handle
         */
        static void Destroy(WindowHandle handle);
        
        /**
         * @brief Shows a window
         * @param handle Window handle
         */
        static void Show(WindowHandle handle);
        
        /**
         * @brief Hides a window
         * @param handle Window handle
         */
        static void Hide(WindowHandle handle);
        
        /**
         * @brief Sets window title
         * @param handle Window handle
         * @param title Window title
         */
        static void SetTitle(WindowHandle handle, const char* title);
        
        /**
         * @brief Sets window size
         * @param handle Window handle
         * @param width Window width
         * @param height Window height
         */
        static void SetSize(WindowHandle handle, int32 width, int32 height);
        
        /**
         * @brief Gets window size
         * @param handle Window handle
         * @param width Output window width
         * @param height Output window height
         */
        static void GetSize(WindowHandle handle, int32& width, int32& height);
        
        /**
         * @brief Sets window position
         * @param handle Window handle
         * @param x Window X position
         * @param y Window Y position
         */
        static void SetPosition(WindowHandle handle, int32 x, int32 y);
        
        /**
         * @brief Gets window position
         * @param handle Window handle
         * @param x Output window X position
         * @param y Output window Y position
         */
        static void GetPosition(WindowHandle handle, int32& x, int32& y);
        
        /**
         * @brief Sets window fullscreen state
         * @param handle Window handle
         * @param fullscreen True for fullscreen
         */
        static void SetFullscreen(WindowHandle handle, bool fullscreen);
        
        /**
         * @brief Checks if window is fullscreen
         * @param handle Window handle
         * @return True if fullscreen
         */
        static bool IsFullscreen(WindowHandle handle);
        
        /**
         * @brief Minimizes window
         * @param handle Window handle
         */
        static void Minimize(WindowHandle handle);
        
        /**
         * @brief Maximizes window
         * @param handle Window handle
         */
        static void Maximize(WindowHandle handle);
        
        /**
         * @brief Restores window
         * @param handle Window handle
         */
        static void Restore(WindowHandle handle);
        
        /**
         * @brief Focuses window
         * @param handle Window handle
         */
        static void Focus(WindowHandle handle);
        
        /**
         * @brief Checks if window is focused
         * @param handle Window handle
         * @return True if focused
         */
        static bool IsFocused(WindowHandle handle);
        
        /**
         * @brief Checks if window is minimized
         * @param handle Window handle
         * @return True if minimized
         */
        static bool IsMinimized(WindowHandle handle);
        
        /**
         * @brief Checks if window is maximized
         * @param handle Window handle
         * @return True if maximized
         */
        static bool IsMaximized(WindowHandle handle);
        
        /**
         * @brief Sets window visibility
         * @param handle Window handle
         * @param visible True to show window
         */
        static void SetVisible(WindowHandle handle, bool visible);
        
        /**
         * @brief Checks if window is visible
         * @param handle Window handle
         * @return True if visible
         */
        static bool IsVisible(WindowHandle handle);
        
        /**
         * @brief Gets native window handle
         * @param handle Window handle
         * @return Native window handle
         */
        static void* GetNativeHandle(WindowHandle handle);
        
        /**
         * @brief Processes window events
         * @param handle Window handle
         * @return True if window should close
         */
        static bool ProcessEvents(WindowHandle handle);
    };
    
    /**
     * @brief File system operations
     */
    class FileSystem {
    public:
        /**
         * @brief File open mode
         */
        enum class OpenMode {
            Read,
            Write,
            Append,
            ReadWrite
        };
        
        /**
         * @brief File seek origin
         */
        enum class SeekOrigin {
            Begin,
            Current,
            End
        };
        
        /**
         * @brief File attributes
         */
        enum class Attribute {
            ReadOnly = DSRT_BIT(0),
            Hidden = DSRT_BIT(1),
            System = DSRT_BIT(2),
            Directory = DSRT_BIT(3),
            Archive = DSRT_BIT(4),
            Normal = DSRT_BIT(5)
        };
        
        /**
         * @brief File time type
         */
        enum class TimeType {
            Created,
            Modified,
            Accessed
        };
        
        /**
         * @brief File handle
         */
        using FileHandle = void*;
        
        /**
         * @brief Directory handle
         */
        using DirHandle = void*;
        
        /**
         * @brief File information
         */
        struct FileInfo {
            char name[256];
            uint64 size;
            uint32 attributes;
            uint64 creationTime;
            uint64 lastWriteTime;
            uint64 lastAccessTime;
        };
        
        /**
         * @brief Opens a file
         * @param path File path
         * @param mode Open mode
         * @return File handle
         */
        static FileHandle OpenFile(const char* path, OpenMode mode);
        
        /**
         * @brief Closes a file
         * @param handle File handle
         */
        static void CloseFile(FileHandle handle);
        
        /**
         * @brief Reads from file
         * @param handle File handle
         * @param buffer Output buffer
         * @param size Number of bytes to read
         * @return Number of bytes read
         */
        static size_t ReadFile(FileHandle handle, void* buffer, size_t size);
        
        /**
         * @brief Writes to file
         * @param handle File handle
         * @param buffer Input buffer
         * @param size Number of bytes to write
         * @return Number of bytes written
         */
        static size_t WriteFile(FileHandle handle, const void* buffer, size_t size);
        
        /**
         * @brief Seeks in file
         * @param handle File handle
         * @param offset Seek offset
         * @param origin Seek origin
         * @return New file position
         */
        static int64 SeekFile(FileHandle handle, int64 offset, SeekOrigin origin);
        
        /**
         * @brief Gets file position
         * @param handle File handle
         * @return Current file position
         */
        static int64 TellFile(FileHandle handle);
        
        /**
         * @brief Gets file size
         * @param handle File handle
         * @return File size in bytes
         */
        static int64 GetFileSize(FileHandle handle);
        
        /**
         * @brief Checks if file exists
         * @param path File path
         * @return True if file exists
         */
        static bool FileExists(const char* path);
        
        /**
         * @brief Deletes a file
         * @param path File path
         * @return True if successful
         */
        static bool DeleteFile(const char* path);
        
        /**
         * @brief Renames a file
         * @param oldPath Old file path
         * @param newPath New file path
         * @return True if successful
         */
        static bool RenameFile(const char* oldPath, const char* newPath);
        
        /**
         * @brief Copies a file
         * @param source Source file path
         * @param destination Destination file path
         * @return True if successful
         */
        static bool CopyFile(const char* source, const char* destination);
        
        /**
         * @brief Creates a directory
         * @param path Directory path
         * @return True if successful
         */
        static bool CreateDirectory(const char* path);
        
        /**
         * @brief Deletes a directory
         * @param path Directory path
         * @return True if successful
         */
        static bool DeleteDirectory(const char* path);
        
        /**
         * @brief Checks if directory exists
         * @param path Directory path
         * @return True if directory exists
         */
        static bool DirectoryExists(const char* path);
        
        /**
         * @brief Opens a directory
         * @param path Directory path
         * @return Directory handle
         */
        static DirHandle OpenDirectory(const char* path);
        
        /**
         * @brief Closes a directory
         * @param handle Directory handle
         */
        static void CloseDirectory(DirHandle handle);
        
        /**
         * @brief Reads directory entry
         * @param handle Directory handle
         * @param info Output file information
         * @return True if entry was read
         */
        static bool ReadDirectory(DirHandle handle, FileInfo& info);
        
        /**
         * @brief Gets current working directory
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetCurrentDirectory(char* buffer, size_t size);
        
        /**
         * @brief Sets current working directory
         * @param path Directory path
         * @return True if successful
         */
        static bool SetCurrentDirectory(const char* path);
        
        /**
         * @brief Gets executable directory
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetExecutableDirectory(char* buffer, size_t size);
        
        /**
         * @brief Gets temporary directory
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetTempDirectory(char* buffer, size_t size);
        
        /**
         * @brief Creates temporary file
         * @param buffer Output file path buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool CreateTempFile(char* buffer, size_t size);
        
        /**
         * @brief Gets file attributes
         * @param path File path
         * @return File attributes
         */
        static uint32 GetFileAttributes(const char* path);
        
        /**
         * @brief Sets file attributes
         * @param path File path
         * @param attributes File attributes
         * @return True if successful
         */
        static bool SetFileAttributes(const char* path, uint32 attributes);
        
        /**
         * @brief Gets file time
         * @param path File path
         * @param type Time type
         * @return File time
         */
        static uint64 GetFileTime(const char* path, TimeType type);
        
        /**
         * @brief Sets file time
         * @param path File path
         * @param type Time type
         * @param time File time
         * @return True if successful
         */
        static bool SetFileTime(const char* path, TimeType type, uint64 time);
    };
    
    /**
     * @brief System information
     */
    class SystemInfo {
    public:
        /**
         * @brief Processor architecture
         */
        enum class Architecture {
            Unknown,
            x86,
            x64,
            ARM,
            ARM64
        };
        
        /**
         * @brief Gets processor architecture
         * @return Processor architecture
         */
        static Architecture GetArchitecture();
        
        /**
         * @brief Gets number of processor cores
         * @return Number of cores
         */
        static uint32 GetProcessorCount();
        
        /**
         * @brief Gets page size
         * @return Page size in bytes
         */
        static uint32 GetPageSize();
        
        /**
         * @brief Gets allocation granularity
         * @return Allocation granularity in bytes
         */
        static uint32 GetAllocationGranularity();
        
        /**
         * @brief Gets total physical memory
         * @return Total physical memory in bytes
         */
        static uint64 GetTotalPhysicalMemory();
        
        /**
         * @brief Gets available physical memory
         * @return Available physical memory in bytes
         */
        static uint64 GetAvailablePhysicalMemory();
        
        /**
         * @brief Gets total virtual memory
         * @return Total virtual memory in bytes
         */
        static uint64 GetTotalVirtualMemory();
        
        /**
         * @brief Gets available virtual memory
         * @return Available virtual memory in bytes
         */
        static uint64 GetAvailableVirtualMemory();
        
        /**
         * @brief Gets system name
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetSystemName(char* buffer, size_t size);
        
        /**
         * @brief Gets system version
         * @param major Output major version
         * @param minor Output minor version
         * @param build Output build number
         * @return True if successful
         */
        static bool GetSystemVersion(uint32& major, uint32& minor, uint32& build);
        
        /**
         * @brief Gets computer name
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetComputerName(char* buffer, size_t size);
        
        /**
         * @brief Gets user name
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetUserName(char* buffer, size_t size);
    };
    
    /**
     * @brief System utilities
     */
    class System {
    public:
        /**
         * @brief Sleeps for specified milliseconds
         * @param milliseconds Time to sleep
         */
        static void Sleep(uint32 milliseconds);
        
        /**
         * @brief Gets system tick count
         * @return Tick count in milliseconds
         */
        static uint64 GetTickCount();
        
        /**
         * @brief Gets high-resolution timer frequency
         * @return Timer frequency in Hz
         */
        static uint64 GetTimerFrequency();
        
        /**
         * @brief Gets high-resolution timer value
         * @return Timer value
         */
        static uint64 GetTimerValue();
        
        /**
         * @brief Executes a command
         * @param command Command to execute
         * @return Exit code
         */
        static int32 ExecuteCommand(const char* command);
        
        /**
         * @brief Gets environment variable
         * @param name Variable name
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetEnvironmentVariable(const char* name, char* buffer, size_t size);
        
        /**
         * @brief Sets environment variable
         * @param name Variable name
         * @param value Variable value
         * @return True if successful
         */
        static bool SetEnvironmentVariable(const char* name, const char* value);
        
        /**
         * @brief Gets command line arguments
         * @param argc Output argument count
         * @param argv Output argument array
         * @return True if successful
         */
        static bool GetCommandLineArguments(int32& argc, char**& argv);
        
        /**
         * @brief Gets process ID
         * @return Process ID
         */
        static uint32 GetProcessId();
        
        /**
         * @brief Gets thread ID
         * @return Thread ID
         */
        static uint32 GetThreadId();
        
        /**
         * @brief Terminates the process
         * @param exitCode Exit code
         */
        static void TerminateProcess(int32 exitCode);
        
        /**
         * @brief Gets last error code
         * @return Error code
         */
        static uint32 GetLastError();
        
        /**
         * @brief Gets last error message
         * @param buffer Output buffer
         * @param size Buffer size
         * @return True if successful
         */
        static bool GetLastErrorMessage(char* buffer, size_t size);
    };
    
} // namespace DSRT
