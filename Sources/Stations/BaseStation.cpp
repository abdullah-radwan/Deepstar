// ==============================================================
//                 ORBITER MODULE: Deepstar Base Station
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// BaseStation.cpp
// Control module for Deepstar Base Station class
//
// ==============================================================

#include "BaseStation.h"

void BaseStation::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DB_FUELMASS);

	THRUSTER_HANDLE rcsHandles[14], groupHandles[4];

	rcsHandles[0] = CreateThruster(_V(-6, 0.7, -0.4), _V(0, -1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[1] = CreateThruster(_V(6, -0.7, -0.4), _V(0, 1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[2] = CreateThruster(_V(-6, -0.7, -0.4), _V(0, 1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[3] = CreateThruster(_V(6, 0.7, -0.4), _V(0, -1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[4] = CreateThruster(_V(0, 0, 15), _V(0, 1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[5] = CreateThruster(_V(0, 0, -15), _V(0, -1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[6] = CreateThruster(_V(0, 0, 15), _V(0, -1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[7] = CreateThruster(_V(0, 0, -15), _V(0, 1, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[8] = CreateThruster(_V(0, 0, 15), _V(1, 0, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[9] = CreateThruster(_V(0, 0, -15), _V(-1, 0, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[10] = CreateThruster(_V(0, 0, 15), _V(-1, 0, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[11] = CreateThruster(_V(0, 0, -15), _V(1, 0, 0), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[12] = CreateThruster(_V(0, 0, 0), _V(0, 0, -1), DB_MAXRCSTH, propHandle, DB_ISP);
	rcsHandles[13] = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DB_MAXRCSTH, propHandle, DB_ISP);

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

void BaseStation::clbkLoadStateEx(FILEHANDLE scn, void* status)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line))
	{
		for (int index = 0; index < stationData.systemCount; index++)
		{
			System* system = &stationData.systems[index];

			int length = strlen(system->scnName);

			if (!_strnicmp(line, system->scnName, length))
			{
				sscanf(line + length, "%d", &system->status);
				sscanf(line + length + 3, "%lf", &system->proc);

				SetAnimation(system->id, system->proc);
			}
			else ParseScenarioLineEx(line, status);
		}
	}
}

void BaseStation::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	for (int index = 0; index < stationData.systemCount; index++)
	{
		System* system = &stationData.systems[index];

		sprintf(buffer, "%d %0.4f", system->status, system->proc);
		oapiWriteScenario_string(scn, system->scnName, buffer);
	}
}

bool BaseStation::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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

	skp->Text(x, y, "Ctrl + 1 = Forward dish", 23);
	y += 20;

	skp->Text(x, y, "Ctrl + 2 = Aft dish", 19);

	if (stationData.hasSpin)
	{
		y += 35;
		skp->Text(x, y, "Ctrl + G = Spin gravity", 23);
	}

	if (stationData.hasSolar)
	{
		if (stationData.hasSpin) y += 20;
		else y += 35;
		skp->Text(x, y, "Ctrl + K = Solar panels", 23);
	}

	y += 35;

	for (int index = 0; index < stationData.systemCount; index++)
	{
		System* system = &stationData.systems[index];

		if (system->status == SystemStatus::CLOSED) sprintf(buffer, "%s: Stopped", system->infoName);
		else sprintf(buffer, "%s: Active", system->infoName);

		skp->Text(x, y, buffer, strlen(buffer));

		y += 20;
	}

	return true;
}

void BaseStation::clbkPostStep(double simt, double simdt, double mjd)
{
	for (int index = 0; index < stationData.systemCount; index++)
	{
		System* system = &stationData.systems[index];

		if (system->status == SystemStatus::OPEN)
		{
			double da = simdt * system->speed;

			system->proc = fmod(system->proc + da, 1);

			SetAnimation(system->id, system->proc);
		}
	}
}

int BaseStation::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
{
	if (!down || !KEYMOD_CONTROL(kstate)) return 0;

	System* system = nullptr;

	switch (key)
	{
	// Port dish
	case OAPI_KEY_1:
		system = &stationData.systems[0];
		break;

	// Starboard dish
	case OAPI_KEY_2:
		system = &stationData.systems[1];
		break;

	// Spin gravity
	case OAPI_KEY_G:
		if (stationData.hasSpin) system = &stationData.systems[2];
		break;

	// Solar panels
	case OAPI_KEY_K:
		if (stationData.hasSolar) system = &stationData.systems[(stationData.hasSpin ? 3 : 2)];
		break;

	default: return 0;
	};

	if (!system) return 0;

	switch (system->status)
	{
	default:
	case SystemStatus::OPEN:
		system->status = SystemStatus::CLOSED;
		return 1;

	case SystemStatus::CLOSED:
		system->status = SystemStatus::OPEN;
		return 1;
	}
}