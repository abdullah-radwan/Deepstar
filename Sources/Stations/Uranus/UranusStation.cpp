// ==============================================================
//                 ORBITER MODULE: Deepstar Uranus Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// UranusStation.cpp
// Control module for Deepstar Uranus Station vessel class
//
// ==============================================================

#include "UranusStation.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new UranusStation(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<UranusStation*>(vessel); }

void UranusStation::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DB_SIZE);
	SetEmptyMass(DB_EMPTYMASS);
	SetPMI(DB_PMI);
	SetCrossSections(DB_CS);

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
	stationData = { systems, 3, true, false };

	AddMesh("Deepstar/UranusStation");
	SetCameraOffset({ 0, 5, 14 });
}

void UranusStation::DefineSystems()
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
}