// ==============================================================
//                 ORBITER MODULE: Deepstar Depot 1
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Depot1.cpp
// Control module for Deepstar Depot 1 vessel class
//
// ==============================================================

#include "Depot1.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Depot1(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Depot1*>(vessel); }

void Depot1::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DD_SIZE);
	SetEmptyMass(DD_EMPTYMASS);
	SetPMI(DD_PMI);
	SetCrossSections(DD_CS);

	EnableTransponder(true);
	SetTransponderChannel(400);

	DOCKHANDLE dockPorts[2];

	dockPorts[0] = CreateDock({ 0, 0, 16.307 }, { 0, 0, 1 },  { 0, 1, 0 });
	dockPorts[1] = CreateDock({ 0, 0, -16.307 }, { 0, 0, -1 }, { 0, 1, 0 });

	for (int dockPort = 0; dockPort < 2; dockPort++) 
	{
		EnableIDS(dockPorts[dockPort], true);
		SetIDSChannel(dockPorts[dockPort], 401 + dockPort);
	}

	DefineEngines();
	DefineSystems();
	depotData = { DD_FUELMASS, systems, 3 };
	
	AddMesh("Deepstar/Depot1");
	SetCameraOffset({ 0, 7.5, 2 });
}

void Depot1::DefineSystems()
{
	// Port dish
	systems[0].infoName = "Port dish";
	systems[0].scnName = "PORT_DISH";
	
	systems[0].id = CreateAnimation(0);
	systems[0].status = SystemStatus::CLOSED;
	systems[0].proc = 0;
	systems[0].speed = 0.05;

	static UINT PortGrp[2] = { 6,7 };
	static MGROUP_ROTATE PortRot(0, PortGrp, 2, _V(-16.316, 0, 0), _V(0, 1, 0), (float)(2 * PI));
	AddAnimationComponent(systems[0].id, 0, 1, &PortRot);

	// Starboard dish
	systems[1].infoName = "Starboard dish";
	systems[1].scnName = "STRB_DISH";

	systems[1].id = CreateAnimation(0);
	systems[1].status = SystemStatus::CLOSED;
	systems[1].proc = 0;
	systems[1].speed = 0.05;

	static UINT StrbGrp[2] = { 8,9 };
	static MGROUP_ROTATE StrbRot(0, StrbGrp, 2, _V(16.316, 0, 0), _V(0, 1, 0), (float)(2 * PI));
	AddAnimationComponent(systems[1].id, 0, 1, &StrbRot);

	// Solar panels
	systems[2].infoName = "Solar panels";
	systems[2].scnName = "SOLAR_PANELS";

	systems[2].id = CreateAnimation(0);
	systems[2].status = 0;
	systems[2].proc = 0;
	systems[2].speed = 0.0167;

	static UINT SolarGrp[1] = { 4 };
	static MGROUP_ROTATE SolarRot(0, SolarGrp, 1, _V(0, 0, 0), _V(1, 0, 0), (float)(2 * PI));
	AddAnimationComponent(systems[2].id, 0, 1, &SolarRot);
}