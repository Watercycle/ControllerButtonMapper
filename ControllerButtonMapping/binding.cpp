#include "stdafx.h"
#include "binding.h"
#include "binding_enums.h"
#include "controller.h"

Binding::~Binding()
{
        
}

Binding::Binding(const vector<ControllerAction>& triggers, const vector<KeyAction>& responses)
    : triggers(triggers), keysToPress(responses)
{

}

bool Binding::triggered(Controller* controller) const
{
    for (ControllerAction action : triggers)
    {
        if (!action.triggered(controller))
        {
            return false;
        }
    }

    return true;
}

void Binding::runAction(Controller* controller)
{
    SimulateKeys(keysToPress);
}
/*
Binding::Binding(const vector<ActionBinding>& triggers, const vector<ActionBinding>& actions): trigger(triggers), actions(actions)
{
        
}

*/
