// ==============================================================
//                 ORBITER MODULE: Deepstar Depot 1
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Depot1.h
// Class interface for Deepstar Depot 1 vessel class
//
// ==============================================================

#pragma once
#include "../BaseDepot.h"

class Depot1 : public BaseDepot
{
public:
	Depot1(OBJHANDLE hVessel, int flightmodel) : BaseDepot(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[3];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DD_SIZE = 240;                         // mean radius [m]
const VECTOR3 DD_CS = { 4677.16, 516.12, 9014.29 };  // x,y,z cross sections [m^2]
const VECTOR3 DD_PMI = { 4814.40, 118.19, 4873.66 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DD_EMPTYMASS = 8e5;                    // empty vessel mass [kg]
const double  DD_FUELMASS = 4.8e6;                   // max fuel mass [kg]            