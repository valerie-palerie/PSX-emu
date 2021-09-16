#include <iostream>
#include "Playstation.h"

int main()
{
	Playstation psx;
	psx.Init();
	do
	{
		//Ticking and stuff is to be implemented later to make sure the hardware runs at the correct speed.
		psx.Tick(0.0);
	} while (1);
}