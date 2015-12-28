#include "stdafx.h"
#include "virtual_button.h"
#include "axis.h"


VirtualButton::VirtualButton(Controller* controller, function<bool()> pressed) : Button(controller), pressed(pressed)
{

}

bool VirtualButton::isPressed()
{
	return pressed();
}