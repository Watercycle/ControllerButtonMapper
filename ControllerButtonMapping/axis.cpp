#include "stdafx.h"
#include "axis.h"
#include "binding_enums.h"
#include "virtual_button.h"
#include "controller.h"

Axis::Axis(Controller* controller, const AxisType& name) : Control(controller), name(name)
{
        
}

bool Axis::inRegion(const Direction& targetRegion) const
{
    return true;
}

void Axis::makeVirtualButton(const string& type, float threshold, const Direction& region)
{
    const string enumName = static_cast<string>(name) + type;
    controller->buttons.insert({ ButtonType::from(enumName),
        make_shared<VirtualButton>(controller, [=]() { 
        return (getThreshold() > threshold && inRegion(region)); 
    })});
}