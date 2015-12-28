#pragma once
#include "control.h"
#include "button.h"

class Axis;

class VirtualButton : public Button
{
public:
	function<bool()> pressed;

    VirtualButton(Controller* controller, function<bool()> pressed);

    bool isPressed() override;
};
