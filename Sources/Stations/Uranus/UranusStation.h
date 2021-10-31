// ==============================================================
//                 ORBITER MODULE: Deepstar Uranus Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// UranusStation.h
// Class interface for Deepstar Uranus Station vessel class
//
// ==============================================================

#pragma once
#include "../BaseStation.h"

class UranusStation : public BaseStation
{
public:
	UranusStation(OBJHANDLE hVessel, int flightmodel) : BaseStation(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	System systems[3];
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DB_SIZE = 150;                         // mean radius [m]
const VECTOR3 DB_CS = { 471.92, 831.11, 609.41 };    // x,y,z cross sections [m^2]
const VECTOR3 DB_PMI = { 150.46, 1413.49, 1322.69 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DB_EMPTYMASS = 2.4e5;                  // empty vessel mass [kg]