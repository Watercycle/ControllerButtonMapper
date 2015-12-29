#pragma once
#include "control.h"
#include <unordered_map>
#include "binding_enums.h"

class VirtualButton;

class Axis : public Control
{
protected:
    const AxisType name;

public:
    Axis(Controller* controller, const AxisType& nameId);
    virtual void updateSettings(unordered_map<SettingType, string>& settings) = 0;
    virtual float getThreshold() = 0;
    virtual bool inRegion(const Direction& targetRegion) const;
    void makeVirtualButton(const string& type, float threshold, const Direction& region);
    virtual void setupVirtualButtons() = 0;
};