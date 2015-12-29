#include "stdafx.h"
#include "dpad.h"
#include <unordered_map>
#include "controller.h"

DPad::DPad(sf::Joystick::Axis leftRight, sf::Joystick::Axis upDown, Controller* controller, const AxisType& name)
    : Axis(controller, name), leftRight(leftRight),upDown(upDown), threshold(0)
{
}

void DPad::update()
{
    state = vec2(sf::Joystick::getAxisPosition(controller->id, sf::Joystick::PovX), sf::Joystick::getAxisPosition(controller->id, sf::Joystick::PovY));
    state = vec2(util::scale(state.x, -100, 100, -1, 1), util::scale(state.y, -100, 100, -1, 1));
}

// detect quadrant based on an ' X '
bool DPad::inRegion(const Direction& targetRegion) const
{
	bool lowerLeft = state.y > state.x;
	bool lowerRight = state.y > -state.x;

	Direction region;
	if (lowerLeft && lowerRight) region = Direction::Up;
	else if (lowerLeft && !lowerRight) region = Direction::Left;
	else if (!lowerLeft && !lowerRight) region = Direction::Down;
	else region = Direction::Right;

	return region == targetRegion;
}

float DPad::getThreshold()
{
    return state.lengthSquared();
}

void DPad::updateSettings(unordered_map<SettingType, string>& settings)
{
    threshold = 0.5;
}


void DPad::setupVirtualButtons()
{
    makeVirtualButton("Up", threshold, Direction::Up);
    makeVirtualButton("Right", threshold, Direction::Right);
    makeVirtualButton("Down", threshold, Direction::Down);
    makeVirtualButton("Left", threshold, Direction::Left);
}
