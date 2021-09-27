#include <iostream>
#include <chrono>
#include "Playstation.h"

int main()
{
	Playstation psx;
	psx.Init();
	auto refTime = std::chrono::steady_clock::now();
	do
	{
		auto currentTime = std::chrono::steady_clock::now();
		double deltaT = std::chrono::duration<double>(currentTime - refTime).count();
		refTime = currentTime;
		psx.Tick(deltaT);
	} while (1);
}