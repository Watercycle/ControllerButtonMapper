#pragma once
#include "enum.h"
#include <unordered_map>
#include <unordered_set>

CREATE_GLOBAL_ENUM(Direction,
    Up, Right, Down, Left)

CREATE_GLOBAL_ENUM(InputActionType,
    Press, Release, Hold)

CREATE_GLOBAL_ENUM(ButtonType,
    /* generic, physical buttons (up to 32) */
    Button0, Button1, Button2, Button3, Button4, Button5, Button6, Button7, Button8, Button9, Button10, Button11, Button12,

    /* VIRTUAL KEYS*/
    /* dpad */
    DPadUp, DPadRight, DPadDown, DPadLeft,

    /* left stick */
    LeftStickUp, LeftStickRight, LeftStickDown, LeftStickLeft,
    LeftStickUpHard, LeftStickRightHard, LeftStickDownHard, LeftStickLeftHard,

    /* right stick */
    RightStickUp, RightStickRight, RightStickDown, RightStickLeft,
    RightStickUpHard, RightStickRightHard, RightStickDownHard, RightStickLeftHard,

    /* left trigger */
    LeftTrigger, LeftTriggerHard,

    /* right trigger */
    RightTrigger, RightTriggerHard)

CREATE_GLOBAL_ENUM(AxisType,
    DPad, LeftStick, RightStick, LeftTrigger, RightTrigger)

CREATE_GLOBAL_ENUM(KeyType,
    /* mouse */
    MouseLeft, MouseRight, MouseScrollWheel, MouseXButton1, MouseXButton2,

    /* F1 - F15 */
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15,

    /* numbers */
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    
    /* arrows */
    Up, Down, Left, Right,

    /* pairs */
    OpenBracket, CloseBracket,
    PageUp, PageDown,
    Slash, Backslash,
    Space, Backspace,
    Enter, Insert, Delete,

    /* generalizations */
    Shift, Alt, Control,

    /* others */
    PrintScreen, Clear, Home, Pause, End,

    Escape, Tab, CapsLock, Tilde, Dash, Equal, Fn, Cmd,

    Comma, Colon, Period, Quote,

    /* alphabet */
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    /* numpad numbers */
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,

    /* numpad */
    NumLock, Add, Decimal, Divide, Multiply, Subtract, NumpadEnter)


#define UNKNOWN 0

struct KeyMapping
{
    unsigned virtualKeyId;
    unsigned scanCodeId;
    string description;
};

class Controller;

struct KeyAction
{
    const InputActionType action;
    const vector<KeyType> keys;

    ~KeyAction();
    KeyAction(InputActionType action, vector<KeyType> keys) : action(action), keys(keys) {}

    friend std::ostream& operator<< (std::ostream& os, const KeyAction& type)
    {
        os << type.action << ": ";
        for (const auto& key : type.keys) os << key << " ";
        return os;
    }
};

struct ControllerAction
{
    ~ControllerAction();

    const InputActionType action;
    const vector<ButtonType> buttons;

    bool triggered(Controller* controller) const;
};

extern unordered_map<KeyType, KeyMapping> virtualKeyCodes;
extern unordered_set<KeyType> currentlyPressed;

void ReleaseAllHeldKeys();

void SimulateMouseScroll(float amount, bool vertical);
void SimulateKeys(vector<KeyAction> keyActions, bool usingScancode = false);

void SetCursorPos(vec2 pos);
vec2 GetCursorPos();
void MoveCursorToPos(vec2 pos);
vec2 GetTotalScreenSize();

void wait(float milliseconds);