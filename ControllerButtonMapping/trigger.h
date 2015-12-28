#pragma once
#include <unordered_map>
#include "axis.h"

class Trigger : public Axis
{
public:
    float state = 0;
    sf::Joystick::Axis button;

    float softThreshold;
    float hardThreshold;

public:
    Trigger(sf::Joystick::Axis button, Controller* controller, const AxisType& name);
    void update() override;
    void updateSettings(unordered_map<string, string>& settings) override;
    void setupVirtualButtons() override;
    float getThreshold() override;
};
