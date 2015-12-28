#pragma once
//#include "binding_enums.h"
#include "binding_enums.h"

class Controller;
//struct ControllerAction;

class Binding
{
public:
    ~Binding();

    const vector<ControllerAction> triggers;
    const vector<KeyAction> keysToPress;

    Binding(const vector<ControllerAction>& triggers, const vector<KeyAction>& responses);

    void runAction(Controller* controller);
    bool triggered(Controller* controller) const;
};
