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
    cout << "Loading the settings..." << endl;

    initializeSettingsFromConfigFile("settings/config.txt");
    initializeSettingsFromConfigFile("settings/options/" + settings["options"]);
    initializeKeyMappingsFromMappingFile("settings/mappings/" + settings["mappings"]);

	cout << "Finished loading settings!" << endl;

    cout << "Connecting for the first time..." << endl;
    sf::Joystick::update();
    while (!sf::Joystick::isConnected(id))
    {
        cout << "Still trying to connect..." << endl;
        wait(stof(settings["auto-reconnect-frequency"]));
        sf::Joystick::update();
    }
	cout << "Connected to controller!\n";

	cout << "Preparing the controller..." << endl;
	setupAxes();
    initializeKeyBindingsFromBindingFile("settings/bindings/" + settings["bindings"]);
	cout << "Controller is ready to go!" << endl;

	cout << "Beginning to poll controller." << endl;
	connected = true;

    keepPollingThreadsAlive = true;
    mainPollingThread = thread([&] { updateAxes(); });
    bindingsUpdateThread = thread([&] { updateBindings(); });

	mainPollingThread.join();
	bindingsUpdateThread.join();
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

void Controller::updateAxes()
{
    const float controllerReconnectFrequency = stof(settings["auto-reconnect-frequency"]);
    const float pollFrequency = stof(settings["poll-frequency"]);

    wait(100); // give the cursor a little time to pick up.

    vec2 mouseLocalOffset;
    vec2 mouseLocalTargetPos; // relative to the cursor position
    vec2 accumulator;

    vec2 screenSize = GetTotalScreenSize();


    while (keepPollingThreadsAlive)
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

        mouseLocalTargetPos += leftStick->state * leftStick->sensitivity * 20;
        mouseLocalOffset = util::lerp(mouseLocalOffset, mouseLocalTargetPos, 0.75f);


        const vec2 diff = mouseLocalTargetPos - mouseLocalOffset;
        if (fabs(diff.x) < 0.5) accumulator.x += diff.x;
        if (fabs(diff.y) < 0.5) accumulator.y += diff.y;

        bool xTooSmall = fabs(accumulator.x) < 0.5f;
        bool yTooSmall = fabs(accumulator.y) < 0.5f;

        SetCursorPos(GetCursorPos() + diff);
        SetCursorPos(GetCursorPos() + accumulator);
            
        if (!xTooSmall) accumulator.x = 0;
        if (!yTooSmall) accumulator.y = 0;

        
        

//        mouseLocalTargetPos.clip(vec2(0, 0), vec2(screenSize.x, screenSize.y));
//        MoveCursorToPos(mouseLocalTargetPos);




//        ScrollMouseVertical(rightStick->state.y * rightStick->sensitivity * -0.5f);
//        ScrollMouseHorizontal(rightStick->state.x * 5);

        for (auto& axis : axes)
        {
            axis->update();
        }

		wait(pollFrequency); // pulling at 100% CPU usage would be really wasteful
	}
}

void Controller::initializeKeyMappingsFromMappingFile(const string& filename)
{
    // load the controller mapping
    ifstream mappingFile(filename);
    for (std::string line; getline(mappingFile, line); )
    {
        const auto tokens = util::split(util::toLower(util::removeSpaces(line)), '=');
        const string& userButtonName = tokens[0];
        userButtonNameToId[userButtonName] = tokens[1];
    }
}

bool fail(bool condition, int lineNumber, const string& message)
{
    if (!condition)
    {
        cerr << "Binding Ignored... line " << lineNumber << ": " << message << "." << endl;
        return true;
    } 

    return false;
}

// TODO: This method is a little hefty - find a way to trim it down.
void Controller::initializeKeyBindingsFromBindingFile(const string& filename)
{
    // load the controller mapping
    ifstream bindingFile(filename);
    int lineNumber = 0;

    for (std::string line; getline(bindingFile, line); )
    {
        lineNumber++;

        // skip empty lines and comments
        if (line.substr(0, 2) == "--" || line.empty()) continue;

        // case insensitive
        line = util::toLower(line);

        // PART ONE, every binding should begin with 'bind'
        if (fail(line.substr(0, 4) == "bind", lineNumber, "Expected 'bind' at the start of the line")) return;

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

            if (fail(actionType.isDefined(), lineNumber, "'" + tokens[0] + "' is not a valid controller action")) return;

            vector<ButtonType> actionButtons;
            for (int i = 1; i < tokens.size(); i++)
            {
                const string buttonString = tokens[i];
                bool isPhysical = (userButtonNameToId.count(buttonString) > 0);
                ButtonType buttonType;
                if (isPhysical)
                {
                    buttonType = ButtonType::from("button" + userButtonNameToId[buttonString]);
                }
                else
                {
                    buttonType = ButtonType::from(buttonString);
                }

                if (fail(buttonType.isDefined(), lineNumber, "'" + buttonString + "' is not a registered controller button")) return;

                actionButtons.push_back(buttonType);
            }

            bindingFrom.push_back({ actionType, actionButtons });
        }

        // PART TWO, every binding should be followed with a 'to'
        getline(bindingFile, line);

        lineNumber++;

        // case insensitive
        line = util::toLower(line);

        if (fail(line.substr(0, 2) == "to", lineNumber, "Expected 'to' at the start of the line")) return;

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

            if (fail(actionType.isDefined(), lineNumber, "'" + tokens[0] + "' is not a valid key action")) return;

            // associate several keys with that action type (and validate their existence)
            vector<KeyType> actionKeys;
            for (int i = 1; i < tokens.size(); i++)
            {
                const string& token = tokens[i];
                KeyType keyType = KeyType::from(token);

                if (fail(keyType.isDefined(), lineNumber, "'" + token + "' is not a registered key")) return;
                actionKeys.push_back(keyType);
            }

            bindingTo.push_back({ actionType, actionKeys });
        }


        // add our compiled binding to the binding map.
        keyBindings.push_back(make_unique<Binding>(bindingFrom, bindingTo));
    }

}

void Controller::initializeSettingsFromConfigFile(const string& filename)
{

    // load the general controller settings
    ifstream configFile(filename);
    for (std::string line; getline(configFile, line); )
	{
        auto tokens = util::split(line, '=');
        const string setting = util::toLower(util::removeSpaces(tokens[0]));
        const string value = util::toLower(util::removeSpaces(tokens[1]));

        settings[setting] = value;
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
    float buttonPressAndReleaseDelay = stof(settings["button-press-and-release-time"]);

    

	while (keepPollingThreadsAlive && isConnected())
	{
        if (buttonHeldDown(ButtonType::RightStickUp)) cout << "woo\n";

		// check if the bindings were pressed
		for (auto& binding : keyBindings)
		{
			if (binding->triggered(this))
			{
				binding->runAction(this);
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