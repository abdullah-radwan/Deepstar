// ==============================================================
//                 ORBITER MODULE: Deepstar Base Probe
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// BaseProbe.h
// Class interface for Deepstar Base Probe vessel class
//
// ==============================================================

#pragma once
#include <UCSO/CustomCargo.h>
#include "../Common.h"

class BaseProbe : public VESSEL4, public UCSO::CustomCargo
{
public:
	BaseProbe(OBJHANDLE hVessel, int flightmodel);

	void clbkLoadStateEx(FILEHANDLE scn, void* status);
	void clbkSaveState(FILEHANDLE scn);

	bool clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp);
	void clbkPostStep(double simt, double simdt, double mjd);
	int clbkConsumeBufferedKey(DWORD key, bool down, char* kstate);

	CargoInfo GetCargoInfo() override;
	OBJHANDLE GetCargoHandle() override;
	ATTACHMENTHANDLE GetCargoAttachmentHandle() override;

	ATTACHMENTHANDLE attachHandle = nullptr;
	System solarSystem;

private:
	UCSO::CustomCargo::CargoInfo cargoInfo;
};