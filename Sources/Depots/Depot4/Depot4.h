// ==============================================================
//                 ORBITER MODULE: Deepstar Depot 4
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Depot4.h
// Class interface for Deepstar Depot 4 vessel class
//
// ==============================================================

#pragma once
#include "../BaseDepot.h"

class Depot4 : public BaseDepot
{
public:
	Depot4(OBJHANDLE hVessel, int flightmodel) : BaseDepot(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[2];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DD_SIZE = 300;                        // mean radius [m]
const VECTOR3 DD_CS = { 4704.46, 463.42, 4933.38 }; // x,y,z cross sections [m^2]
const VECTOR3 DD_PMI = { 4905.83, 60.17, 4905.74 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DD_EMPTYMASS = 7e5;                   // empty vessel mass [kg]
const double  DD_FUELMASS = 4.8e6;                  // max fuel mass [kg]