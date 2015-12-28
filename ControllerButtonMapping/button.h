#pragma once
#include "control.h"

class Controller;

class Button : public Control
{
public:
    virtual ~Button()
    {
    }

    Button(Controller* controller);

    virtual bool wasPressedLastFrame();
    virtual bool isPressed() = 0;
    void update() override;

protected:
    abool pressedLastFrame = false;
};