#include "stdafx.h"
#include "controller.h"
#include "binding_enums.h"


int main()
{
    Controller primary;
    primary.connectToFirstAvaialblePort();
    primary.disconnect();

	return EXIT_SUCCESS;
}

