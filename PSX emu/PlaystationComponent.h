#pragma once

class Playstation;

class PlaystationComponent
{
protected:
	Playstation* _playstation;

public:
	Playstation* playstation() const { return _playstation; }

	virtual void Init() = 0;
	virtual void Tick(double deltaT) = 0;

	PlaystationComponent(Playstation* playstation)
		: _playstation(playstation)
	{
	}
};