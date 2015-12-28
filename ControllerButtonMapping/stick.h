#pragma once
#include <unordered_map>
#include "axis.h"

class Stick : public Axis
{
public:
    sf::Joystick::Axis leftRight;
    sf::Joystick::Axis upDown;
    vec2 state;
    float sensitivity; // we're going to scale the state

    float radius; // e.g. for xbox the controller stick's state range from -100 to 100
    float deadzone; // how much of the controller is a deadzone? 0 to 100%
    float defaultSensitivity;
    bool usingAcceleration;

    float softThreshold;
    float hardThreshold;

    Stick(sf::Joystick::Axis leftRight, sf::Joystick::Axis upDown, Controller* controller, const AxisType& name);

    void update() override;
    float getThreshold() override;
    bool Stick::inRegion(const Direction& targetRegion) const override;
    void updateSettings(unordered_map<string, string>& settings) override;
    void setupVirtualButtons() override;


private:
    void calibrate(int controllerId);
};
