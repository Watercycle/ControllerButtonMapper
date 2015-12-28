#include "stdafx.h"
#include "trigger.h"
#include <unordered_map>
#include "controller.h"

Trigger::Trigger(sf::Joystick::Axis button, Controller* controller, const AxisType& name)
    : Axis(controller, name), button(button), softThreshold(0), hardThreshold(0)
{
}

void Trigger::update()
{
    state = sf::Joystick::getAxisPosition(controller->id, button);
    state = util::scale(state, -100, 100, 0, 1);

}

float Trigger::getThreshold()
{
    return state;
}


void Trigger::updateSettings(unordered_map<string, string>& settings)
{
    softThreshold = stof(settings["trigger-soft"]);
    hardThreshold = stof(settings["trigger-hard"]);
}

void Trigger::setupVirtualButtons()
{
    makeVirtualButton("", softThreshold, Direction::Down);
    makeVirtualButton("Hard", hardThreshold, Direction::Down);
}