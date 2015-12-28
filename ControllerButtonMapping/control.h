#pragma once

class Controller;

class Control
{
protected:
    Controller* controller;

public:
	virtual ~Control()
	{
	}

	Control(Controller* controller);

    virtual void update() = 0;
};
