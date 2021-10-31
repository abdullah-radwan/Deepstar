// ==============================================================
//                 ORBITER MODULE: Deepstar Base Depot
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// BaseDepot.h
// Class interface for Deepstar Base Depot class
//
// ==============================================================

#pragma once
#include "../Common.h"

class BaseDepot : public VESSEL4
{
public:
	BaseDepot(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel) {}
	void DefineEngines();

	void clbkLoadStateEx(FILEHANDLE scn, void* status);
	void clbkSaveState(FILEHANDLE scn);

	bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);
	void clbkPostStep(double simt, double simdt, double mjd);
	int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate);

	struct
	{
		double fuelMass;
		System* systems;
		int systemCount;
	} depotData;

	char buffer[256];
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DD_ISP = 1e5;      // fuel-specific impulse [m/s]
const double  DD_MAXRCSTH = 1e7; // max RCS engine thrust [n]