// ==============================================================
//                 ORBITER MODULE: Deepstar Aerolander
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Lander.h
// Class interface for Deepstar Lander vessel class
//
// ==============================================================

#pragma once
#include <UCSO/Vessel.h>
#include <XRSound.h>

#include "../Common.h"

class Lander : public VESSEL4
{
public:
	Lander(OBJHANDLE hVessel, int flightmodel);
	~Lander();

	void clbkSetClassCaps(FILEHANDLE cfg);

	void clbkLoadStateEx(FILEHANDLE scn, void* status);
	void clbkSaveState(FILEHANDLE scn);

	bool clbkLoadVC(int id);
	void clbkPostCreation();

	void clbkMFDMode(int mfd, int mode);
	void clbkHUDMode(int mode);
	void clbkRCSMode(int mode);
	void clbkNavMode(int mode, bool active);

	bool clbkVCRedrawEvent(int id, int ev, SURFHANDLE surf);
	bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);
	void clbkPostStep(double simt, double simdt, double mjd);

	int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate);
	bool clbkVCMouseEvent(int id, int ev, VECTOR3& p);

private:
	static int vesselCount;
	static oapi::Font* mfdFont;
	static oapi::Font* systemFont;

	MESHHANDLE vcMesh;
	XRSound* xrSound;
	THRUSTER_HANDLE rcsHandles[20];
	char buffer[256];

	int hudMode = HUDMode::DEFAULT;
	bool rcsConfig = false;	

	MainFuel mainFuel;
	System hatchSystem;

	UCSO::Vessel* ucso;	
	CargoData cargoData;

	void DefineEngines();
	void DefineSystems();
	void DefineSlots();

	void DrawInfoHUD(int x, int y, oapi::Sketchpad* skp);
	void DrawCargoHUD(int x, int y, oapi::Sketchpad* skp);

	void SetRCSConfig();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DL_SIZE = 15;                      // mean radius [m]
const VECTOR3 DL_CS = { 70.96, 82.94, 50.12 };   // x,y,z cross sections [m^2]
const VECTOR3 DL_PMI = { 15.40, 10.17, 12.61 };  // principal moments of inertia (mass-normalised) [m^2]
const double  DL_EMPTYMASS = 19e3;               // empty vessel mass [kg]
const double  DL_FUELMASS = 3e4;                 // max fuel mass [kg]
const double  DL_ISP = 8800;                     // fuel-specific impulse [m/s]
const double  DL_MAXHVRTH = 2e5;                 // max hover engine thrust [n]
const double  DL_MAXRCSTH = 3500;                // max RCS engine thrust [n]

namespace SoundID
{
	enum
	{
		HYDRAULICS,
		CLICK,
		HATCH_OPEN,
		HATCH_CLOSED,
		RCS_NORMAL,
		RCS_DOCKING,

		MAIN_FULL,
		MAIN_LOW,
		MAIN_DEPLETED,

		SLOT_EMPTY,
		SLOT_OCCUPIED,
		CARGO_RELEASED,
		CARGO_RELEASE_FAILED,
		CARGO_GRAPPLED,
		CARGO_GRAPPLE_NORANGE,
		CARGO_GRAPPLE_FAILED
	};
}

// Touchdown points
static const DWORD tdVtxN = 12;
static TOUCHDOWNVTX tdVtx[tdVtxN] =
{
	{ _V(-0.000035, -8.35928, -4.935),  4.8e5, 2.8e5, 3.0, 3},
	{ _V(4.92593, -8.35928, 4.93468),   4.8e5, 2.8e5, 3.0, 3},
	{ _V(-4.926, -8.35928, 4.93468),    4.8e5, 2.8e5, 3.0, 3},
	{ _V(2.85932, -2.172, 3.82499),     4.8e5, 2.8e5, 3.0},
	{ _V(-2.86004, -2.172, 3.82499),    4.8e5, 2.8e5, 3.0},
	{ _V(2.9537, -2.17155, -5.62383),   4.8e5, 2.8e5, 3.0},
	{ _V(-2.95299, -2.17155, -5.62383), 4.8e5, 2.8e5, 3.0},
	{ _V(1.01806, 1.90513, 4.0048),     4.8e5, 2.8e5, 3.0},
	{ _V(-1.01806, 1.90513, 4.0048),    4.8e5, 2.8e5, 3.0},
	{ _V(1.01806, 1.90513, -3.91307),   4.8e5, 2.8e5, 3.0},
	{ _V(-1.01806, 1.90513, -3.91307),  4.8e5, 2.8e5, 3.0},
	{ _V(0, 2.69699, 0),                4.8e5, 2.8e5, 3.0}
};