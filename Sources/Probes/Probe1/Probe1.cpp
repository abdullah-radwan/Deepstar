// ==============================================================
//                 ORBITER MODULE: Deepstar Probe 1
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Probe1.cpp
// Control module for Deepstar Probe 1 vessel class
//
// ==============================================================

#include "Probe1.h"

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Probe1(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Probe1*>(vessel); }

void Probe1::clbkSetClassCaps(FILEHANDLE cfg)
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

	AddMesh("Deepstar/Probe1");
}

void Probe1::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DP_FUELMASS);

	// Main engine
	THRUSTER_HANDLE mainHandle = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DP_MAXMAINTH, propHandle, DP_ISP);;
	CreateThrusterGroup(&mainHandle, 1, THGROUP_MAIN);

	AddExhaust(mainHandle, 0.6, 0.05, _V(0, 0, -1.585), _V(0, 0, -1));

	// RCS engines
	THRUSTER_HANDLE rcsHandles[16], groupHandles[4];

	// Up thrusters
	rcsHandles[0] = CreateThruster(_V(-0.633, 0.097, 0.736), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[0], 0.2, 0.01, _V(-0.633, 0.097, 0.736), _V(0, 1, 0));

	rcsHandles[1] = CreateThruster(_V(-0.633, 0.097, -0.479), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[1], 0.2, 0.01, _V(-0.633, 0.097, -0.479), _V(0, 1, 0));

	rcsHandles[2] = CreateThruster(_V(0.633, 0.097, 0.736), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[2], 0.2, 0.01, _V(0.633, 0.097, 0.736), _V(0, 1, 0));

	rcsHandles[3] = CreateThruster(_V(0.633, 0.097, -0.479), _V(0, -1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[3], 0.2, 0.01, _V(0.633, 0.097, -0.479), _V(0, 1, 0));

	// Down thrusters
	rcsHandles[4] = CreateThruster(_V(-0.633, -0.096, 0.736), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[4], 0.2, 0.01, _V(-0.633, -0.096, 0.736), _V(0, -1, 0));

	rcsHandles[5] = CreateThruster(_V(-0.633, -0.096, -0.479), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[5], 0.2, 0.01, _V(-0.633, -0.096, -0.479), _V(0, -1, 0));

	rcsHandles[6] = CreateThruster(_V(0.633, -0.096, 0.736), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[6], 0.2, 0.01, _V(0.633, -0.096, 0.736), _V(0, -1, 0));

	rcsHandles[7] = CreateThruster(_V(0.633, -0.096, -0.479), _V(0, 1, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[7], 0.2, 0.01, _V(0.633, -0.096, -0.479), _V(0, -1, 0));

	// Right thrusters
	rcsHandles[8] = CreateThruster(_V(-0.73, 0, 0.736), _V(1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[8], 0.2, 0.01, _V(-0.73, 0, 0.736), _V(-1, 0, 0));

	rcsHandles[9] = CreateThruster(_V(-0.73, 0, -0.479), _V(1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[9], 0.2, 0.01, _V(-0.73, 0, -0.479), _V(-1, 0, 0));

	// Left thrusters
	rcsHandles[10] = CreateThruster(_V(0.73, 0, 0.736), _V(-1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[10], 0.2, 0.01, _V(0.73, 0, 0.736), _V(1, 0, 0));

	rcsHandles[11] = CreateThruster(_V(0.73, 0, -0.479), _V(-1, 0, 0), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[11], 0.2, 0.01, _V(0.73, 0, -0.479), _V(1, 0, 0));

	// Forward thrusters
	rcsHandles[12] = CreateThruster(_V(-0.633, 0, 0.832), _V(0, 0, -1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[12], 0.2, 0.01, _V(-0.633, 0, 0.832), _V(0, 0, 1));

	rcsHandles[13] = CreateThruster(_V(0.633, 0, 0.832), _V(0, 0, -1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[13], 0.2, 0.01, _V(0.633, 0, 0.832), _V(0, 0, 1));

	// Aft thrusters
	rcsHandles[14] = CreateThruster(_V(-0.633, 0, -0.576), _V(0, 0, 1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[14], 0.2, 0.01, _V(-0.633, 0, -0.576), _V(0, 0, -1));

	rcsHandles[15] = CreateThruster(_V(0.633, 0, -0.576), _V(0, 0, 1), DP_MAXRCSTH, propHandle, DP_ISP);
	AddExhaust(rcsHandles[15], 0.2, 0.01, _V(0.633, 0, -0.576), _V(0, 0, -1));

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

void Probe1::DefineSystems()
{
	solarSystem.id = CreateAnimation(0);
	solarSystem.status = SystemStatus::CLOSED;
	solarSystem.proc = 0;
	solarSystem.speed = 0.067;

	static UINT AnimGrp1[1] = { 0 };
	static MGROUP_ROTATE AnimRot1(0, AnimGrp1, 1, _V(0, 0.645, -0.554), _V(-1, 0, 0), (float)(PI05));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimRot1);

	static UINT AnimGrp2[1] = { 1 };
	static MGROUP_ROTATE AnimRot2(0, AnimGrp2, 1, _V(0, -0.645, -0.554), _V(1, 0, 0), (float)(PI05));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimRot2);

	static UINT AnimGrp3[1] = { 5 };
	static MGROUP_TRANSLATE AnimTrans1(0, AnimGrp3, 1, _V(0.923, 0, 0));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimTrans1);

	static UINT AnimGrp4[1] = { 4 };
	static MGROUP_TRANSLATE AnimTrans2(0, AnimGrp4, 1, _V(-0.923, 0, 0));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimTrans2);

	static UINT AnimGrp5[1] = { 6 };
	static MGROUP_SCALE AnimScale(0, AnimGrp5, 1, _V(0, 0.129, 0), _V(2.57, 1, 1));
	AddAnimationComponent(solarSystem.id, 0, 1, &AnimScale);
}