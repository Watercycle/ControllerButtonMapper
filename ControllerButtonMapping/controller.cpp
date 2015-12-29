#include "stdafx.h"
#include "controller.h"
#include "binding.h"
#include <fstream>
#include "button.h"
#include "dpad.h"
#include "trigger.h"
#include "stick.h"
#include "binding_enums.h"
#include "physical_button.h"
#include "window.h"

Controller::~Controller()
{
    disconnect();
}

Controller::Controller() : id(id)
{
}

void Controller::setupButtons() 
{
    for (int i = 0; i < buttonCount; i++)
    {
        auto button = make_shared<PhysicalButton>(this, i);
        buttons.insert({ ButtonType::from("button" + to_string(i)), button });
    }
}

shared_ptr<Stick> Controller::setupStick(const AxisType& name, sf::Joystick::Axis first, sf::Joystick::Axis second)
{
    if (sf::Joystick::hasAxis(id, first) && sf::Joystick::hasAxis(id, second))
    {
        auto stick = make_shared<Stick>(first, second, this, name);
        axes.push_back(stick);
        return stick;
    }

    return nullptr;
}

shared_ptr<DPad> Controller::setupDPad(const AxisType& name, sf::Joystick::Axis leftRight, sf::Joystick::Axis upDown)
{
    if (sf::Joystick::hasAxis(id, leftRight) && sf::Joystick::hasAxis(id, upDown))
    {
        auto dPad = make_shared<DPad>(leftRight, upDown, this, name);
        axes.push_back(dPad);
        return dPad;
    }

    return nullptr;
}

shared_ptr<Trigger> Controller::setupTrigger(const AxisType& name, sf::Joystick::Axis trigger)
{
    if (sf::Joystick::hasAxis(id, trigger))
    {
        auto trig = make_shared<Trigger>(trigger, this, name);
        axes.push_back(trig);
        return trig;
    }

    return nullptr;
}

void Controller::disconnect()
{
    connected = false;
    keepPollingThreadsAlive = false;
    keepBindingThreadsAlive = false;
    keepAxisPollingThreadAlive = false;

    backgroundPollingThread.join();
    mainPollingThread.join();
    bindingsUpdateThread.join();

    ReleaseAllHeldKeys();
}

void Controller::setupAxes()
{
    buttonCount = sf::Joystick::getButtonCount(id);

    setupButtons();
    setupDPad(AxisType::DPad, sf::Joystick::Axis::PovX, sf::Joystick::Axis::PovY);
    leftStick = setupStick(AxisType::LeftStick, sf::Joystick::X, sf::Joystick::Y);
    rightStick = setupStick(AxisType::RightStick, sf::Joystick::V, sf::Joystick::U);
    leftTrigger = setupTrigger(AxisType::LeftTrigger, sf::Joystick::Axis::Z);
    rightTrigger = setupTrigger(AxisType::RightTrigger, sf::Joystick::Axis::R);

    for (auto& axis : axes)
    {
        axis->updateSettings(settings);
        axis->setupVirtualButtons();
    }
}

void Controller::connect(unsigned controllerId)
{
    id = controllerId;

    initializeKeyMappingsFromMappingFile(appSettings, "settings/config.txt");
    initializeKeyMappingsFromMappingFile(userButtonNameToId, "settings/mappings/" + appSettings[SettingType::MappingPath]);
    initializeKeyMappingsFromMappingFile(userApplicationList, "settings/app-binding-list.txt");

    backgroundPollingThread = thread([&] { runBackgroundTasks(); });
    mainPollingThread = thread([&] { updateAxes(); });
    bindingsUpdateThread = thread([&] { updateBindings(); });

    // wait for all of the threads to terminate
    backgroundPollingThread.join();
	mainPollingThread.join();
	bindingsUpdateThread.join();
}

void Controller::reinitializeSettingsAndBindings(const string& bindingFile)
{
    keepBindingThreadsAlive = false;
    keepAxisPollingThreadAlive = false;
    bindingsUpdateThread.join();
    mainPollingThread.join();

    buttons.clear();
    settings.clear();
    keyBindings.clear();

    initializeKeyBindingsFromBindingFile("settings/bindings/" + bindingFile,  true);
    keepBindingThreadsAlive = true;
    keepAxisPollingThreadAlive = true;
    mainPollingThread = thread([&] { updateAxes(); });
    bindingsUpdateThread = thread([&] { updateBindings(); });
}

string currentBinding;
wstring topWindowName;
void Controller::runBackgroundTasks()
{
    while (keepPollingThreadsAlive)
    {
        wstring newTopWindowName = Window::GetTopWindowExecutableName();
        if (topWindowName != newTopWindowName)
        {
            const string withoutExe = util::toLower(util::toString(newTopWindowName));
            cout << "now using '" << withoutExe << "'" << endl;

            if (userApplicationList.count(withoutExe))
            {
                const string newBindingToUse = userApplicationList[withoutExe];
                cout << "Reinitializing settings for '" << withoutExe << "'.  Switching from '" 
                    << currentBinding << "' to '" << newBindingToUse << "'." << endl;
                currentBinding = newBindingToUse;
                reinitializeSettingsAndBindings(newBindingToUse);
            }
            else
            {
                if (currentBinding != "default.bind")
                {
                    cout << "Reverting to default bindings" << endl;
                    reinitializeSettingsAndBindings("default.bind");
                    currentBinding = "default.bind";
                }
            }
        }

        topWindowName = newTopWindowName;

        wait(500);
    }
}

void Controller::initializeKeyMappingsFromMappingFile(unordered_map<SettingType, string>& map, const string& filename)
{

    // load the general controller settings
    ifstream configFile(filename);
    for (std::string line; getline(configFile, line); )
    {
        // skip empty lines and comments
        if (line.substr(0, 2) == "--" || line.empty()) continue;

        auto tokens = util::split(line, '=');
        SettingType setting = SettingType::from(util::removeSpaces(tokens[0]));
        const string value = util::toLower(util::removeSpaces(tokens[1]));

        map[setting] = value;
    }
}

void Controller::connectToFirstAvaialblePort()
{
    // block until a connection is found
    while (!connected)
    {
        sf::Joystick::update();
        for (int i = 0; i < sf::Joystick::Count; i++)
        {
            if (sf::Joystick::isConnected(i))
            {
                connect(i);
                cout << "Found a controller!" << endl;
                return;
            }
        }

        cout << "Looking for a controller to connect to..." << endl;
        wait(1000);
    }
}

bool Controller::isConnected() const
{
	return connected;
}

vec2 mouseCurrentPos;
vec2 mouseTargetPos; // relative to the cursor position
vec2 accumulator;

void controlCursor(const shared_ptr<Stick> primary, const shared_ptr<Stick> secondary)
{
    if (primary == nullptr) return;

    // There actual position doesn't matter so much as how the 'current' vector approaches
    // the 'target vector'
    mouseTargetPos += primary->state * primary->sensitivity * 20;
    mouseCurrentPos = util::lerp(mouseCurrentPos, mouseTargetPos, 0.75f);
    const vec2 change = mouseTargetPos - mouseCurrentPos;

    // movement was too small to be recognized, so we accumulate it
    if (fabs(change.x) < 0.5f) accumulator.x += change.x;
    if (fabs(change.y) < 0.5f) accumulator.y += change.y;

    // If the value is too small to be recognized ( < 0.5 ) then the position will remain the same
    SetCursorPos(GetCursorPos() + change);
    SetCursorPos(GetCursorPos() + accumulator);

    // once the accumulator has been used, reset it for the next accumulation.
    if (fabs(accumulator.x) >= 0.5f) accumulator.x = 0;
    if (fabs(accumulator.y) >= 0.5f) accumulator.y = 0;

    if (secondary == nullptr) return;

    SimulateMouseScroll(secondary->state.y * secondary->sensitivity * -2.0f, true);
    SimulateMouseScroll(secondary->state.x * secondary->sensitivity * 2.0f, false);
}

void Controller::updateAxes()
{
    bool test = 1;
    const float controllerReconnectFrequency = 200;
    const float pollFrequency = 10;

    shared_ptr<Stick> primary = nullptr;
    shared_ptr<Stick> secondary = nullptr;
    string stickSetting = settings[SettingType::MousePrimaryStick];
    bool usingScrolling = settings[SettingType::UseControlStickScrolling] == "true" ? true : false;

    if (stickSetting == "leftstick")
    {
        primary = leftStick;
        if (usingScrolling) secondary = rightStick;
    }
    else if (stickSetting == "rightstick")
    {
        primary = rightStick;
        if (usingScrolling) secondary = leftStick;
    }
    


    wait(100); // give the cursor a little time to pick up.

    while (keepAxisPollingThreadAlive)
    {
		// Check whether the controller is connected
		// ===============================================

        sf::Joystick::update();
        if (!sf::Joystick::isConnected(id))
        {
            cout << "Lost connection... Trying To Reconnect...\n";
			connected = false;
			wait(controllerReconnectFrequency);
			continue; // don't do anything until connected
        }

		connected = true;

        // Poll axis states
        // ===============================================

        controlCursor(primary, secondary);

        for (auto& axis : axes)
        {
            axis->update();
        }

		wait(pollFrequency); // pulling at 100% CPU usage would be really wasteful
	}
}

void Controller::initializeKeyMappingsFromMappingFile(unordered_map<string, string>& map, const string& filename)
{
    // load the controller mapping
    ifstream mappingFile(filename);

    for (std::string line; std::getline(mappingFile, line); )
    {
        // skip empty lines and comments
        if (line.substr(0, 2) == "--" || line.empty()) continue;

        const auto tokens = util::split(util::toLower(util::removeSpaces(line)), '=');
        const string& userButtonName = tokens[0];
        map[userButtonName] = tokens[1];
    }
}

std::string line;

void need(bool expected, const string& message, int lineNumber)
{
    if (!expected)
    {
        cerr << "Failed To Compile Bindings... line " << lineNumber << ": " << message << "." << endl;
        cin.ignore();
        exit(EXIT_FAILURE);
    }
}

/* skip empty lines and comments*/
void readToNextValidLine(ifstream& file, int& lineNumber)
{
    while (std::getline(file, line)) 
    {
        lineNumber++;
        line = util::trim(line);
        if (line.substr(0, 2) != "--" && !line.empty())
        {
            line = util::toLower(line);
            break;
        }
    }
}

// TODO: This method is a little hefty - find a way to trim it down.
void Controller::initializeKeyBindingsFromBindingFile(const string& filename, bool firstCall)
{
    // load the controller mapping
    ifstream bindingFile(filename);
    int lineNumber = 1;

    readToNextValidLine(bindingFile, lineNumber); // ignore initial whitespace
    need(line == "settings:", "Expected 'settings' at the beginning of the file", lineNumber);
    readToNextValidLine(bindingFile, lineNumber); // read over 'settings'

    // read in all of the settings
    while (line != "bindings:")
    {
        const auto tokens = util::split(line, '=');
        SettingType setting = SettingType::from(util::removeSpaces(tokens[0]));
        need(setting.isDefined(), "Setting name '" + tokens[0] + "' is not a valid setting", lineNumber);

        const string value = util::toLower(util::removeSpaces(tokens[1]));
        settings[setting] = value;
        readToNextValidLine(bindingFile, lineNumber);
    }

    need(line == "bindings:", "Expected 'bindings' at the end of settings", lineNumber);
    readToNextValidLine(bindingFile, lineNumber); // read over 'bindings'

    // this is a good time to do it!
    if (firstCall) setupAxes();

    while (bindingFile)
    {
        // PART ONE, every binding should begin with 'bind'
        need(line.substr(0, 4) == "bind", "Expected 'bind' at the start of the line", lineNumber);

        // skip the 'bind' directive and split up the binding groups
        const vector<string> actionBindingGroups = util::split(line.substr(5, line.length()), '+');

        // this is where we will collect our 'bind' ActionBindings
        vector<ControllerAction> bindingFrom;

        // turn each binding group into a ActionBinding
        for (const string& group : actionBindingGroups)
        {
            // get the action type from the split up keys
            const auto tokens = util::split(group, ' ');
            InputActionType actionType = InputActionType::from(tokens[0]);

            need(actionType.isDefined(), "'" + tokens[0] + "' is not a valid controller action", lineNumber);

            vector<ButtonType> actionButtons;
            for (int i = 1; i < tokens.size(); i++)
            {
                const string buttonString = tokens[i];
                bool isPhysical = (userButtonNameToId.count(buttonString) > 0);
                ButtonType buttonType;
                if (isPhysical)
                {
                    buttonType = ButtonType::from("button" + userButtonNameToId[buttonString]);
                    need(buttonType.isDefined(), "'" + buttonString + "' is not a registered controller button", lineNumber);
                }
                else
                {
                    buttonType = ButtonType::from(buttonString);
                    need(buttonType.isDefined(), "'" + buttonString + "' is not a registered controller button", lineNumber);
                    need(buttons.count(buttonType) > 0, "'" + buttonString + "' is valid, but not detected on your controller", lineNumber);
                }

                actionButtons.push_back(buttonType);
            }

            bindingFrom.push_back({ actionType, actionButtons });
        }

        // PART TWO, every binding should be followed with a 'to'
        readToNextValidLine(bindingFile, lineNumber);

        need(line.substr(0, 2) == "to", "Expected 'to' at the start of the line", lineNumber);

        // skip the 'to' directive and split up the binding groups
        const vector<string> actionToGroups = util::split(line.substr(3, line.length()), '+');

        // this is where we will collect our 'to' ActionBindings
        vector<KeyAction> bindingTo;

        // turn each binding group into a BindingAction
        for (const string& group : actionToGroups)
        {
            // get the action type from the split up keys
            const vector<string> tokens = util::split(group, ' ');
            InputActionType actionType = InputActionType::from(tokens[0]);

            need(actionType.isDefined(), "'" + tokens[0] + "' is not a valid key action", lineNumber);

            // associate several keys with that action type (and validate their existence)
            vector<KeyType> actionKeys;
            for (int i = 1; i < tokens.size(); i++)
            {
                const string& token = tokens[i];
                KeyType keyType = KeyType::from(token);

                need(keyType.isDefined(), "'" + token + "' is not a registered key", lineNumber);
                actionKeys.push_back(keyType);
            }

            bindingTo.push_back({ actionType, actionKeys });
        }

        readToNextValidLine(bindingFile, lineNumber);

        // add our compiled binding to the binding map.
        keyBindings.push_back(make_unique<Binding>(bindingFrom, bindingTo));
    }
}

bool Controller::buttonPressed(const ButtonType& button)
{
    auto control = buttons[button];
    bool pressedNow = control->isPressed();
    bool wasNotPressedBefore = !control->wasPressedLastFrame();
    return pressedNow && wasNotPressedBefore;
}

bool Controller::buttonReleased(const ButtonType& button)
{
    auto control = buttons[button];
    return !control->isPressed() && control->wasPressedLastFrame();
}

bool Controller::buttonHeldDown(const ButtonType& button)
{
    auto control = buttons[button];
    return control->isPressed() && control->wasPressedLastFrame();
}

void Controller::updateBindings()
{
    // typically 25-75ms.  higher values make it easier to press a controller button combo
    float buttonPressAndReleaseDelay = 50;

	while (keepBindingThreadsAlive)
	{
        if (!isConnected()) continue;

		// check if the bindings were pressed
		for (auto& binding : keyBindings)
		{
			if (binding->triggered(this))
			{
				binding->runAction(this);

                // combos will over-shadow individual buttons
                if (binding->triggers.size() > 1 || binding->triggers[0].buttons.size() > 1)
                {
                    cout << "OVERSHADOW" << endl;
                    break;
                }
            }
		}

		// let the buttons know if they were pressed this frame
		for (auto& part : buttons)
		{
			part.second->update();
		}

		wait(buttonPressAndReleaseDelay); // a delay so that press/release combos are easier to do
	}
}