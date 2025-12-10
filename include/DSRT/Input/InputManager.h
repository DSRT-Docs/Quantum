/**
 * @file InputManager.h
 * @brief Central input management system
 * @ingroup Input
 * 
 * @details Handles keyboard, mouse, gamepad, and touch input with
 *          event-driven and polling APIs. Supports input mapping and
 *          action-based input systems.
 */

#pragma once
#include "../Export.h"
#include "../Core/Types.h"
#include "../Math/Vector2.h"
#include <string>
#include <unordered_map>
#include <functional>
#include <vector>

namespace DSRT {
namespace Input {

/**
 * @enum KeyCode
 * @brief Keyboard key codes
 * @ingroup Input
 * 
 * @details Platform-independent key codes. Values match USB HID usage codes.
 */
enum class KeyCode : uint16_t {
    Unknown = 0,
    
    // Alphanumeric keys
    A = 4, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num1 = 30, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9, Num0,
    
    // Function keys
    F1 = 58, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
    
    // Navigation keys
    Escape = 41,
    Tab = 43,
    CapsLock = 57,
    Space = 44,
    Enter = 40,
    Backspace = 42,
    
    // Arrow keys
    Right = 79,
    Left = 80,
    Down = 81,
    Up = 82,
    
    // Modifier keys
    LeftShift = 225,
    RightShift = 229,
    LeftControl = 224,
    RightControl = 228,
    LeftAlt = 226,
    RightAlt = 230,
    LeftSuper = 227,  // Windows/Command key
    RightSuper = 231,
    
    // Special keys
    Insert = 73,
    Delete = 76,
    Home = 74,
    End = 77,
    PageUp = 75,
    PageDown = 78,
    PrintScreen = 70,
    ScrollLock = 71,
    Pause = 72,
    Menu = 101,
    
    // Numpad
    NumLock = 83,
    NumPadDivide = 84,
    NumPadMultiply = 85,
    NumPadSubtract = 86,
    NumPadAdd = 87,
    NumPadEnter = 88,
    NumPad1 = 89, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9, NumPad0,
    NumPadDecimal = 99,
    
    MaxKeyCode = 255
};

/**
 * @enum MouseButton
 * @brief Mouse button codes
 * @ingroup Input
 */
enum class MouseButton : uint8_t {
    Left = 0,
    Right = 1,
    Middle = 2,
    Button4 = 3,
    Button5 = 4,
    Button6 = 5,
    Button7 = 6,
    Button8 = 7,
    
    MaxButtons = 8
};

/**
 * @enum GamepadButton
 * @brief Gamepad/controller button codes
 * @ingroup Input
 * 
 * @details Follows Xbox controller layout for consistency.
 */
enum class GamepadButton : uint8_t {
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    LeftBumper = 4,
    RightBumper = 5,
    Back = 6,
    Start = 7,
    Guide = 8,
    LeftThumb = 9,
    RightThumb = 10,
    DPadUp = 11,
    DPadRight = 12,
    DPadDown = 13,
    DPadLeft = 14,
    
    MaxButtons = 15
};

/**
 * @enum GamepadAxis
 * @brief Gamepad analog axes
 * @ingroup Input
 */
enum class GamepadAxis : uint8_t {
    LeftX = 0,
    LeftY = 1,
    RightX = 2,
    RightY = 3,
    LeftTrigger = 4,
    RightTrigger = 5,
    
    MaxAxes = 6
};

/**
 * @struct InputAction
 * @brief Action mapping for input abstraction
 * @ingroup Input
 * 
 * @details Maps physical inputs to logical game actions
 *          (e.g., "Jump", "Fire", "Move").
 */
struct DSRT_API InputAction {
    std::string name;                     /**< Action identifier */
    std::vector<KeyCode> keyBindings;     /**< Keyboard bindings */
    std::vector<MouseButton> mouseBindings; /**< Mouse bindings */
    std::vector<GamepadButton> gamepadBindings; /**< Gamepad bindings */
    float deadzone = 0.1f;                /**< Analog deadzone */
    bool inverted = false;                /**< Invert axis values */
    
    /**
     * @brief Check if action has any bindings
     * @return true if at least one binding exists
     */
    bool HasBindings() const;
};

/**
 * @struct InputEvent
 * @brief Input event data
 * @ingroup Input
 */
struct DSRT_API InputEvent {
    enum class Type {
        KeyPressed,
        KeyReleased,
        KeyRepeated,
        MousePressed,
        MouseReleased,
        MouseMoved,
        MouseScrolled,
        GamepadConnected,
        GamepadDisconnected,
        GamepadButtonPressed,
        GamepadButtonReleased,
        GamepadAxisMoved,
        TouchBegan,
        TouchMoved,
        TouchEnded,
        TextInput
    };
    
    Type type;                            /**< Event type */
    uint64_t timestamp;                   /**< Event time in milliseconds */
    
    union {
        struct {
            KeyCode key;                  /**< Key code */
            bool repeat;                  /**< Auto-repeat flag */
        } key;
        
        struct {
            MouseButton button;           /**< Mouse button */
            Math::Vector2 position;       /**< Mouse position */
        } mouse;
        
        struct {
            int gamepadId;                /**< Gamepad ID */
            GamepadButton button;         /**< Gamepad button */
        } gamepadButton;
        
        struct {
            int gamepadId;                /**< Gamepad ID */
            GamepadAxis axis;             /**< Gamepad axis */
            float value;                  /**< Axis value [-1.0, 1.0] */
        } gamepadAxis;
        
        struct {
            float deltaX;                 /**< Horizontal scroll */
            float deltaY;                 /**< Vertical scroll */
        } scroll;
        
        struct {
            uint32_t codepoint;           /**< Unicode codepoint */
        } text;
    } data;                               /**< Event-specific data */
};

/**
 * @class InputManager
 * @brief Central input management system
 * @ingroup Input
 * 
 * @details Provides both polling and event-driven input APIs.
 *          Supports multiple input devices and action mapping.
 * 
 * @example Polling input:
 * @code
 * InputManager& input = GetInputManager();
 * 
 * if (input.IsKeyPressed(KeyCode::Space)) {
 *     player->Jump();
 * }
 * 
 * Math::Vector2 mousePos = input.GetMousePosition();
 * float mouseDelta = input.GetMouseDelta();
 * @endcode
 * 
 * @example Event-based input:
 * @code
 * input.AddKeyPressedCallback([](KeyCode key) {
 *     if (key == KeyCode::Escape) {
 *         ShowMenu();
 *     }
 * });
 * @endcode
 * 
 * @example Action mapping:
 * @code
 * InputAction jumpAction;
 * jumpAction.name = "Jump";
 * jumpAction.keyBindings = {KeyCode::Space, KeyCode::W};
 * jumpAction.gamepadBindings = {GamepadButton::A};
 * 
 * if (input.GetActionValue("Jump") > 0.5f) {
 *     player->Jump();
 * }
 * @endcode
 */
class DSRT_API InputManager {
public:
    /**
     * @brief Create input manager instance
     * @return Shared pointer to input manager
     */
    static Ref<InputManager> Create();
    
    virtual ~InputManager() = default;
    
    /**
     * @brief Initialize input system
     * @return true if initialization succeeded
     */
    virtual bool Initialize() = 0;
    
    /**
     * @brief Shutdown input system
     */
    virtual void Shutdown() = 0;
    
    /**
     * @brief Update input state
     * @param deltaTime Time since last update
     * 
     * @details Call once per frame to update internal state,
     *          process events, and calculate deltas.
     */
    virtual void Update(float deltaTime) = 0;
    
    // Keyboard state
    
    /**
     * @brief Check if key is currently pressed
     * @param key Key code to check
     * @return true if key is down
     */
    virtual bool IsKeyPressed(KeyCode key) const = 0;
    
    /**
     * @brief Check if key was pressed this frame
     * @param key Key code to check
     * @return true if key transitioned from up to down
     */
    virtual bool IsKeyJustPressed(KeyCode key) const = 0;
    
    /**
     * @brief Check if key was released this frame
     * @param key Key code to check
     * @return true if key transitioned from down to up
     */
    virtual bool IsKeyJustReleased(KeyCode key) const = 0;
    
    /**
     * @brief Get key pressed duration
     * @param key Key code to check
     * @return Time in seconds key has been held
     */
    virtual float GetKeyHoldDuration(KeyCode key) const = 0;
    
    // Mouse state
    
    /**
     * @brief Get mouse position
     * @return Screen-space coordinates
     */
    virtual Math::Vector2 GetMousePosition() const = 0;
    
    /**
     * @brief Get mouse position delta since last frame
     * @return Movement delta in pixels
     */
    virtual Math::Vector2 GetMouseDelta() const = 0;
    
    /**
     * @brief Get mouse scroll delta
     * @return Scroll wheel movement
     */
    virtual Math::Vector2 GetMouseScrollDelta() const = 0;
    
    /**
     * @brief Check if mouse button is pressed
     * @param button Mouse button to check
     * @return true if button is down
     */
    virtual bool IsMouseButtonPressed(MouseButton button) const = 0;
    
    /**
     * @brief Check if mouse button was just pressed
     * @param button Mouse button to check
     * @return true if button was pressed this frame
     */
    virtual bool IsMouseButtonJustPressed(MouseButton button) const = 0;
    
    /**
     * @brief Check if mouse button was just released
     * @param button Mouse button to check
     * @return true if button was released this frame
     */
    virtual bool IsMouseButtonJustReleased(MouseButton button) const = 0;
    
    /**
     * @brief Set mouse cursor position
     * @param position New screen position
     * @return true if position was set successfully
     */
    virtual bool SetMousePosition(const Math::Vector2& position) = 0;
    
    /**
     * @brief Show or hide mouse cursor
     * @param visible true to show cursor
     */
    virtual void SetMouseCursorVisible(bool visible) = 0;
    
    /**
     * @brief Capture or release mouse cursor
     * @param captured true to capture (confine to window)
     */
    virtual void SetMouseCursorCaptured(bool captured) = 0;
    
    // Gamepad state
    
    /**
     * @brief Check if gamepad is connected
     * @param gamepadId Gamepad index (0-based)
     * @return true if gamepad is connected
     */
    virtual bool IsGamepadConnected(int gamepadId) const = 0;
    
    /**
     * @brief Get number of connected gamepads
     * @return Connected gamepad count
     */
    virtual int GetConnectedGamepadCount() const = 0;
    
    /**
     * @brief Get gamepad name
     * @param gamepadId Gamepad index
     * @return Device name string
     */
    virtual std::string GetGamepadName(int gamepadId) const = 0;
    
    /**
     * @brief Check if gamepad button is pressed
     * @param gamepadId Gamepad index
     * @param button Button to check
     * @return true if button is down
     */
    virtual bool IsGamepadButtonPressed(int gamepadId, GamepadButton button) const = 0;
    
    /**
     * @brief Check if gamepad button was just pressed
     * @param gamepadId Gamepad index
     * @param button Button to check
     * @return true if button was pressed this frame
     */
    virtual bool IsGamepadButtonJustPressed(int gamepadId, GamepadButton button) const = 0;
    
    /**
     * @brief Check if gamepad button was just released
     * @param gamepadId Gamepad index
     * @param button Button to check
     * @return true if button was released this frame
     */
    virtual bool IsGamepadButtonJustReleased(int gamepadId, GamepadButton button) const = 0;
    
    /**
     * @brief Get gamepad axis value
     * @param gamepadId Gamepad index
     * @param axis Axis to query
     * @return Axis value [-1.0, 1.0] with deadzone applied
     */
    virtual float GetGamepadAxis(int gamepadId, GamepadAxis axis) const = 0;
    
    /**
     * @brief Get raw gamepad axis value
     * @param gamepadId Gamepad index
     * @param axis Axis to query
     * @return Raw axis value without deadzone
     */
    virtual float GetGamepadAxisRaw(int gamepadId, GamepadAxis axis) const = 0;
    
    /**
     * @brief Set gamepad vibration
     * @param gamepadId Gamepad index
     * @param leftMotor Left motor intensity [0.0, 1.0]
     * @param rightMotor Right motor intensity [0.0, 1.0]
     * @param duration Vibration duration in seconds (0 = infinite)
     * @return true if vibration was started
     */
    virtual bool SetGamepadVibration(int gamepadId, float leftMotor, float rightMotor, float duration = 0.0f) = 0;
    
    // Touch input (mobile)
    
    /**
     * @brief Get number of active touch points
     * @return Touch point count
     */
    virtual int GetTouchPointCount() const = 0;
    
    /**
     * @brief Get touch point position
     * @param index Touch point index
     * @return Screen position
     */
    virtual Math::Vector2 GetTouchPosition(int index) const = 0;
    
    /**
     * @brief Get touch point ID
     * @param index Touch point index
     * @return Unique touch identifier
     */
    virtual int64_t GetTouchId(int index) const = 0;
    
    // Action system
    
    /**
     * @brief Register input action
     * @param action Action definition
     * @return true if action was registered
     */
    virtual bool RegisterAction(const InputAction& action) = 0;
    
    /**
     * @brief Unregister input action
     * @param actionName Action name
     * @return true if action was found and removed
     */
    virtual bool UnregisterAction(const std::string& actionName) = 0;
    
    /**
     * @brief Get action value
     * @param actionName Action name
     * @return Action intensity [0.0, 1.0]
     * 
     * @details Returns maximum value from all bound inputs.
     *          For binary inputs, returns 1.0 if pressed.
     */
    virtual float GetActionValue(const std::string& actionName) const = 0;
    
    /**
     * @brief Check if action was triggered this frame
     * @param actionName Action name
     * @return true if any bound input transitioned from up to down
     */
    virtual bool IsActionTriggered(const std::string& actionName) const = 0;
    
    /**
     * @brief Check if action was released this frame
     * @param actionName Action name
     * @return true if any bound input transitioned from down to up
     */
    virtual bool IsActionReleased(const std::string& actionName) const = 0;
    
    // Event system
    
    /**
     * @brief Add key pressed callback
     * @param callback Function to call when key is pressed
     * @return Callback ID for removal
     */
    virtual uint64_t AddKeyPressedCallback(std::function<void(KeyCode)> callback) = 0;
    
    /**
     * @brief Add key released callback
     * @param callback Function to call when key is released
     * @return Callback ID for removal
     */
    virtual uint64_t AddKeyReleasedCallback(std::function<void(KeyCode)> callback) = 0;
    
    /**
     * @brief Add mouse moved callback
     * @param callback Function to call when mouse moves
     * @return Callback ID for removal
     */
    virtual uint64_t AddMouseMovedCallback(std::function<void(const Math::Vector2&)> callback) = 0;
    
    /**
     * @brief Add text input callback
     * @param callback Function to call when text is entered
     * @return Callback ID for removal
     */
    virtual uint64_t AddTextInputCallback(std::function<void(uint32_t codepoint)> callback) = 0;
    
    /**
     * @brief Remove callback by ID
     * @param callbackId Callback identifier
     * @return true if callback was found and removed
     */
    virtual bool RemoveCallback(uint64_t callbackId) = 0;
    
    /**
     * @brief Clear all callbacks of specific type
     * @tparam CallbackType Callback type (auto-deduced)
     */
    template<typename CallbackType>
    void ClearCallbacks();
    
    /**
     * @brief Poll input events
     * @param[out] events Vector to receive events
     * 
     * @details Retrieves all events since last poll.
     *          Events are cleared after polling.
     */
    virtual void PollEvents(std::vector<InputEvent>& events) = 0;
    
    // Configuration
    
    /**
     * @brief Set mouse sensitivity
     * @param sensitivity Sensitivity multiplier
     */
    virtual void SetMouseSensitivity(float sensitivity) = 0;
    
    /**
     * @brief Get mouse sensitivity
     * @return Current sensitivity setting
     */
    virtual float GetMouseSensitivity() const = 0;
    
    /**
     * @brief Set gamepad deadzone
     * @param gamepadId Gamepad index
     * @param deadzone Deadzone value [0.0, 1.0]
     */
    virtual void SetGamepadDeadzone(int gamepadId, float deadzone) = 0;
    
    /**
     * @brief Get gamepad deadzone
     * @param gamepadId Gamepad index
     * @return Current deadzone setting
     */
    virtual float GetGamepadDeadzone(int gamepadId) const = 0;
    
    /**
     * @brief Set key repeat delay
     * @param delay Delay in seconds before repeat starts
     */
    virtual void SetKeyRepeatDelay(float delay) = 0;
    
    /**
     * @brief Set key repeat rate
     * @param rate Repeat rate in seconds between repeats
     */
    virtual void SetKeyRepeatRate(float rate) = 0;
    
    // Utility
    
    /**
     * @brief Convert key code to string
     * @param key Key code
     * @return Human-readable key name
     */
    virtual std::string KeyToString(KeyCode key) const = 0;
    
    /**
     * @brief Convert string to key code
     * @param str Key name string
     * @return Key code, or KeyCode::Unknown on failure
     */
    virtual KeyCode StringToKey(const std::string& str) const = 0;
    
    /**
     * @brief Get clipboard text
     * @return Clipboard content
     */
    virtual std::string GetClipboardText() const = 0;
    
    /**
     * @brief Set clipboard text
     * @param text Text to copy to clipboard
     * @return true if operation succeeded
     */
    virtual bool SetClipboardText(const std::string& text) = 0;
    
protected:
    InputManager() = default;
};

} // namespace Input
} // namespace DSRT
