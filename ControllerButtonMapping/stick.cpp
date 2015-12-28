#include "stdafx.h"
#include "stick.h"
#include "controller.h"
#include "virtual_button.h"

Stick::Stick(sf::Joystick::Axis leftRight, sf::Joystick::Axis upDown, Controller* controller, const AxisType& name)
    : Axis(controller, name), leftRight(leftRight), upDown(upDown), sensitivity(0), radius(0), 
      deadzone(0), defaultSensitivity(0), usingAcceleration(false)
{
}

void Stick::update()
{
    // pull the raw position state from controller's analog stick
    state = vec2(sf::Joystick::getAxisPosition(controller->id, leftRight), sf::Joystick::getAxisPosition(controller->id, upDown));

    // normalize the state
    state = vec2(util::scale(state.x, -100, 100, -1, 1), util::scale(state.y, -100, 100, -1, 1));

    // apply a deadzone to prevent drifting in crappy controllers
    float currentThreshold = getThreshold();
    if (currentThreshold < deadzone)
    {
        state = vec2(0, 0);
    }
    else
    {
        // reduce the harshness of coming out of the deazone  by scaling it
        state *= (state.length() - deadzone) / (1 - deadzone);

        // most people suck at moving just up or down naturally, so we help them out a bit.
        const float yBias = fabs(state.y / state.x);

        // e.g. if the y axis is being used 500% more than the x axis then
        // 90% of the motion will apply to the y axis.
        const float axialCorrectionTrigger = 5.0f;
        const float axialBiasStrength = 0.9f;

        // e.g. when one direction is being used twice as much as the other.
        if (yBias > axialCorrectionTrigger)
        {
            // bias to the y axis
            state.x *= (1 - axialBiasStrength);
        }
        else if (yBias < (1.0f / axialCorrectionTrigger))
        {
            // bias to the x axis
            state.y *= (1 - axialBiasStrength);
        }

        // use a logrithmic acceleration for sensitivity
        if (usingAcceleration) sensitivity = defaultSensitivity * exp(2.68f * currentThreshold);
    }
}

// detect quadrant based on an ' X '
bool Stick::inRegion(const Direction& targetRegion) const
{
    bool lowerLeft = state.y > state.x;
    bool lowerRight = state.y > -state.x;

	Direction region;
    if (lowerLeft && lowerRight) region =  Direction::Down;
    else if (lowerLeft && !lowerRight) region = Direction::Left;
    else if (!lowerLeft && !lowerRight) region = Direction::Up;
    else region = Direction::Right;

	return region == targetRegion;
}

// returns the distance as a number from 0 to 1
float Stick::getThreshold()
{
    // sticks tend to get a little off-center so we compensate by capping the value
    return min(state.lengthSquared(), 1.0f);
}

void Stick::updateSettings(unordered_map<string, string>& settings)
{
    radius = 100;
    softThreshold = stof(settings["stick-soft"]);
    hardThreshold = stof(settings["stick-hard"]);
    deadzone = stof(settings[static_cast<string>(name) + "-deadzone"]);
    usingAcceleration = settings[static_cast<string>(name) + "-using-acceleration"] == "true" ? true : false;
    defaultSensitivity = stof(settings[static_cast<string>(name) + "-sensitivity"]);
    sensitivity = defaultSensitivity;
}


void Stick::setupVirtualButtons()
{
    makeVirtualButton("Up", softThreshold, Direction::Up);
    makeVirtualButton("Right", softThreshold, Direction::Right);
    makeVirtualButton("Down", softThreshold, Direction::Down);
    makeVirtualButton("Left", softThreshold, Direction::Left);

    makeVirtualButton("UpHard", hardThreshold, Direction::Up);
    makeVirtualButton("RightHard", hardThreshold, Direction::Right);
    makeVirtualButton("DownHard", hardThreshold, Direction::Down);
    makeVirtualButton("LeftHard", hardThreshold, Direction::Left);
}

void Stick::calibrate(int controllerId)
{

}
