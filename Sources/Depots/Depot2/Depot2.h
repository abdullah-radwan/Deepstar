// ==============================================================
//                 ORBITER MODULE: Deepstar Depot 2
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Depot2.h
// Class interface for Deepstar Depot 2 vessel class
//
// ==============================================================

#pragma once
#include "../BaseDepot.h"

class Depot2 : public BaseDepot
{
public:
	Depot2(OBJHANDLE hVessel, int flightmodel) : BaseDepot(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[3];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DD_SIZE = 200;                          // mean radius [m]
const VECTOR3 DD_CS = { 5978.90, 1673.28, 10213.54 }; // x,y,z cross sections [m^2]
const VECTOR3 DD_PMI = { 1125.72, 340.60, 1154.74 };  // principal moments of inertia (mass-normalised) [m^2]
const double  DD_EMPTYMASS = 8e5;                     // empty vessel mass [kg]
const double  DD_FUELMASS = 9.6e6;                    // max fuel mass [kg]