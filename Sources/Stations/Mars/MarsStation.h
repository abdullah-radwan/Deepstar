// ==============================================================
//                 ORBITER MODULE: Deepstar Mars Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// MarsStation.h
// Class interface for Deepstar Mars Station vessel class
//
// ==============================================================

#pragma once
#include "../BaseStation.h"

class MarsStation : public BaseStation
{
public:
	MarsStation(OBJHANDLE hVessel, int flightmodel) : BaseStation(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[4];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DM_SIZE = 150;                         // mean radius [m]
const VECTOR3 DM_CS = { 411.31, 1891.46, 585.90 };   // x,y,z cross sections [m^2]
const VECTOR3 DM_PMI = { 141.18, 1439.10, 1311.87 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DM_EMPTYMASS = 2.5e5;                  // empty vessel mass [kg]