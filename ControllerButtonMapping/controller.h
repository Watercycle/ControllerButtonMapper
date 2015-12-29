#pragma once
#include <unordered_map>
#include "binding_enums.h"

class Stick;
class DPad;
class Trigger;
class Binding;
class Button;
class Axis;

enum class Option
{
	
};

class Controller
{
public:
    unsigned id = 0;
    abool connected = false;
	abool keepPollingThreadsAlive = true;
    abool keepBindingThreadsAlive = true;
    abool keepAxisPollingThreadAlive = true;

    int buttonCount = 0;
    int stickCount = 0;
    int dPadCount = 0;
    int triggerCount = 0;

    unordered_map<ButtonType, shared_ptr<Button>> buttons;
    vector<shared_ptr<Axis>> axes;

    unordered_map<SettingType, string> appSettings;
    unordered_map<SettingType, string> settings; // controlelr settings
    unordered_map<string, string> userButtonNameToId;
    unordered_map<string, string> userApplicationList;
    vector<unique_ptr<Binding>> keyBindings;

    // kept for reading real-time input. will be a nullptr if not supported by the controller.
    shared_ptr<Stick> leftStick;
    shared_ptr<Stick> rightStick;
    shared_ptr<Trigger> leftTrigger;
    shared_ptr<Trigger> rightTrigger;

	thread mainPollingThread;
	thread backgroundPollingThread;
	thread bindingsUpdateThread;

public:
    ~Controller();
    Controller();

    void connect(unsigned controllerId);
    void connectToFirstAvaialblePort();
    void disconnect();

    void setupAxes();
    void setupButtons();
    shared_ptr<Stick> setupStick(const AxisType& name, sf::Joystick::Axis first, sf::Joystick::Axis second);
    shared_ptr<DPad> setupDPad(const AxisType& name, sf::Joystick::Axis leftRight, sf::Joystick::Axis upDown);
    shared_ptr<Trigger> setupTrigger(const AxisType& name, sf::Joystick::Axis trigger);
    
    void reinitializeSettingsAndBindings(const string& bindingFile);
    void initializeKeyMappingsFromMappingFile(unordered_map<string, string>& map, const string& filename);
    void initializeKeyBindingsFromBindingFile(const string& filename, bool firstCall);
    void initializeKeyMappingsFromMappingFile(unordered_map<SettingType, string>& map, const string& filename);

    void runBackgroundTasks();
    void updateAxes();
    void updateBindings();

    bool buttonPressed(const ButtonType& button);
    bool buttonReleased(const ButtonType& button);
    bool buttonHeldDown(const ButtonType& button);
	bool isConnected() const;
};
