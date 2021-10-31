// ==============================================================
//                 ORBITER MODULE: Deepstar
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Deepstar.h
// Class interface for Deepstar vessel class
//
// ==============================================================

#pragma once
#include <UCSO/Vessel.h>
#include <XRSound.h>

#include "../Common.h"

class Deepstar : public VESSEL4
{
public:
	Deepstar(OBJHANDLE hVessel, int flightmodel);
	~Deepstar();

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
	static const int mfdUpdateLineup[4];

	MESHHANDLE vcMesh;
	XRSound* xrSound;
	DOCKHANDLE noseDock = nullptr;
	SpotLight* dockLight = nullptr;
	THRUSTER_HANDLE rcsHandles[14];
	char buffer[256];
	
	int hudMode = HUDMode::DEFAULT;
	bool rcsConfig = false;

	MainFuel mainFuel;
	struct DSSystem : System { bool repeat; } systems[4]; // 0: hab, 1: Port dish CW, 2: Port dish CCW, 3: Starboard dish CW, 4: Starboard dish CCW, 5: Nose docking port

	UCSO::Vessel* ucso;
	struct
	{
		int sideIndex = 0;    // 0: starboard, 1: port
		int sectionIndex = 0; // 0-7: cargo sections, 8: probe slot
		int slotIndex = 0;    // 0-8: selected slot in section

		int selectedSlot = 2; // 0: probe starboard, 1: probe port, 2-145: cargo slots
		int selectedCargo = 0;   // Selected UCSO cargo to add

		bool displayInfo = false;
		const char* message = nullptr;
		double timer = 0;
	} cargoData;

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
const double  DS_SIZE = 230;                           // mean radius [m]
const VECTOR3 DS_CS = { 10978.34, 10094.56, 2392.19 }; // x,y,z cross sections [m^2]
const VECTOR3 DS_PMI = { 15570.58, 15510.20, 466.25 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DS_EMPTYMASS = 2e6;                      // empty vessel mass [kg]
const double  DS_FUELMASS = 12e6;                      // max fuel mass [kg]
const double  DS_ISP = 1e5;                            // fuel-specific impulse [m/s]
const double  DS_MAXMAINTH = 4e7;					   // max main engine thrust [n]
const double  DS_MAXRCSTH = 2e7;                       // max RCS engine thrust [n]

namespace SystemID
{
	enum
	{
		PORT_DISH,
		STRB_DISH,		
		SPIN_GRAVITY,
		NOSE_DOCK_DOORS
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