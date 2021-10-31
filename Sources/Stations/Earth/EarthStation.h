// ==============================================================
//                 ORBITER MODULE: Deepstar Earth Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// EarthStation.h
// Class interface for Deepstar Earth Station vessel class
//
// ==============================================================

#pragma once
#include "../../Common.h"

class EarthStation : public VESSEL4
{
public:
	EarthStation(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

	void clbkLoadStateEx(FILEHANDLE scn, void* status);
	void clbkSaveState(FILEHANDLE scn);

	bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);
	void clbkPostStep(double simt, double simdt, double mjd);
	int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate);

private:
	System systems[2];

	char buffer[256];

	void DefineEngines();
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DE_SIZE = 360;                           // mean radius [m]
const VECTOR3 DE_CS = { 13053.62, 9272.04, 5033.62 };  // x,y,z cross sections [m^2]
const VECTOR3 DE_PMI = { 13684.09, 9724.04, 5033.62 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DE_EMPTYMASS = 1.5e6;                    // empty vessel mass [kg]
const double  DE_FUELMASS = 4e6;                       // max fuel mass [kg]
const double  DE_ISP = 2e5;                            // fuel-specific impulse [m/s]
const double  DE_MAXRCSTH = 1e7;                       // max RCS engine thrust [n]