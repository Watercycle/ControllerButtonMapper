#pragma once
#include "control.h"
#include "button.h"

class PhysicalButton : public Button
{
public:
    unsigned id;

    PhysicalButton(Controller* controller, unsigned id);

    bool isPressed() override;
};