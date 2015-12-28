#include "stdafx.h"
#include "physical_button.h"
#include "controller.h"

PhysicalButton::PhysicalButton(Controller* controller, unsigned id) : Button(controller), id(id)
{
}

bool PhysicalButton::isPressed()
{
    return sf::Joystick::isButtonPressed(controller->id, id);
}