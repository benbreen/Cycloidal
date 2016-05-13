#pragma once

#include "Types.h"

class Wheel
{
public:
	Wheel();
	~Wheel();



private:
	Point _center;

	// nw
	int _numTeeth;

	// D
	double _pitchCircleDiameter;

	// r
	double _pitchCircleRadius;

	// m
	double _module;

	// cp
	double _circularPitch;

	// R
	double _gearRatio;
};

