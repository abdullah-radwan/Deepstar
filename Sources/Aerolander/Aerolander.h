// ==============================================================
//                 ORBITER MODULE: Deepstar Aerolander
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Aerolander.h
// Class interface for Deepstar Aerolander vessel class
//
// ==============================================================

#pragma once
#include <UCSO/Vessel.h>
#include <XRSound.h>

#include "../Common.h"

class Aerolander : public VESSEL4
{
public:
	Aerolander(OBJHANDLE hVessel, int flightmodel);
	~Aerolander();

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
	THRUSTER_HANDLE rcsHandles[17];
	char buffer[256];

	int hudMode = HUDMode::DEFAULT;
	bool rcsConfig = false, gearDown = false;

	MainFuel mainFuel;
	System systems[5]; // 0: payload, 1: gear, 2: hover, 3: hatch, 4: airbrake

	struct
	{
		THRUSTER_HANDLE handle;
		THGROUP_HANDLE groupHandle = nullptr;
	} hoverEngine;

	struct
	{
		DOCKHANDLE handle = nullptr;
		DWORD idsChannel;
	} dockPort;

	UCSO::Vessel* ucso;
	CargoData cargoData;

	void DefineEngines();
	void DefineSystems();
	void DefineSlots();

	void DrawInfoHUD(int x, int y, oapi::Sketchpad* skp);
	void DrawCargoHUD(int x, int y, oapi::Sketchpad* skp);

	void SetRCSConfig();
	bool SlotAvailable();
	static void VLiftCoeff(VESSEL* v, double aoa, double M, double Re, void* context, double* cl, double* cm, double* cd);
	static void HLiftCoeff(VESSEL* v, double beta, double M, double Re, void* context, double* cl, double* cm, double* cd);
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DA_SIZE = 20;                      // mean radius [m]
const VECTOR3 DA_CS = { 173.98, 483.71, 70.35 }; // x,y,z cross sections [m^2]
const VECTOR3 DA_PMI = { 67.39, 80.81, 18.58 };  // principal moments of inertia (mass-normalised) [m^2]
const VECTOR3 DA_RD = { 0.10, 0.13, 0.04 };      // rotation drag coefficients
const double  DA_EMPTYMASS = 26e3;               // empty vessel mass [kg]
const double  DA_FUELMASS = 3e4;                 // max fuel mass [kg]
const double  DA_ISP = 9e3;                      // fuel-specific impulse [m/s]
const double  DA_MAXMAINTH = 2e5;                // max main engine thrust [n]
const double  DA_MAXHVRTH = 2.5e5;				 // max hover engine thrust [n]
const double  DA_MAXRCSTH = 5e3;                 // max RCS engine thrust [n]

namespace SystemID
{
	enum
	{
		LANDING_GEAR,
		AIRBRAKE,
		PAYLOAD_BAY,
		HOVER_DOORS,
		EGRESS_HATCH
	};
}

namespace SoundID
{
	enum
	{
		HYDRAULICS,
		CLICK,
		RCS_NORMAL,
		RCS_DOCKING,

		GEAR_WHINE,
		GEAR_DOWN_LOCKED,
		GEAR_DOWN,
		GEAR_UP_LOCKED,
		GEAR_UP,
		HOVER_DOORS_CLOSED,
		HATCH_OPEN,
		HATCH_CLOSED,

		MAIN_FULL,
		MAIN_LOW,
		MAIN_DEPLETED,
		WARN_GEAR_UP,

		BAY_DOORS_CLOSED,
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
static const DWORD tdVtxN = 13; 

static TOUCHDOWNVTX tdVtxGearDown[tdVtxN] = 
{
	{ _V(0, -6.7, 6.215),                1e6, 1e5, 1.6, 0.1},
	{ _V(-5, -6.7, -9.949),              1e6, 1e5, 3.0, 0.2},
	{ _V(5,-6.7,-9.949),                 1e6, 1e5, 3.0, 0.2},
	{ _V(-9.55077, -0.844817, -15.1981), 1e7, 1e5, 3.0},
	{ _V(9.5504, -0.845963, -15.1981),   1e7, 1e5, 3.0},
	{ _V(-3.60805, -1.49942, 10.7846),   1e7, 1e5, 3.0},
	{ _V(3.59202, -1.58751, 10.7846),    1e7, 1e5, 3.0},
	{ _V(-11.3144, 3.17974, -14.822),    1e7, 1e5, 3.0},
	{ _V(11.3144, 3.17973, -14.822),     1e7, 1e5, 3.0},
	{ _V(-11.3135, 3.18419, -8.38107),   1e7, 1e5, 3.0},
	{ _V(11.3134, 3.18418,-8.38107),     1e7, 1e5, 3.0},
	{ _V(0, 2.70611, 7.23938),           1e7, 1e5, 3.0},
	{ _V(0, -0.692416, 14.1048),         1e7, 1e5, 3.0}
};

static TOUCHDOWNVTX tdVtxGearUp[tdVtxN] = 
{
	{ _V(0, -3.25487, 6.215),            1e7, 1e5, 3.0, 3},
	{ _V(-5, -2.92676, -9.949),          1e7, 1e5, 3.0, 3},
	{ _V(5, -2.92676, -9.949),           1e7, 1e5, 3.0, 3},
	{ _V(-9.55077, -0.844817, -15.1981), 1e7, 1e5, 3.0},
	{ _V(9.5504, -0.845963, -15.1981),   1e7, 1e5, 3.0},
	{ _V(-3.60805, -1.49942, 10.7846),   1e7, 1e5, 3.0},
	{ _V(3.59202, -1.58751, 10.7846),    1e7, 1e5, 3.0},
	{ _V(-11.3144, 3.17974, -14.822),    1e7, 1e5, 3.0},
	{ _V(11.3144, 3.17973, -14.822),     1e7, 1e5, 3.0},
	{ _V(-11.3135, 3.18419, -8.38107),   1e7, 1e5, 3.0},
	{ _V(11.3134, 3.18418,-8.38107),     1e7, 1e5, 3.0},
	{ _V(0, 2.70611, 7.23938),           1e7, 1e5, 3.0},
	{ _V(0, -0.692416, 14.1048),         1e7, 1e5, 3.0}
};