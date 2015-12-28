#include "stdafx.h"
#include "binding_enums.h"
#include <unordered_map>
#include <unordered_set>
#include "controller.h"

KeyAction::~KeyAction() {}
ControllerAction::~ControllerAction() {}

bool ControllerAction::triggered(Controller* controller) const
{
    for (ButtonType button : buttons)
    {
        switch (action)
        {
        case InputActionType::Press: if (!controller->buttonPressed(button)) return false; break;
        case InputActionType::Release: if (!controller->buttonReleased(button)) return false; break;
        case InputActionType::Hold: if (!controller->buttonHeldDown(button)) return false; break;
        }
    }

    return true;
}

#ifdef _WIN32
#include <windows.h>

//std::mutex keyLockGuard;

unordered_map<KeyType, KeyMapping> create_key_map()
{
    unordered_map<KeyType, KeyMapping> map =
        {
            // mouse
            { KeyType::MouseLeft, { VK_LBUTTON, UNKNOWN, "Left Mouse Click" } },
            { KeyType::MouseRight, { VK_RBUTTON, UNKNOWN, "Right Mouse Click" } },
            { KeyType::MouseScrollWheel, { VK_MBUTTON , UNKNOWN, "Mouse Scroll Wheel Press" } },
            { KeyType::MouseXButton1, { VK_XBUTTON1, UNKNOWN, "Mouse XButton 1" } },
            { KeyType::MouseXButton2, { VK_XBUTTON2, UNKNOWN, "Mouse XButton 2" } },

            // F1 - F15
            {KeyType::F1,{VK_F1, 59, "F1"}},
            {KeyType::F2,{VK_F2, 60, "F2"}},
            {KeyType::F3,{VK_F3, 61, "F3"}},
            {KeyType::F4,{VK_F4, 62, "F4"}},
            {KeyType::F5,{VK_F5, 63, "F5"}},
            {KeyType::F6,{VK_F6, 64, "F6"}},
            {KeyType::F7,{VK_F7, 65, "F7"}},
            {KeyType::F8,{VK_F8, 66, "F8"}},
            {KeyType::F9,{VK_F9, 67, "F9"}},
            {KeyType::F10,{VK_F10, 68, "F10"}},
            {KeyType::F11,{VK_F11, 87, "F11"}},
            {KeyType::F12,{VK_F12, 88, "F12"}},
            {KeyType::F13,{VK_F13, UNKNOWN, "F13"}},
            {KeyType::F14,{VK_F14, UNKNOWN, "F14"}},
            {KeyType::F15,{VK_F15, UNKNOWN, "F15"}},

            // numbers
            {KeyType::Num0,{0x30, 11, "The Number 0"}},
            {KeyType::Num1,{0x31, 2, "The Number 1"}},
            {KeyType::Num2,{0x32, 3, "The Number 2"}},
            {KeyType::Num3,{0x33, 4, "The Number 3"}},
            {KeyType::Num4,{0x34, 5, "The Number 4"}},
            {KeyType::Num5,{0x35, 6, "The Number 5"}},
            {KeyType::Num6,{0x36, 7, "The Number 6"}},
            {KeyType::Num7,{0x37, 8, "The Number 7"}},
            {KeyType::Num8,{0x38, 9, "The Number 8"}},
            {KeyType::Num9,{0x39, 10, "The Number 9"}},

            // arrows
            {KeyType::Up,{VK_UP, 72, "Arrow Up"}},
            {KeyType::Down,{VK_DOWN, 80, "Arrow Down"}},
            {KeyType::Left,{VK_LEFT, 75, "Arrow Left"}},
            {KeyType::Right,{VK_RIGHT, 77, "Arrow Right"}},


            // pairs
/*
            {"lshift",{VK_LSHIFT, 42, "Left Shift"}},
            {"rshift",{VK_RSHIFT, 54, "Right Shift"}},
            {"lalt",{VK_LMENU, 56, "Left Alt"}},
            {"ralt",{VK_RMENU, 56, "Right Alt"}},
            {"lctrl",{VK_LCONTROL, 29, "Left Control"}},
            {"rctrl",{VK_RCONTROL, 29, "Right Conttrol"}},
*/
            {KeyType::OpenBracket,{VK_OEM_4, 26, "Left Bracket"}},
            {KeyType::CloseBracket,{VK_OEM_6, 27, "Right Bracket"}},
            {KeyType::PageUp ,{VK_PRIOR, 73, "Page Up"}},
            {KeyType::PageDown ,{VK_NEXT, 81, "Page Down"}},
            {KeyType::Backslash,{VK_OEM_5, 43, "BackSlash"}},
            {KeyType::Slash,{VK_OEM_2, 53, "ForwardSlash"}},
            {KeyType::Space,{VK_SPACE, 57, "Space"}},
            {KeyType::Backspace,{VK_BACK, 14, "BackSpace"}},
            {KeyType::Enter,{VK_RETURN, 28, "Enter"}},
            {KeyType::Insert,{VK_INSERT, 82, "Insert"}},
            {KeyType::Delete,{VK_DELETE, 83, "Delete"}},

            // generalization
            {KeyType::Shift,{VK_SHIFT, 42, "Shift"}},
            {KeyType::Alt,{VK_MENU, 56, "Alt"}},
            {KeyType::Control,{VK_CONTROL, 29, "Control"}},

            // rest
            {KeyType::PrintScreen,{VK_PRINT, 55, "Print"}},
            {KeyType::Clear,{VK_OEM_CLEAR, UNKNOWN, "Clear"}},
            {KeyType::Home ,{VK_HOME, 71, "Home"}},
            {KeyType::Pause ,{VK_PAUSE, UNKNOWN, "Pause"}},
            {KeyType::End ,{VK_END, 79, "End"}},

            {KeyType::Escape,{VK_ESCAPE, 1, "Escape"}},
            {KeyType::Tab,{VK_TAB, 15, "Tab"}},
            {KeyType::CapsLock,{VK_CAPITAL, 58, "CapsLock"}},
            {KeyType::Tilde,{VK_OEM_3, 41, "Tidle"}},
            {KeyType::Dash,{VK_OEM_MINUS, 12, "Dash"}},
            {KeyType::Equal,{VK_OEM_PLUS, 13, "Equal"}},
            {KeyType::Fn,{0XFF, UNKNOWN, "Fn"}}, // typically on laptops
            {KeyType::Cmd,{ VK_LWIN ,UNKNOWN, "Cmd/Windows Key"}}, // typically the windows key

            {KeyType::Comma,{VK_OEM_COMMA, 51, "Comma"}},
            {KeyType::Colon,{VK_OEM_1, 39, "Colon"}},
            {KeyType::Period,{VK_OEM_PERIOD, 52, "Period"}},
            {KeyType::Quote,{VK_OEM_7, 40, "Quote"}},

            // alphabet
            {KeyType::A,{0x41, 30, "Key A"}},
            {KeyType::B,{0x42, 48, "Key B"}},
            {KeyType::C,{0x43, 46, "Key C"}},
            {KeyType::D,{0x44, 32, "Key D"}},
            {KeyType::E,{0x45, 18, "Key E"}},
            {KeyType::F,{0x46, 33, "Key F"}},
            {KeyType::G,{0x47, 34, "Key G"}},
            {KeyType::H,{0x48, 35, "Key H"}},
            {KeyType::I,{0x49, 23, "Key I"}},
            {KeyType::J,{0x4A, 36, "Key J"}},
            {KeyType::K,{0x4B, 37, "Key K"}},
            {KeyType::L,{0x4C, 38, "Key L"}},
            {KeyType::M,{0x4D, 50, "Key M"}},
            {KeyType::N,{0x4E, 49, "Key N"}},
            {KeyType::O,{0x4F, 24, "Key O"}},
            {KeyType::P,{0x50, 25, "Key P"}},
            {KeyType::Q,{0x51, 16, "Key Q"}},
            {KeyType::R,{0x52, 19, "Key R"}},
            {KeyType::S,{0x53, 31, "Key S"}},
            {KeyType::T,{0x54, 20, "Key T"}},
            {KeyType::U,{0x55, 22, "Key U"}},
            {KeyType::V,{0x56, 47, "Key V"}},
            {KeyType::W,{0x57, 17, "Key W"}},
            {KeyType::X,{0x58, 45, "Key X"}},
            {KeyType::Y,{0x59, 21, "Key Y"}},
            {KeyType::Z,{0x5A, 44, "Key Z"}},

            // numpad
            {KeyType::NumLock,{VK_NUMLOCK, 45, "NumLock"}},
            {KeyType::Numpad0,{ VK_NUMPAD0, 82, "Numpad 0" } },
            {KeyType::Numpad1,{ VK_NUMPAD1, 79, "Numpad 1" } },
            {KeyType::Numpad2,{ VK_NUMPAD2, 80, "Numpad 2" } },
            {KeyType::Numpad3,{ VK_NUMPAD3, 81, "Numpad 3" } },
            {KeyType::Numpad4,{ VK_NUMPAD4, 75, "Numpad 4" } },
            {KeyType::Numpad5,{ VK_NUMPAD5, 76, "Numpad 5" } },
            {KeyType::Numpad6,{ VK_NUMPAD6, 77, "Numpad 6" } },
            {KeyType::Numpad7,{ VK_NUMPAD7, 71, "Numpad 7" } },
            {KeyType::Numpad8,{ VK_NUMPAD8, 72, "Numpad 8" } },
            {KeyType::Numpad9,{VK_NUMPAD9, 73, "Numpad 9"}},
            {KeyType::Add,{VK_OEM_PLUS, 78, "Numpad Plus"}},
            {KeyType::Decimal,{VK_DECIMAL, 83, "Numpad Decimal"}},
            {KeyType::Divide,{VK_DIVIDE, 53, "Numpad Divide"}},
            {KeyType::Multiply,{VK_MULTIPLY, 224, "Numpad Multiply"}},
            {KeyType::Subtract,{VK_OEM_MINUS, 74, "Numpad Minus"}},
            {KeyType::NumpadEnter,{VK_RETURN, 28, "Numpad Enter"}}
        };

    return map;
}


unordered_set<KeyType> returnMouseKeyTypes()
{
    unordered_set<KeyType> mouseKeys;
    for (const auto& key : KeyType::values())
    {
        const string lowercaseKey = util::toLower(key);
        if (lowercaseKey.substr(0, 5) == "mouse")
        {
            mouseKeys.insert(key);
        }
    }

    return mouseKeys;
}

unordered_set<KeyType> mouseKeys = returnMouseKeyTypes();

unordered_map<KeyType, KeyMapping> virtualKeyCodes = create_key_map();
unordered_set<KeyType> currentlyPressed;

void SimulateMouseScroll(float amount, bool vertical)
{
    INPUT input = { 0 };
    input.mi.dwFlags = vertical ? MOUSEEVENTF_WHEEL : MOUSEEVENTF_HWHEEL;
    input.mi.mouseData = static_cast<DWORD>(amount);

    SendInput(1, &input, sizeof(INPUT));
}

// sure ain't the prettiest method!
void SimulateMouse(const vector<KeyType>& keys, bool holding)
{
    vector<INPUT> inputs;
    bool swapped = (GetSystemMetrics(SPI_SETMOUSEBUTTONSWAP) != 0); // non-zero if swapped
    cout << (holding ? "holding" : "releasing") << " mouse keys: " << keys <<  endl;
        
    for (KeyType key : keys)
    {
        ASSERT(key.in(mouseKeys), "key '" << key << "' should be used with SimulateKey instead");
        INPUT input = { 0 };
        input.type = INPUT_MOUSE;

        if (holding)
        {
            switch (key)
            {
            case KeyType::MouseLeft: input.mi.dwFlags = swapped ? MOUSEEVENTF_RIGHTDOWN : MOUSEEVENTF_LEFTDOWN; break;
            case KeyType::MouseRight: input.mi.dwFlags = swapped ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN; break;
            case KeyType::MouseScrollWheel: input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN; break;
            case KeyType::MouseXButton1: /* fallthrough */;
            case KeyType::MouseXButton2: input.mi.dwFlags = MOUSEEVENTF_XDOWN; break;
            }
        }
        else
        {
            switch (key)
            {
            case KeyType::MouseLeft: input.mi.dwFlags = swapped ? MOUSEEVENTF_RIGHTUP : MOUSEEVENTF_LEFTUP; break;
            case KeyType::MouseRight: input.mi.dwFlags = swapped ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP; break;
            case KeyType::MouseScrollWheel: input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
            case KeyType::MouseXButton1: /* fallthrough */;
            case KeyType::MouseXButton2: input.mi.dwFlags = MOUSEEVENTF_XUP; break;
            }
        }

        inputs.push_back(input);
    }

    SendInput(static_cast<UINT>(keys.size()), inputs.data(), sizeof(INPUT));
}

#define KEYEVENTF_KEYDOWN 0

void SimulateKey(const vector<KeyType>& keys, bool holding, bool usingScanCode)
{
    vector<INPUT> inputs;
    cout << (holding ? "holding" : "releasing") << " keyboard keys: " << keys << endl;

    for (KeyType key : keys)
    {
        ASSERT(!key.in(mouseKeys), "key '" << key << "' should be used with SimulateMouse instead");
        INPUT input = { 0 };
        input.type = INPUT_KEYBOARD;

        if (holding)
        {
            if (usingScanCode)
            {
                input.ki.wScan = virtualKeyCodes[key].scanCodeId;
                input.ki.dwFlags = KEYEVENTF_SCANCODE | 0;
            }
            else
            {
                input.ki.wVk = virtualKeyCodes[key].virtualKeyId;
                input.ki.dwFlags = KEYEVENTF_KEYDOWN;
            }
        }
        else
        {
            if (usingScanCode)
            {
                input.ki.wScan = virtualKeyCodes[key].scanCodeId;
                input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            }
            else
            {
                input.ki.wVk = virtualKeyCodes[key].virtualKeyId;
                input.ki.dwFlags = KEYEVENTF_KEYUP;
            }
        }

        inputs.push_back(input);
    }

     SendInput(static_cast<UINT>(keys.size()), inputs.data(), sizeof(INPUT));
}

void ReleaseAllHeldKeys()
{
//    lock_guard<mutex> lock(keyLockGuard);
    vector<KeyType> toRelease(currentlyPressed.begin(), currentlyPressed.end());
    vector<KeyAction> actions = { KeyAction(InputActionType::Release, toRelease) };
    if (toRelease.size() > 0) SimulateKeys({});
}

void SimulateKeys(vector<KeyAction> keyActions, bool usingScancode)
{
    for (auto group : keyActions)
    {
        // mouse is handled differently from keyboard.
        vector<KeyType> mouse, keyboard;
        for (KeyType key : group.keys)
        {
            currentlyPressed.erase(key);
            if (key.in(mouseKeys)) mouse.push_back(key); else keyboard.push_back(key);
        }

        // run the action on this group of keys
        if (group.action == InputActionType::Release) 
        {
            if (mouse.size() > 0) SimulateMouse(mouse, false);
            if (keyboard.size() > 0) SimulateKey(keyboard, false, usingScancode);
        }
        else if (group.action == InputActionType::Hold)
        {
            if (mouse.size() > 0) SimulateMouse(mouse, true);
            if (keyboard.size() > 0) SimulateKey(keyboard, true, usingScancode);
        }
        else if (group.action == InputActionType::Press)
        {
            if (mouse.size() > 0) SimulateMouse(mouse, true);
            if (keyboard.size() > 0) SimulateKey(keyboard, true, usingScancode);
            if (mouse.size() > 0) SimulateMouse(mouse, false);
            if (keyboard.size() > 0) SimulateKey(keyboard, false, usingScancode);
        }
    }
}

void SetCursorPos(vec2 pos)
{
    SetCursorPos(static_cast<int>(round(pos.x)), static_cast<int>(round(pos.y)));
}

/// Returns the amount moved 
void MoveCursorToPos(vec2 target)
{
//    vec2 curPos = GetCursorPos();
//    if (curPos.)

    vec2 lerped = util::lerp(GetCursorPos(), target, 0.8f);
    SetCursorPos(static_cast<int>(round(lerped.x)), static_cast<int>(round(lerped.y)));
}

vec2 GetCursorPos()
{
    POINT cur;
    GetCursorPos(&cur);
    vec2 current(static_cast<float>(cur.x), static_cast<float>(cur.y));
    return current;
}

vec2 GetTotalScreenSize()
{
    float x = static_cast<float>(GetSystemMetrics(SM_CXSCREEN));
    float y = static_cast<float>(GetSystemMetrics(SM_CYSCREEN));

    return vec2(x, y);
}

void wait(float milliseconds)
{
    this_thread::sleep_for(chrono::milliseconds(static_cast<int>(milliseconds)));
}

#elif __linux__

#endif

