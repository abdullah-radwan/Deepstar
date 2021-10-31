// ==============================================================
//                 ORBITER MODULE: Deepstar Probe 2
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Probe2.cpp
// Control module for Deepstar Probe 2 vessel class
//
// ==============================================================

#include "Probe2.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Probe2(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Probe2*>(vessel); }

void Probe2::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DP_SIZE);
	SetEmptyMass(DP_EMPTYMASS);
	SetPMI(DP_PMI);
	SetCrossSections(DP_CS);
	SetTouchdownPoints(tdVtx, tdVtxN);

	DefineEngines();
	DefineSystems();

	EnableTransponder(true);
	SetTransponderChannel(480);
	attachHandle = CreateAttachment(true, _V(0, 0, 0), _V(0, -1, 0), _V(0, 0, 1), "Probe Attachment");

	AddMesh("Deepstar/Probe2");
}

void Probe2::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DP_FUELMASS);

	// Main engine
	THRUSTER_HANDLE mainHandle = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DP_MAXMAINTH, propHandle, DP_ISP);;
	CreateThrusterGroup(&mainHandle, 1, THGROUP_MAIN);

	AddExhaust(mainHandle, 0.6, 0.05, _V(0, 0, -1.6), _V(0, 0, -1));

	// RCS engines
	THRUSTER_HANDLE rcsHandles[16], groupHandles[4];

	// Up thrusters
	rcsHandles[0] = CreateThruster(_V(-0.633, 0.097, 0.089), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[0], 0.2, 0.01, _V(-0.633, 0.097, 0.089), _V(0, 1, 0));

	rcsHandles[1] = CreateThruster(_V(-0.633, 0.097, -0.927), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[1], 0.2, 0.01, _V(-0.633, 0.097, -0.927), _V(0, 1, 0));

	rcsHandles[2] = CreateThruster(_V(0.633, 0.097, 0.089), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[2], 0.2, 0.01, _V(0.633, 0.097, 0.089), _V(0, 1, 0));

	rcsHandles[3] = CreateThruster(_V(0.633, 0.097, -0.927), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[3], 0.2, 0.01, _V(0.633, 0.097, -0.927), _V(0, 1, 0));

	// Down thrusters
	rcsHandles[4] = CreateThruster(_V(-0.633, -0.096, 0.089), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[4], 0.2, 0.01, _V(-0.633, -0.096, 0.089), _V(0, -1, 0));

	rcsHandles[5] = CreateThruster(_V(-0.633, -0.096, -0.927), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[5], 0.2, 0.01, _V(-0.633, -0.096, -0.927), _V(0, -1, 0));

	rcsHandles[6] = CreateThruster(_V(0.633, -0.096, 0.089), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[6], 0.2, 0.01, _V(0.633, -0.096, 0.089), _V(0, -1, 0));

	rcsHandles[7] = CreateThruster(_V(0.633, -0.096, -0.927), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[7], 0.2, 0.01, _V(0.633, -0.096, -0.927), _V(0, -1, 0));

	// Right thrusters
	rcsHandles[8] = CreateThruster(_V(-0.73, 0, 0.089), _V(1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[8], 0.2, 0.01, _V(-0.73, 0, 0.089), _V(-1, 0, 0));

	rcsHandles[9] = CreateThruster(_V(-0.73, 0, -0.927), _V(1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[9], 0.2, 0.01, _V(-0.73, 0, -0.927), _V(-1, 0, 0));

	// Left thrusters
	rcsHandles[10] = CreateThruster(_V(0.73, 0, 0.089), _V(-1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[10], 0.2, 0.01, _V(0.73, 0, 0.089), _V(1, 0, 0));

	rcsHandles[11] = CreateThruster(_V(0.73, 0, -0.927), _V(-1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[11], 0.2, 0.01, _V(0.73, 0, -0.927), _V(1, 0, 0));

	// Forward thrusters
	rcsHandles[12] = CreateThruster(_V(-0.633, 0, 0.185), _V(0, 0, -1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[12], 0.2, 0.01, _V(-0.633, 0, 0.185), _V(0, 0, 1));

	rcsHandles[13] = CreateThruster(_V(0.633, 0, 0.185), _V(0, 0, -1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[13], 0.2, 0.01, _V(0.633, 0, 0.185), _V(0, 0, 1));

	// Aft thrusters
	rcsHandles[14] = CreateThruster(_V(-0.633, 0, -1.024), _V(0, 0, 1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[14], 0.2, 0.01, _V(-0.633, 0, -1.024), _V(0, 0, -1));

	rcsHandles[15] = CreateThruster(_V(0.633, 0, -1.024), _V(0, 0, 1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[15], 0.2, 0.01, _V(0.633, 0, -1.024), _V(0, 0, -1));

	groupHandles[0] = rcsHandles[1];
	groupHandles[1] = rcsHandles[3];
	groupHandles[2] = rcsHandles[4];
	groupHandles[3] = rcsHandles[6];
	CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHUP);

	groupHandles[0] = rcsHandles[0];
	groupHandles[1] = rcsHandles[2];
	groupHandles[2] = rcsHandles[5];
	groupHandles[3] = rcsHandles[7];
	CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHDOWN);

	groupHandles[0] = rcsHandles[2];
	groupHandles[1] = rcsHandles[3];
	groupHandles[2] = rcsHandles[4];
	groupHandles[3] = rcsHandles[5];
	CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_BANKRIGHT);

	groupHandles[0] = rcsHandles[0];
	groupHandles[1] = rcsHandles[1];
	groupHandles[2] = rcsHandles[6];
	groupHandles[3] = rcsHandles[7];
	CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_BANKLEFT);

	groupHandles[0] = rcsHandles[8];
	groupHandles[1] = rcsHandles[11];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_YAWRIGHT);

	groupHandles[0] = rcsHandles[9];
	groupHandles[1] = rcsHandles[10];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_YAWLEFT);

	groupHandles[0] = rcsHandles[4];
	groupHandles[1] = rcsHandles[5];
	groupHandles[2] = rcsHandles[6];
	groupHandles[3] = rcsHandles[7];
	CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_UP);

	groupHandles[0] = rcsHandles[0];
	groupHandles[1] = rcsHandles[1];
	groupHandles[2] = rcsHandles[2];
	groupHandles[3] = rcsHandles[3];
	CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_DOWN);

	groupHandles[0] = rcsHandles[8];
	groupHandles[1] = rcsHandles[9];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_RIGHT);

	groupHandles[0] = rcsHandles[10];
	groupHandles[1] = rcsHandles[11];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_LEFT);

	groupHandles[0] = rcsHandles[14];
	groupHandles[1] = rcsHandles[15];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_FORWARD);

	groupHandles[0] = rcsHandles[12];
	groupHandles[1] = rcsHandles[13];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BACK);
}

void Probe2::DefineSystems()
{
	solarSystem.id = CreateAnimation(0);
	solarSystem.status = SystemStatus::CLOSED;
	solarSystem.proc = 0;
	solarSystem.speed = 0.083;

	static UINT AnimGrp1[1] = { 2 };
	static MGROUP_ROTATE AnimRot1(0, AnimGrp1, 1, _V(-0.395, 0.395, -1.09), _V(-0.7, -0.7, 0), (float)(PI05));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimRot1);

	static UINT AnimGrp2[1] = { 3 };
	static MGROUP_ROTATE AnimRot2(0, AnimGrp2, 1, _V(-0.395, -0.395, -1.09), _V(0.7, -0.7, 0), (float)(PI05));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimRot2);

	static UINT AnimGrp3[1] = { 4 };
	static MGROUP_ROTATE AnimRot3(0, AnimGrp3, 1, _V(0.395, 0.395, -1.09), _V(-0.7, 0.7, 0), (float)(PI05));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimRot3);

	static UINT AnimGrp4[1] = { 5 };
	static MGROUP_ROTATE AnimRot4(0, AnimGrp4, 1, _V(0.395, -0.395, -1.09), _V(0.7, 0.7, 0), (float)(PI05));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimRot4);

	static UINT AnimGrp5[1] = { 6 };
	static MGROUP_TRANSLATE AnimTrans(0, AnimGrp5, 1, _V(0, 0.24, 0));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimTrans);
}