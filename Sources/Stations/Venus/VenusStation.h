// ==============================================================
//                 ORBITER MODULE: Deepstar Venus Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// VenusStation.h
// Class interface for Deepstar Venus Station vessel class
//
// ==============================================================

#pragma once
#include "../BaseStation.h"

class VenusStation : public BaseStation
{
public:
	VenusStation(OBJHANDLE hVessel, int flightmodel) : BaseStation(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[3];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DV_SIZE = 150;                            // mean radius [m]
const VECTOR3 DV_CS = { 27513.81, 17884.19, 4307.84 }; // x,y,z cross sections [m^2]
const VECTOR3 DV_PMI = { 9761.33, 9767.10, 2297.08 };  // principal moments of inertia (mass-normalised) [m^2]
const double  DV_EMPTYMASS = 4.5e5;                    // empty vessel mass [kg]