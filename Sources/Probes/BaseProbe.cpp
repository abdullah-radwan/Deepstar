// ==============================================================
//                 ORBITER MODULE: Deepstar Base Probe
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// BaseProbe.cpp
// Control module for Deepstar Base Probe vessel class
//
// ==============================================================

#include "BaseProbe.h"
#include "../Common.h"

BaseProbe::BaseProbe(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel) { cargoInfo.type = UCSO::CustomCargo::STATIC; }

void BaseProbe::clbkLoadStateEx(FILEHANDLE scn, void* status)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line)) 
	{
		if (!_strnicmp(line, "SOLAR_PANELS", 12)) 
		{
			sscanf(line + 12, "%d", &solarSystem.status);
			sscanf(line + 15, "%lf", &solarSystem.proc);
			SetAnimation(solarSystem.id, solarSystem.proc);
		}
		else ParseScenarioLineEx(line, status);
	}
}

void BaseProbe::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	char buffer[256];
	sprintf(buffer, "%d %0.4f", solarSystem.status, solarSystem.proc);

	oapiWriteScenario_string(scn, "SOLAR_PANELS", buffer);
}

bool BaseProbe::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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

	skp->Text(x, y, "Ctrl + K = Solar panels", 23);
	y += 35;

	switch (solarSystem.status)
	{
	case SystemStatus::CLOSED:
		skp->Text(x, y, "Solar panels: Retracted", 23);
		break;
	case SystemStatus::OPENING:
		skp->Text(x, y, "Solar panels: Extending", 23);
		break;
	case SystemStatus::OPEN:
		skp->Text(x, y, "Solar panels: Extended", 22);
		break;
	case SystemStatus::CLOSING:
		skp->Text(x, y, "Solar panels: Retracting", 24);
		break;
	}

	return true;
}

void BaseProbe::clbkPostStep(double simt, double simdt, double mjd)
{
	if (solarSystem.status == SystemStatus::OPENING || solarSystem.status == SystemStatus::CLOSING)
	{
		double da = simdt * solarSystem.speed;

		if (solarSystem.status == SystemStatus::CLOSING)
		{
			if (solarSystem.proc > 0.0) solarSystem.proc = max(0.0, solarSystem.proc - da);
			else solarSystem.status = SystemStatus::CLOSED;
		}
		else
		{
			if (solarSystem.proc < 1.0) solarSystem.proc = min(1.0, solarSystem.proc + da);
			else solarSystem.status = SystemStatus::OPEN;
		}

		SetAnimation(solarSystem.id, solarSystem.proc);
	}

	if (GroundContact() && !(GetFlightStatus() & 1) && GetGroundspeed() <= 0.2)
	{
		VECTOR3 thrustVector;

		if (!GetThrustVector(thrustVector)) SetStatusLanded(this, 0.675503);
	}
}

int BaseProbe::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
{
	if (!down || !KEYMOD_CONTROL(kstate)) return 0;

	switch (key)
	{
	case OAPI_KEY_K:
		switch (solarSystem.status)
		{
		case SystemStatus::CLOSED:
			solarSystem.status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::CLOSING:
			solarSystem.status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::OPENING:
			solarSystem.status = SystemStatus::CLOSING;
			return 1;

		case SystemStatus::OPEN:
			solarSystem.status = SystemStatus::CLOSING;
			return 1;

		default:
			solarSystem.status = SystemStatus::CLOSED;
			return 1;
		}

	default: return 0;
	}
}

UCSO::CustomCargo::CargoInfo BaseProbe::GetCargoInfo() { return cargoInfo; }

OBJHANDLE BaseProbe::GetCargoHandle() { return GetHandle(); }

ATTACHMENTHANDLE BaseProbe::GetCargoAttachmentHandle() { return attachHandle; }