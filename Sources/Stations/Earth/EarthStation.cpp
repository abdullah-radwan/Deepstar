// ==============================================================
//                 ORBITER MODULE: Deepstar Earth Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// EarthStation.cpp
// Control module for Deepstar Earth Station vessel class
//
// ==============================================================

#include "EarthStation.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new EarthStation(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<EarthStation*>(vessel); }

void EarthStation::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DE_SIZE);
	SetEmptyMass(DE_EMPTYMASS);
	SetPMI(DE_PMI);
	SetCrossSections(DE_CS);

	EnableTransponder(true);
	SetTransponderChannel(510);

	DOCKHANDLE dockPorts[5];

	dockPorts[0] = CreateDock({ -64.351, 0, -155.600 }, { 0, 0, 1 },  { 0, 1, 0 });
	dockPorts[1] = CreateDock({ -64.351, 0, -174.988 }, { 0, 0, -1 }, { 0, 1, 0 });
	dockPorts[2] = CreateDock({ -86.481, 0, -165.293 }, { -1, 0, 0 }, { 0, 1, 0 });
	dockPorts[3] = CreateDock({ -42.293, 0, -174.988 }, { 0, 0, -1 }, { 0, 1, 0 });
	dockPorts[4] = CreateDock({ -20.234, 0, -174.988 }, { 0, 0, -1 }, { 0, 1, 0 });

	for (int dockPort = 0; dockPort < 5; dockPort++) 
	{
		EnableIDS(dockPorts[dockPort], true);
		SetIDSChannel(dockPorts[dockPort], 511 + dockPort);
	}

	DefineEngines();
	DefineSystems();

	AddMesh("Deepstar/EarthStation");
	SetCameraOffset({ -64.351, 9.72, -165.293 });
}

void EarthStation::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DE_FUELMASS);

	THRUSTER_HANDLE rcsHandles[14], groupHandles[4];

	rcsHandles[0] = CreateThruster(_V(-6, 0.7, -0.4), _V(0, -1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[1] = CreateThruster(_V(6, -0.7, -0.4), _V(0, 1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[2] = CreateThruster(_V(-6, -0.7, -0.4), _V(0, 1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[3] = CreateThruster(_V(6, 0.7, -0.4), _V(0, -1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[4] = CreateThruster(_V(0, 0, 15), _V(0, 1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[5] = CreateThruster(_V(0, 0, -15), _V(0, -1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[6] = CreateThruster(_V(0, 0, 15), _V(0, -1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[7] = CreateThruster(_V(0, 0, -15), _V(0, 1, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[8] = CreateThruster(_V(0, 0, 15), _V(1, 0, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[9] = CreateThruster(_V(0, 0, -15), _V(-1, 0, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[10] = CreateThruster(_V(0, 0, 15), _V(-1, 0, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[11] = CreateThruster(_V(0, 0, -15), _V(1, 0, 0), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[12] = CreateThruster(_V(0, 0, 0), _V(0, 0, -1), DE_MAXRCSTH, propHandle, DE_ISP);
	rcsHandles[13] = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DE_MAXRCSTH, propHandle, DE_ISP);

	groupHandles[0] = rcsHandles[4];
	groupHandles[1] = rcsHandles[5];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_PITCHUP);

	groupHandles[0] = rcsHandles[6];
	groupHandles[1] = rcsHandles[7];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_PITCHDOWN);

	groupHandles[0] = rcsHandles[0];
	groupHandles[1] = rcsHandles[1];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BANKLEFT);

	groupHandles[0] = rcsHandles[2];
	groupHandles[1] = rcsHandles[3];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BANKRIGHT);

	groupHandles[0] = rcsHandles[4];
	groupHandles[1] = rcsHandles[7];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_UP);

	groupHandles[0] = rcsHandles[5];
	groupHandles[1] = rcsHandles[6];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_DOWN);

	groupHandles[0] = rcsHandles[10];
	groupHandles[1] = rcsHandles[11];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_YAWLEFT);

	groupHandles[0] = rcsHandles[8];
	groupHandles[1] = rcsHandles[9];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_YAWRIGHT);

	groupHandles[0] = rcsHandles[9];
	groupHandles[1] = rcsHandles[10];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_LEFT);

	groupHandles[0] = rcsHandles[8];
	groupHandles[1] = rcsHandles[11];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_RIGHT);

	CreateThrusterGroup(rcsHandles + 13, 1, THGROUP_ATT_FORWARD);
	CreateThrusterGroup(rcsHandles + 12, 1, THGROUP_ATT_BACK);
}

void EarthStation::DefineSystems()
{
	// Fuel boom
	systems[0].scnName = "FUEL_BOOM";
	systems[0].id = CreateAnimation(0);
	systems[0].status = SystemStatus::CLOSED;
	systems[0].proc = 0;
	systems[0].speed = 0.0331;

	static UINT FuelGrp[1] = { 4 };
	static MGROUP_ROTATE FuelRot(0, FuelGrp, 1, _V(-21.422, 35.669, 109.089), _V(0, 1, 0), (float)(PI));
	AddAnimationComponent(systems[0].id, 0, 1, &FuelRot);

	// Solar panels
	systems[1].scnName = "SOLAR_PANELS";
	systems[1].id = CreateAnimation(0);
	systems[1].status = SystemStatus::CLOSED;
	systems[1].proc = 0;
	systems[1].speed = 0.017;

	static UINT Solar1Grp[1] = { 5 };
	static MGROUP_ROTATE Solar1Rot(0, Solar1Grp, 1, _V(42.147, -15.621, -70.948), _V(0, 0, 1), (float)(2 * PI));
	AddAnimationComponent(systems[1].id, 0, 1, &Solar1Rot);

	static UINT Solar2Grp[1] = { 6 };
	static MGROUP_ROTATE Solar2Rot(0, Solar2Grp, 1, _V(42.147, 15.621, -70.948), _V(0, 0, 1), (float)(2 * PI));
	AddAnimationComponent(systems[1].id, 0, 1, &Solar2Rot);
}

void EarthStation::clbkLoadStateEx(FILEHANDLE scn, void* status)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line)) 
	{
		for (auto& system : systems)
		{
			int length = strlen(system.scnName);

			if (!_strnicmp(line, system.scnName, length))
			{
				sscanf(line + length, "%d", &system.status);
				sscanf(line + length + 3, "%lf", &system.proc);

				SetAnimation(system.id, system.proc);
			}
			else ParseScenarioLineEx(line, status);
		}
	}
}

void EarthStation::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	for (auto& system : systems)
	{
		sprintf(buffer, "%d %0.4f", system.status, system.proc);
		oapiWriteScenario_string(scn, system.scnName, buffer);
	}
}

bool EarthStation::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
	VESSEL4::clbkDrawHUD(mode, hps, skp);

	// Determine the screen ratio
	int s = hps->H;
	double d = s * 0.00130208;
	int sw = hps->W;
	int lw = static_cast<int>(16 * sw / 1024);
	int x = 0;
	if (s / sw < 0.7284) x = (lw * 10) + 10;
	int y = static_cast<int>((168 * d) + (-88 * d));

	skp->Text(x, y, "Ctrl + G = Fuel boom", 20);
	y += 20;

	skp->Text(x, y, "Ctrl + K = Solar panels", 23);
	y += 20;

	y += 35;

	switch (systems[0].status)
	{
	case SystemStatus::CLOSED:
		skp->Text(x, y, "Fuel boom: Retracted", 20);
		break;
	case SystemStatus::OPENING:
		skp->Text(x, y, "Fuel boom: Extending", 20);
		break;
	case SystemStatus::OPEN:
		skp->Text(x, y, "Fuel boom: Extended", 19);
		break;
	case SystemStatus::CLOSING:
		skp->Text(x, y, "Fuel boom: Retracting", 21);
		break;
	}

	y += 20;

	if (systems[1].status == SystemStatus::CLOSED) skp->Text(x, y, "Solar panels: Stopped", 21);
	else skp->Text(x, y, "Solar panels: Active", 20);

	return true;
}

void EarthStation::clbkPostStep(double simt, double simdt, double mjd)
{
	if (systems[0].status == SystemStatus::OPENING || systems[0].status == SystemStatus::CLOSING)
	{
		double da = simdt * systems[0].speed;

		if (systems[0].status == SystemStatus::CLOSING)
		{
			if (systems[0].proc > 0.0) systems[0].proc = max(0.0, systems[0].proc - da);
			else systems[0].status = SystemStatus::CLOSED;
		}
		else
		{
			if (systems[0].proc < 1.0) systems[0].proc = min(1.0, systems[0].proc + da);
			else systems[0].status = SystemStatus::OPEN;
		}

		SetAnimation(systems[0].id, systems[0].proc);
	}

	if (systems[1].status == SystemStatus::OPEN)
	{
		double da = simdt * systems[1].speed;

		systems[1].proc = fmod(systems[1].proc + da, 1);

		SetAnimation(systems[1].id, systems[1].proc);
	}
}

int EarthStation::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
{
	if (!down || !KEYMOD_CONTROL(kstate)) return 0;

	switch (key)
	{
	// Fuel boom
	case OAPI_KEY_G:
		switch (systems[0].status)
		{
		case SystemStatus::CLOSED:
			systems[0].status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::CLOSING:
			systems[0].status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::OPENING:
			systems[0].status = SystemStatus::CLOSING;
			return 1;

		case SystemStatus::OPEN:
			systems[0].status = SystemStatus::CLOSING;
			return 1;

		default:
			systems[0].status = SystemStatus::CLOSED;
			return 1;
		}

	// Solar panels
	case OAPI_KEY_K:
		switch (systems[1].status)
		{
		default:
		case SystemStatus::OPEN:
			systems[1].status = SystemStatus::CLOSED;
			return 1;

		case SystemStatus::CLOSED:
			systems[1].status = SystemStatus::OPEN;
			return 1;
		}

	default: return 0;
	}
}