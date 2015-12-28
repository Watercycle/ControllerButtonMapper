#include "stdafx.h"
#include "button.h"

Button::Button(Controller* controller) : Control(controller)
{
    
}

bool Button::wasPressedLastFrame()
{
    return pressedLastFrame;
}

void Button::update()
{
    pressedLastFrame = isPressed();
}
