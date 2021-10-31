// ==============================================================
//                 ORBITER MODULE: Deepstar Mars Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// MarsStation.cpp
// Control module for Deepstar Mars Station vessel class
//
// ==============================================================

#include "MarsStation.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new MarsStation(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<MarsStation*>(vessel); }

void MarsStation::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DM_SIZE);
	SetEmptyMass(DM_EMPTYMASS);
	SetPMI(DM_PMI);
	SetCrossSections(DM_CS);

	EnableTransponder(true);
	SetTransponderChannel(500);

	DOCKHANDLE dockPorts[2];

	dockPorts[0] = CreateDock({ 0, 0, 31.061 }, { 0, 0, 1 },  { 0, 1, 0 });
	dockPorts[1] = CreateDock({ 0, 0, -31.061 }, { 0, 0, -1 }, { 0, 1, 0 });

	for (int dockPort = 0; dockPort < 2; dockPort++) 
	{
		EnableIDS(dockPorts[dockPort], true);
		SetIDSChannel(dockPorts[dockPort], 501 + dockPort);
	}

	DefineEngines();
	DefineSystems();
	stationData = { systems, 4, true, true };

	AddMesh("Deepstar/MarsStation");
	SetCameraOffset({ 0, 5, 14 });
}

void MarsStation::DefineSystems()
{
	// Forward dish
	systems[0].infoName = "Forward dish";
	systems[0].scnName = "FWD_DISH";

	systems[0].id = CreateAnimation(0);
	systems[0].status = SystemStatus::CLOSED;
	systems[0].proc = 0;
	systems[0].speed = 0.05;

	static UINT FwdGrp[2] = { 7,8 };
	static MGROUP_ROTATE FwdRot(0, FwdGrp, 2, _V(0.004, 0, 11.705), _V(0, 1, 0), (float)(2 * PI));
	AddAnimationComponent(systems[0].id, 0, 1, &FwdRot);

	// Aft dish
	systems[1].infoName = "Aft dish";
	systems[1].scnName = "AFT_DISH";

	systems[1].id = CreateAnimation(0);
	systems[1].status = SystemStatus::CLOSED;
	systems[1].proc = 0;
	systems[1].speed = 0.05;

	static UINT AftGrp[2] = { 9,10 };
	static MGROUP_ROTATE AftRot(0, AftGrp, 2, _V(0.004, 0, -11.705), _V(0, 1, 0), (float)(2 * PI));
	AddAnimationComponent(systems[1].id, 0, 1, &AftRot);

	// Spin gravity
	systems[2].infoName = "Spin gravity";
	systems[2].scnName = "SPIN_GRAVITY";

	systems[2].id = CreateAnimation(0);
	systems[2].status = SystemStatus::CLOSED;
	systems[2].proc = 0;
	systems[2].speed = 0.0331;

	static UINT SpinGrp[3] = { 0,1,2 };
	static MGROUP_ROTATE SpinRot(0, SpinGrp, 3, _V(0, 0, 0), _V(0, 0, 1), (float)(2 * PI));
	AddAnimationComponent(systems[2].id, 0, 1, &SpinRot);

	// Solar panels
	systems[3].infoName = "Solar panels";
	systems[3].scnName = "SOLAR_PANELS";

	systems[3].id = CreateAnimation(0);
	systems[3].status = SystemStatus::CLOSED;
	systems[3].proc = 0;
	systems[3].speed = 0.0167;

	static UINT Solar1Grp[1] = { 14 };
	static MGROUP_ROTATE SolarRot1(0, Solar1Grp, 1, _V(0, 0, 22.852), _V(1, 0, 0), (float)(2 * PI));
	AddAnimationComponent(systems[3].id, 0, 1, &SolarRot1);

	static UINT Solar2Grp[1] = { 13 };
	static MGROUP_ROTATE SolarRot2(0, Solar2Grp, 1, _V(0, 0, -22.852), _V(1, 0, 0), (float)(2 * PI));
	AddAnimationComponent(systems[3].id, 0, 1, &SolarRot2);
}