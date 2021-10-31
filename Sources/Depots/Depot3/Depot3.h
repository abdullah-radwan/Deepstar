// ==============================================================
//                 ORBITER MODULE: Deepstar Depot 3
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Depot3.h
// Class interface for Deepstar Depot 3 vessel class
//
// ==============================================================

#pragma once
#include "../BaseDepot.h"

class Depot3 : public BaseDepot
{
public:
	Depot3(OBJHANDLE hVessel, int flightmodel) : BaseDepot(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[2];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DD_SIZE = 260;                         // mean radius [m]
const VECTOR3 DD_CS = { 6013.66, 1630.52, 6167.99 }; // x,y,z cross sections [m^2]
const VECTOR3 DD_PMI = { 1082.98, 314.43, 1084.11 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DD_EMPTYMASS = 7e5;                    // empty vessel mass [kg]
const double  DD_FUELMASS = 9.6e6;                   // max fuel mass [kg]