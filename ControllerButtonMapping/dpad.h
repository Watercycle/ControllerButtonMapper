#pragma once
#include <unordered_map>
#include "axis.h"

class DPad : public Axis
{
public:
    sf::Joystick::Axis leftRight;
    sf::Joystick::Axis upDown;
    vec2 state;

    float threshold;

public:
    DPad(sf::Joystick::Axis leftRight, sf::Joystick::Axis upDown, Controller* controller, const AxisType& name);

    void update() override;
    bool inRegion(const Direction& targetRegion) const override;
    float getThreshold() override;
    void updateSettings(unordered_map<SettingType, string>& settings) override;
    void setupVirtualButtons() override;
}; 