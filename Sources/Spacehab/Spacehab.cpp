// ==============================================================
//                 ORBITER MODULE: Deepstar Spacehab
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Spacehab.cpp
// Control module for Deepstar Spacehab vessel class
//
// ==============================================================

#include "Spacehab.h"
#include "../Common.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Spacehab(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Spacehab*>(vessel); }

Spacehab::Spacehab(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel) { cargoInfo.type = UCSO::CustomCargo::STATIC; }

void Spacehab::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DH_SIZE);
	SetEmptyMass(DH_EMPTYMASS);
	SetPMI(DH_PMI);
	SetCrossSections(DH_CS);
	SetTouchdownPoints(tdVtx, tdVtxN);

	attachHandle = CreateAttachment(true, _V(0, -0.532, 0.175), _V(0, -1, 0), _V(0, 0, 1), "Spacehab Attachment");

	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DH_FUELMASS);

	THRUSTER_HANDLE rcsHandles[14], groupHandles[4];

	rcsHandles[0] = CreateThruster(_V(-6, 0.7, -0.4), _V(0, -1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[1] = CreateThruster(_V(6, -0.7, -0.4), _V(0, 1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[2] = CreateThruster(_V(-6, -0.7, -0.4), _V(0, 1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[3] = CreateThruster(_V(6, 0.7, -0.4), _V(0, -1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[4] = CreateThruster(_V(0, 0, 15), _V(0, 1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[5] = CreateThruster(_V(0, 0, -15), _V(0, -1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[6] = CreateThruster(_V(0, 0, 15), _V(0, -1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[7] = CreateThruster(_V(0, 0, -15), _V(0, 1, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[8] = CreateThruster(_V(0, 0, 15), _V(1, 0, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[9] = CreateThruster(_V(0, 0, -15), _V(-1, 0, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[10] = CreateThruster(_V(0, 0, 15), _V(-1, 0, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[11] = CreateThruster(_V(0, 0, -15), _V(1, 0, 0), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[12] = CreateThruster(_V(0, 0, 0), _V(0, 0, -1), DH_MAXRCSTH, propHandle, DH_ISP);
	rcsHandles[13] = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DH_MAXRCSTH, propHandle, DH_ISP);

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

	AddMesh("Deepstar/Spacehab");
}

void Spacehab::clbkPostStep(double simt, double simdt, double mjd)
{
	if (GroundContact() && !(GetFlightStatus() & 1) && GetGroundspeed() <= 0.2)
	{
		VECTOR3 thrustVector;

		if (!GetThrustVector(thrustVector)) SetStatusLanded(this, 1.24572);
	}
}

UCSO::CustomCargo::CargoInfo Spacehab::GetCargoInfo() { return cargoInfo; }

OBJHANDLE Spacehab::GetCargoHandle() { return GetHandle(); }

ATTACHMENTHANDLE Spacehab::GetCargoAttachmentHandle() { return attachHandle; }