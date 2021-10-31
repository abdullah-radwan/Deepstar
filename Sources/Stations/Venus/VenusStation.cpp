// ==============================================================
//                 ORBITER MODULE: Deepstar Venus Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// VenusStation.cpp
// Control module for Deepstar Venus Station vessel class
//
// ==============================================================

#include "VenusStation.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new VenusStation(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<VenusStation*>(vessel); }

void VenusStation::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DV_SIZE);
	SetEmptyMass(DV_EMPTYMASS);
	SetPMI(DV_PMI);
	SetCrossSections(DV_CS);

	EnableTransponder(true);
	SetTransponderChannel(500);

	DOCKHANDLE dockPorts[2];

	dockPorts[0] = CreateDock({ 0, 0, 33.943 }, { 0, 0, 1 },  { 0, 1, 0 });
	dockPorts[1] = CreateDock({ 0, 0, -33.943 }, { 0, 0, -1 }, { 0, 1, 0 });

	for (int dockPort = 0; dockPort < 2; dockPort++) 
	{
		EnableIDS(dockPorts[dockPort], true);
		SetIDSChannel(dockPorts[dockPort], 501 + dockPort);
	}

	DefineEngines();
	DefineSystems();
	stationData = { systems, 3, false, true };

	AddMesh("Deepstar/VenusStation");
	SetCameraOffset({ 0, 5, 14 });
}

void VenusStation::DefineSystems()
{
	// Forward dish
	systems[0].infoName = "Forward dish";
	systems[0].scnName = "FWD_DISH";

	systems[0].id = CreateAnimation(0);
	systems[0].status = SystemStatus::CLOSED;
	systems[0].proc = 0;
	systems[0].speed = 0.05;

	static UINT FwdGrp[2] = { 12,13 };
	static MGROUP_ROTATE FwdRot(0, FwdGrp, 2, _V(0, -28.973, 0), _V(1, 0, 0), (float)(2 * PI));
	AddAnimationComponent(systems[0].id, 0, 1, &FwdRot);

	// Aft dish
	systems[1].infoName = "Aft dish";
	systems[1].scnName = "AFT_DISH";

	systems[1].id = CreateAnimation(0);
	systems[1].status = SystemStatus::CLOSED;
	systems[1].proc = 0;
	systems[1].speed = 0.05;

	static UINT AftGrp[2] = { 14,15 };
	static MGROUP_ROTATE AftRot(0, AftGrp, 2, _V(0, -28.973, 0), _V(1, 0, 0), (float)(2 * PI));
	AddAnimationComponent(systems[1].id, 0, 1, &AftRot);

	// Solar panels
	systems[2].infoName = "Solar panels";
	systems[2].scnName = "SOLAR_PANELS";

	systems[2].id = CreateAnimation(0);
	systems[2].status = SystemStatus::CLOSED;
	systems[2].proc = 0;
	systems[2].speed = 0.0167;

	static UINT SolarGrp[2] = { 8, 11 };
	static MGROUP_ROTATE SolarRot(0, SolarGrp, 2, _V(0, 28.401, 0), _V(1, 0, 0), (float)(-2 * PI));
	AddAnimationComponent(systems[2].id, 0, 1, &SolarRot);
}