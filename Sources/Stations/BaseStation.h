// ==============================================================
//                 ORBITER MODULE: Deepstar Base Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// BaseStation.h
// Class interface for Deepstar Base Station class
//
// ==============================================================

#pragma once
#include "../Common.h"

class BaseStation : public VESSEL4
{
public:
	BaseStation(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel) {}
	void DefineEngines();

	void clbkLoadStateEx(FILEHANDLE scn, void* status);
	void clbkSaveState(FILEHANDLE scn);

	bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);
	void clbkPostStep(double simt, double simdt, double mjd);
	int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate);

	struct
	{
		System* systems;
		int systemCount;
		bool hasSpin, hasSolar;
	} stationData;

	char buffer[256];
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DB_FUELMASS = 2e4; // max fuel mass [kg]
const double  DB_ISP = 1e5;      // fuel-specific impulse [m/s]
const double  DB_MAXRCSTH = 3e5; // max RCS engine thrust [n]