// ==============================================================
//                 ORBITER MODULE: Deepstar Lander
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Lander.cpp
// Control module for Deepstar Lander vessel class
//
// ==============================================================

#include "Lander.h"

int Lander::vesselCount = 0;
oapi::Font* Lander::mfdFont = nullptr;
oapi::Font* Lander::systemFont = nullptr;

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Lander(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Lander*>(vessel); }

Lander::Lander(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel)
{
	ucso = UCSO::Vessel::CreateInstance(this);
	ucso->SetUnpackingRange(15);

	sprintf(buffer, "UCSO version: %s", ucso->GetUCSOVersion());
	cargoData.message = _strdup(buffer);

	if (!vesselCount)
	{
		mfdFont = oapiCreateFont(24, true, "Arial", FONT_BOLD);
		systemFont = oapiCreateFont(30, true, "Arial", FONT_BOLD);
	}

	vesselCount++;
}

Lander::~Lander()
{
	delete ucso;

	vesselCount--;

	if (!vesselCount)
	{
		oapiReleaseFont(mfdFont);
		oapiReleaseFont(systemFont);
	}
}

void Lander::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DL_SIZE);
	SetEmptyMass(DL_EMPTYMASS);
	SetPMI(DL_PMI);
	SetCrossSections(DL_CS);
	SetTouchdownPoints(tdVtx, tdVtxN);

	DefineEngines();
	DefineSystems();
	DefineSlots();

	vcMesh = oapiLoadMesh("Deepstar/LanderVC");

	SetMeshVisibilityMode(AddMesh("Deepstar/Lander"), MESHVIS_EXTERNAL);
	SetMeshVisibilityMode(AddMesh(vcMesh), MESHVIS_VC);

	SetCameraOffset({ -0.44, 0.9, 3.2 });
}

void Lander::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DL_FUELMASS);

	// Hover engine
	THRUSTER_HANDLE hoverHandle = CreateThruster(_V(0, 0, 0), _V(0, 1, 0), DL_MAXHVRTH, propHandle, DL_ISP);;
	CreateThrusterGroup(&hoverHandle, 1, THGROUP_HOVER);

	AddExhaust(hoverHandle, 3, 0.6, _V(0, -6, 0), _V(0, -1, 0));

	// RCS engines
	// Up thrusters
	rcsHandles[0] = CreateThruster(_V(-3.836, 0, 3.6), _V(0, -1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[0], 0.4, 0.1, _V(-3.836, 0.746, 3.6), _V(0, 1, 0));

	rcsHandles[1] = CreateThruster(_V(-3.836, 0, -3.6), _V(0, -1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[1], 0.4, 0.1, _V(-3.836, 0.746, -3.6), _V(0, 1, 0));

	rcsHandles[2] = CreateThruster(_V(3.836, 0, 3.6), _V(0, -1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[2], 0.4, 0.1, _V(3.836, 0.746, 3.6), _V(0, 1, 0));

	rcsHandles[3] = CreateThruster(_V(3.836, 0, -3.6), _V(0, -1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[3], 0.4, 0.1, _V(3.836, 0.746, -3.6), _V(0, 1, 0));

	// Down thrusters
	rcsHandles[4] = CreateThruster(_V(-3.836, 0, 3.6), _V(0, 1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[4], 0.4, 0.1, _V(-3.836, -0.288, 3.6), _V(0, -1, 0));

	rcsHandles[5] = CreateThruster(_V(-3.836, 0, -3.6), _V(0, 1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[5], 0.4, 0.1, _V(-3.836, -0.288, -3.6), _V(0, -1, 0));

	rcsHandles[6] = CreateThruster(_V(3.836, 0, 3.6), _V(0, 1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[6], 0.4, 0.1, _V(3.836, -0.288, 3.6), _V(0, -1, 0));

	rcsHandles[7] = CreateThruster(_V(3.836, 0, -3.6), _V(0, 1, 0), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[7], 0.4, 0.1, _V(3.836, -0.288, -3.6), _V(0, -1, 0));

	// Right thrusters
	rcsHandles[8] = CreateThruster(_V(-4.187, 0, 3.6), _V(1, 0, 0), DL_MAXRCSTH * 2, propHandle, DL_ISP);
	AddExhaust(rcsHandles[8], 0.4, 0.1, _V(-4.187, 0.229, 3.6), _V(-1, 0, 0));

	rcsHandles[9] = CreateThruster(_V(-4.187, 0, -3.6), _V(1, 0, 0), DL_MAXRCSTH * 2, propHandle, DL_ISP);
	AddExhaust(rcsHandles[9], 0.4, 0.1, _V(-4.187, 0.229, -3.6), _V(-1, 0, 0));

	// Left thrusters
	rcsHandles[10] = CreateThruster(_V(4.187, 0, 3.6), _V(-1, 0, 0), DL_MAXRCSTH * 2, propHandle, DL_ISP);
	AddExhaust(rcsHandles[10], 0.4, 0.1, _V(4.187, 0.229, 3.6), _V(1, 0, 0));

	rcsHandles[11] = CreateThruster(_V(4.187, 0, -3.6), _V(-1, 0, 0), DL_MAXRCSTH * 2, propHandle, DL_ISP);
	AddExhaust(rcsHandles[11], 0.4, 0.1, _V(4.187, 0.229, -3.6), _V(1, 0, 0));

	// Forward thrusters
	rcsHandles[12] = CreateThruster(_V(-3.836, 0, 4.03), _V(0, 0, -1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[12], 0.4, 0.1, _V(-3.836, 0.228, 4.03), _V(0, 0, 1));

	rcsHandles[13] = CreateThruster(_V(-3.836, 0, -3.169), _V(0, 0, -1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[13], 0.4, 0.1, _V(-3.836, 0.228, -3.169), _V(0, 0, 1));

	rcsHandles[14] = CreateThruster(_V(3.836, 0, 4.03), _V(0, 0, -1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[14], 0.4, 0.1, _V(3.836, 0.228, 4.03), _V(0, 0, 1));

	rcsHandles[15] = CreateThruster(_V(3.836, 0, -3.169), _V(0, 0, -1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[15], 0.4, 0.1, _V(3.836, 0.228, -3.169), _V(0, 0, 1));

	// Aft thrusters
	rcsHandles[16] = CreateThruster(_V(-3.836, 0, 3.169), _V(0, 0, 1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[16], 0.4, 0.1, _V(-3.836, 0.228, 3.169), _V(0, 0, -1));

	rcsHandles[17] = CreateThruster(_V(-3.836, 0, -4.03), _V(0, 0, 1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[17], 0.4, 0.1, _V(-3.836, 0.228, -4.03), _V(0, 0, -1));

	rcsHandles[18] = CreateThruster(_V(3.836, 0, 3.169), _V(0, 0, 1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[18], 0.4, 0.1, _V(3.836, 0.228, 3.169), _V(0, 0, -1));

	rcsHandles[19] = CreateThruster(_V(3.836, 0, -4.03), _V(0, 0, 1), DL_MAXRCSTH, propHandle, DL_ISP);
	AddExhaust(rcsHandles[19], 0.4, 0.1, _V(3.836, 0.228, -4.03), _V(0, 0, -1));
}

void Lander::DefineSystems()
{
	// Transponder and docking port
	int channelFactor = 10 * (vesselCount - 1);

	EnableTransponder(true);
	SetTransponderChannel(540 + channelFactor);

	DOCKHANDLE dockPort = CreateDock({ 0, 2.697, 0 }, { 0, 1, 0 }, { 0, 0, 1 });
	EnableIDS(dockPort, true);
	SetIDSChannel(dockPort, 542 + channelFactor);

	// Egress hatch
	hatchSystem.id = CreateAnimation(0);
	hatchSystem.status = SystemStatus::CLOSED;
	hatchSystem.proc = 0;
	hatchSystem.speed = 0.167;

	static UINT AnimGrp[1] = { 1 };
	static MGROUP_ROTATE AnimRot(0, AnimGrp, 1, _V(0.559, 0, -4.754), _V(0, 1, 0), (float)(-PI05));
	AddAnimationComponent(hatchSystem.id, 0, 1, &AnimRot);
}

void Lander::DefineSlots()
{
	ucso->SetSlotAttachment(0, CreateAttachment(false, { -0.662, -1.691, 0.124 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 1"));
	ucso->SetSlotAttachment(1, CreateAttachment(false, { 0.688, -1.691, 0.124 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 2"));
	ucso->SetSlotAttachment(2, CreateAttachment(false, { -0.662, -1.691, -1.276 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 3"));
	ucso->SetSlotAttachment(3, CreateAttachment(false, { 0.688, -1.691, -1.276 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 4"));

	ucso->SetSlotAttachment(4, CreateAttachment(false, { -0.662, -0.391, 0.124 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 5"));
	ucso->SetSlotAttachment(5, CreateAttachment(false, { 0.688, -0.391, 0.124 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 6"));
	ucso->SetSlotAttachment(6, CreateAttachment(false, { -0.662, -0.391, -1.276 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 7"));
	ucso->SetSlotAttachment(7, CreateAttachment(false, { 0.688, -0.391, -1.276 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 8"));
}

void Lander::clbkLoadStateEx(FILEHANDLE scn, void* status)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, "EGRESS_HATCH", 12))
		{
			sscanf(line + 12, "%d", &hatchSystem.status);
			sscanf(line + 15, "%lf", &hatchSystem.proc);

			SetAnimation(hatchSystem.id, hatchSystem.proc);
		}

		else if (!_strnicmp(line, "RCS_CONFIG", 10))
		{
			int rcsConfig;
			sscanf(line + 10, "%d", &rcsConfig);

			this->rcsConfig = rcsConfig;
		}

		else ParseScenarioLineEx(line, status);
	}
}

void Lander::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	sprintf(buffer, "%d %0.4f", hatchSystem.status, hatchSystem.proc);
	oapiWriteScenario_string(scn, "EGRESS_HATCH", buffer);

	oapiWriteScenario_int(scn, "RCS_CONFIG", rcsConfig);
}

bool Lander::clbkLoadVC(int id)
{
	static VCHUDSPEC hud = { 1, 17, { -0.44, 0.92, 3.97 }, 1 };

	switch (id)
	{
	case VCView::SEAT_LEFT:
	{
		oapiVCSetNeighbours(-1, VCView::SEAT_RIGHT, VCView::TOP_LEFT, -1);

		SetCameraOffset({ -0.44, 0.9, 3.2 });

		SetCameraDefaultDirection({ 0, 0, 1 });

		hud.hudcnt = { -0.44, 0.92, 3.97 };

		break;
	}
	case VCView::SEAT_RIGHT:
	{
		oapiVCSetNeighbours(VCView::SEAT_LEFT, -1, VCView::TOP_RIGHT, -1);

		SetCameraOffset({ 0.44, 0.9, 3.2 });

		SetCameraDefaultDirection({ 0, 0, 1 });

		hud.hudcnt = { 0.44, 0.92, 3.97 };

		break;
	}
	case VCView::TOP_LEFT:
		oapiVCSetNeighbours(-1, VCView::TOP_RIGHT, -1, VCView::SEAT_LEFT);

		SetCameraOffset({ -0.373, 1.3, 3 });

		SetCameraDefaultDirection({ 0, 1, 0 }); // 0, sin(62.5 * RAD), cos(62.5 * RAD)

		return true;

	case VCView::TOP_RIGHT:
		oapiVCSetNeighbours(VCView::TOP_LEFT, -1, -1, VCView::SEAT_RIGHT);

		SetCameraOffset({ 0.381, 1.3, 3 });

		SetCameraDefaultDirection({ 0, 1, 0 }); // 0, sin(62.5 * RAD), cos(62.5 * RAD)

		return true;
	}

	oapiVCRegisterHUD(&hud);

	static VCMFDSPEC mfd1 = { 1, 6 };
	static VCMFDSPEC mfd2 = { 1, 7 };
	static VCMFDSPEC mfd3 = { 1, 12 };
	static VCMFDSPEC mfd4 = { 1, 13 };

	oapiVCRegisterMFD(MFD_LEFT, &mfd1);
	oapiVCRegisterMFD(MFD_RIGHT, &mfd2);
	oapiVCRegisterMFD(MFD_USER1, &mfd3);
	oapiVCRegisterMFD(MFD_USER2, &mfd4);

	static const VECTOR3 mfdKeyPositions[48] =
	{
		_V(-0.835, 0.482487, 3.92396), // MFD 1 KEY 1
		_V(-0.835, 0.437382, 3.90755), // MFD 1 KEY 2
		_V(-0.835, 0.392277, 3.89113), // MFD 1 KEY 3
		_V(-0.835, 0.347172, 3.87471), // MFD 1 KEY 4
		_V(-0.835, 0.302067, 3.8583),  // MFD 1 KEY 5
		_V(-0.835, 0.256961, 3.84188), // MFD 1 KEY 6

		_V(-0.485, 0.482487, 3.92396), // MFD 1 KEY 7
		_V(-0.485, 0.437382, 3.90755), // MFD 1 KEY 8
		_V(-0.485, 0.392277, 3.89113), // MFD 1 KEY 9
		_V(-0.485, 0.347172, 3.87471), // MFD 1 KEY 10
		_V(-0.485, 0.302067, 3.8583),  // MFD 1 KEY 11
		_V(-0.485, 0.256961, 3.84188), // MFD 1 KEY 12

		_V(-0.395, 0.482487, 3.92396), // MFD 2 KEY 1
		_V(-0.395, 0.437382, 3.90755), // MFD 2 KEY 2
		_V(-0.395, 0.392277, 3.89113), // MFD 2 KEY 3
		_V(-0.395, 0.347172, 3.87471), // MFD 2 KEY 4
		_V(-0.395, 0.302067, 3.8583),  // MFD 2 KEY 5
		_V(-0.395, 0.256961, 3.84188), // MFD 2 KEY 6

		_V(-0.045, 0.482487, 3.92396), // MFD 2 KEY 7
		_V(-0.045, 0.437382, 3.90755), // MFD 2 KEY 8
		_V(-0.045, 0.392277, 3.89113), // MFD 2 KEY 9
		_V(-0.045, 0.347172, 3.87471), // MFD 2 KEY 10
		_V(-0.045, 0.302067, 3.8583),  // MFD 2 KEY 11
		_V(-0.045, 0.256961, 3.84188), // MFD 2 KEY 12

		_V(0.045, 0.482487, 3.92396),  // MFD 3 KEY 1
		_V(0.045, 0.437382, 3.90755),  // MFD 3 KEY 2
		_V(0.045, 0.392277, 3.89113),  // MFD 3 KEY 3
		_V(0.045, 0.347172, 3.87471),  // MFD 3 KEY 4
		_V(0.045, 0.302067, 3.8583),   // MFD 3 KEY 5
		_V(0.045, 0.256961, 3.84188),  // MFD 3 KEY 6

		_V(0.395, 0.482487, 3.92396),  // MFD 3 KEY 7
		_V(0.395, 0.437382, 3.90755),  // MFD 3 KEY 8
		_V(0.395, 0.392277, 3.89113),  // MFD 3 KEY 9
		_V(0.395, 0.347172, 3.87471),  // MFD 3 KEY 10
		_V(0.395, 0.302067, 3.8583),   // MFD 3 KEY 11
		_V(0.395, 0.256961, 3.84188),  // MFD 3 KEY 12

		_V(0.485, 0.482487, 3.92396),  // MFD 4 KEY 1
		_V(0.485, 0.437382, 3.90755),  // MFD 4 KEY 2
		_V(0.485, 0.392277, 3.89113),  // MFD 4 KEY 3
		_V(0.485, 0.347172, 3.87471),  // MFD 4 KEY 4
		_V(0.485, 0.302067, 3.8583),   // MFD 4 KEY 5
		_V(0.485, 0.256961, 3.84188),  // MFD 4 KEY 6

		_V(0.835, 0.482487, 3.92396),  // MFD 4 KEY 7
		_V(0.835, 0.437382, 3.90755),  // MFD 4 KEY 8
		_V(0.835, 0.392277, 3.89113),  // MFD 4 KEY 9
		_V(0.835, 0.347172, 3.87471),  // MFD 4 KEY 10
		_V(0.835, 0.302067, 3.8583),   // MFD 4 KEY 11
		_V(0.835, 0.256961, 3.84188)   // MFD 4 KEY 12
	};

	for (int key = 0; key < 48; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::MFD_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::MFD_KEYS << 16) | (key & 0xFFFF), mfdKeyPositions[key], 0.01);
	}

	static const VECTOR3 mfdCtrlPositions[12] =
	{
		_V(-0.78, 0.205278, 3.82307),     // MFD 1 PWR
		_V(-0.731021, 0.205278, 3.82307), // MFD 1 SEL
		_V(-0.54, 0.205278, 10.7885),     // MFD 1 MNU

		_V(-0.34, 0.205278, 3.82307),     // MFD 2 PWR
		_V(-0.291021, 0.205278, 3.82307), // MFD 2 SEL
		_V(-0.1, 0.205278, 3.82307),      // MFD 2 MNU

		_V(0.1, 0.205278, 3.82307),       // MFD 3 PWR
		_V(0.148979, 0.205278, 3.82307),  // MFD 3 SEL
		_V(0.34, 0.205278, 3.82307),      // MFD 3 MNU

		_V(0.54, 0.205278, 3.82307),      // MFD 4 PWR
		_V(0.588979, 0.205278, 3.82307),  // MFD 4 SEL
		_V(0.78, 0.205278, 3.82307)       // MFD 4 MNU
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::MFD_CTRLS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::MFD_CTRLS << 16) | (key & 0xFFFF), mfdCtrlPositions[key], 0.01);
	}

	static const VECTOR3 hudKeyPos[12] =
	{
		_V(-0.483529, 0.52265, 3.93869), // LEFT OFF
		_V(-0.465937, 0.52265, 3.93869), // LEFT SRF
		_V(-0.449048, 0.52265, 3.93869), // LEFT ORB
		_V(-0.431456, 0.52265, 3.93869), // LEFT DCK
		_V(-0.414,    0.52265, 3.93869), // LEFT INF
		_V(-0.396471, 0.52265, 3.93869), // LEFT CRG

		_V(0.483529, 0.52265, 3.93869),  // RIGHT OFF
		_V(0.465937, 0.52265, 3.93869),  // RIGHT SRF
		_V(0.449048, 0.52265, 3.93869),  // RIGHT ORB
		_V(0.431456, 0.52265, 3.93869),  // RIGHT DCK
		_V(0.414,    0.52265, 3.93869),	 // RIGHT INF
		_V(0.396471, 0.52265, 3.93869)   // RIGHT CRG
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::HUD_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::HUD_KEYS << 16) | (key & 0xFFFF), hudKeyPos[key], 0.01);
	}

	static const VECTOR3 rcsKeyPos[6] =
	{
		_V(-0.4648, 0.066157, 3.77233), // LEFT OFF
		_V(-0.4398, 0.066157, 3.77233), // LEFT ROT
		_V(-0.4148, 0.066157, 3.77233), // LEFT LIN

		_V(0.4648, 0.066157, 3.77233),  // RIGHT OFF
		_V(0.4398, 0.066157, 3.77233),  // RIGHT ROT
		_V(0.4148, 0.066157, 3.77233)   // RIGHT LIN
	};

	for (int key = 0; key < 6; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::RCS_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::RCS_KEYS << 16) | (key & 0xFFFF), rcsKeyPos[key], 0.01);
	}

	static const VECTOR3 navKeyPos[12] =
	{
		_V(-0.4648, 0.126185, 3.79424), // LEFT KILL ROT
		_V(-0.4648, 0.101188, 3.78503), // LEFT HOR LVL
		_V(-0.4398, 0.126185, 3.79424), // LEFT PRO GRD
		_V(-0.4398, 0.101188, 3.78503), // LEFT RET GRD
		_V(-0.4148, 0.126185, 3.79424), // LEFT NML +
		_V(-0.4148, 0.101188, 3.78503), // LEFT NML -

		_V(0.4648, 0.126185, 3.79424),  // RIGHT KILL ROT
		_V(0.4648, 0.101188, 3.78503),  // RIGHT HOR LVL
		_V(0.4398, 0.126185, 3.79424),  // RIGHT PRO GRD
		_V(0.4398, 0.101188, 3.78503),  // RIGHT RET GRD
		_V(0.4148, 0.126185, 3.79424),  // RIGHT NML +
		_V(0.4148, 0.101188, 3.78503)   // RIGHT NML -
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::NAV_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::NAV_KEYS << 16) | (key & 0xFFFF), navKeyPos[key], 0.01);
	}

	SURFHANDLE mfdTex = oapiGetTextureHandle(vcMesh, 10);
	SURFHANDLE systemTex = oapiGetTextureHandle(vcMesh, 9);

	oapiVCRegisterArea(VCArea::MFD_KEYS, _R(0, 0, 512, 512), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, mfdTex);
	oapiVCRegisterArea(VCArea::HUD_KEYS, _R(512, 261, 1024, 345), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);
	oapiVCRegisterArea(VCArea::RCS_KEYS, _R(512, 429, 765, 512), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);
	oapiVCRegisterArea(VCArea::NAV_KEYS, _R(512, 345, 1018, 429), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);

	return true;
}

void Lander::clbkPostCreation()
{
	SetRCSConfig();
	mainFuel.full = GetFuelMass() == DL_FUELMASS;

	xrSound = XRSound::CreateInstance(this);
	xrSound->LoadWav(XRSound::HoverEngines, "XRSound/Deepstar/Engine.wav", XRSound::BothViewFar);

	xrSound->LoadWav(SoundID::HYDRAULICS, "XRSound/Default/Hydraulics1.wav", XRSound::BothViewClose);
	xrSound->LoadWav(SoundID::CLICK, "XRSound/Default/SwitchOn1.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::HATCH_OPEN, "XRSound/Default/Hatch Open.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::HATCH_CLOSED, "XRSound/Default/Hatch Closed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::RCS_NORMAL, "XRSound/Default/RCS Config Normal.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::RCS_DOCKING, "XRSound/Default/RCS Config Docking.wav", XRSound::InternalOnly);

	xrSound->LoadWav(SoundID::MAIN_FULL, "XRSound/Default/Main Fuel Tanks Full.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::MAIN_LOW, "XRSound/Default/Warning Main Fuel Low.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::MAIN_DEPLETED, "XRSound/Default/Warning Main Fuel Depleted.wav", XRSound::InternalOnly);

	xrSound->LoadWav(SoundID::SLOT_EMPTY, "XRSound/Default/Slot is Empty.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::SLOT_OCCUPIED, "XRSound/Default/Slot Is Full.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_RELEASED, "XRSound/Default/Cargo Deployed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_RELEASE_FAILED, "XRSound/Default/Cargo Deployment Failed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_GRAPPLED, "XRSound/Default/Cargo Latched In Bay.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_GRAPPLE_NORANGE, "XRSound/Default/No Cargo in Grapple Range.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_GRAPPLE_FAILED, "XRSound/Default/Auto-Grapple Failed.wav", XRSound::InternalOnly);
}

void Lander::clbkMFDMode(int mfd, int mode) { oapiVCTriggerRedrawArea(-1, VCArea::MFD_KEYS); }

void Lander::clbkHUDMode(int mode) { oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS); }

void Lander::clbkRCSMode(int mode) { oapiVCTriggerRedrawArea(-1, VCArea::RCS_KEYS); }

void Lander::clbkNavMode(int mode, bool active) 
{ 
	if (active && rcsConfig) DeactivateNavmode(mode);

	oapiVCTriggerRedrawArea(-1, VCArea::NAV_KEYS);
}

bool Lander::clbkVCRedrawEvent(int id, int ev, SURFHANDLE surf)
{
	oapi::Sketchpad* sp = oapiGetSketchpad(surf);

	if (!sp) return false;

	sp->SetTextAlign(oapi::Sketchpad::CENTER, oapi::Sketchpad::BASELINE);

	switch (id)
	{
	case VCArea::MFD_KEYS:
	{
		sp->SetFont(mfdFont);
		sp->SetTextColor(0x21DDA7);

		int x = 0;

		for (int mfd = 0; mfd < 4; mfd++)
		{
			for (int button = 0; button < 6; button++)
			{
				const char* label = oapiMFDButtonLabel(mfd, button);

				if (!label) continue;

				sp->Text(32 + x, 39 + (button * 64), label, strlen(label));
			}

			x += 64;

			for (int button = 6; button < 12; button++)
			{
				const char* label = oapiMFDButtonLabel(mfd, button);

				if (!label) continue;

				sp->Text(32 + x, 39 + ((button - 6) * 64), label, strlen(label));
			}

			x += 64;
		}

		break;
	}
	case VCArea::HUD_KEYS:
	{
		sp->SetFont(systemFont);

		int defaultMode = oapiGetHUDMode();

		if (hudMode != HUDMode::DEFAULT) defaultMode = -1;

		if (defaultMode == HUD_NONE) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(42, 50, "OFF", 3);

		if (defaultMode == HUD_SURFACE) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(126, 50, "SRF", 3);

		if (defaultMode == HUD_ORBIT) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(210, 50, "ORB", 3);

		if (defaultMode == HUD_DOCKING) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(294, 50, "DCK", 3);

		if (hudMode == HUDMode::INFO) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(378, 50, "INF", 3);

		if (hudMode == HUDMode::CARGO) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(462, 50, "CRG", 3);

		break;
	}
	case VCArea::RCS_KEYS:
	{
		sp->SetFont(systemFont);

		int rcsMode = GetAttitudeMode();

		if (rcsMode == RCS_NONE) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(42, 50, "OFF", 3);

		if (rcsMode == RCS_ROT) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(126, 50, "ROT", 3);

		if (rcsMode == RCS_LIN) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(210, 50, "LIN", 3);

		break;
	}
	case VCArea::NAV_KEYS:
		sp->SetFont(systemFont);

		if (GetNavmodeState(NAVMODE_KILLROT)) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(42, 37, "KILL", 4); sp->Text(42, 63, "ROT", 3);

		if (GetNavmodeState(NAVMODE_HLEVEL)) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(126, 37, "HOR", 3); sp->Text(126, 63, "LVL", 3);

		if (GetNavmodeState(NAVMODE_PROGRADE)) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(210, 37, "PRO", 3); sp->Text(210, 63, "GRD", 3);

		if (GetNavmodeState(NAVMODE_RETROGRADE)) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(294, 37, "RET", 3); sp->Text(294, 63, "GRD", 3);

		if (GetNavmodeState(NAVMODE_NORMAL)) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(378, 37, "NML", 3); sp->Text(378, 63, "+", 1);

		if (GetNavmodeState(NAVMODE_ANTINORMAL)) sp->SetTextColor(0x21DDA7);
		else sp->SetTextColor(0x428F78);

		sp->Text(462, 37, "NML", 3); sp->Text(462, 63, "-", 1);

		break;
	}

	oapiReleaseSketchpad(sp);

	return true;
}

bool Lander::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
{
	if (rcsConfig) skp->Text(5, hps->H - 30, "RCS DOCKING", 11);
	else if (mode == HUD_DOCKING || hudMode == HUDMode::INFO) skp->Text(5, hps->H - 30, "RCS NORMAL", 10);

	if (hudMode == HUDMode::DEFAULT) { VESSEL4::clbkDrawHUD(mode, hps, skp); return true; }

	// Determine the screen ratio
	int s = hps->H;
	double d = s * 0.00130208;
	int sw = hps->W;
	int lw = static_cast<int>(16 * sw / 1024);
	int x = 0;
	if (s / sw < 0.7284) x = (lw * 10) + 10;
	int y = static_cast<int>((168 * d) + (-88 * d));

	if (hudMode == HUDMode::INFO) DrawInfoHUD(x, y, skp);
	else DrawCargoHUD(x, y, skp);

	return true;
}

void Lander::DrawInfoHUD(int x, int y, oapi::Sketchpad* skp)
{
	skp->Text(x, y, "Ctrl + Y = Toggle the egress hatch", 34);
	y += 20;

	skp->Text(x, y, "Alt + J = Toggle the RCS config", 31);
	y += 20;

	skp->Text(x, y, "Shift + C = Toggle the cargo mode", 33);

	y += 35;

	const char* status;

	switch (hatchSystem.status)
	{
	case SystemStatus::CLOSED:
		status = "Closed";
		break;
	case SystemStatus::OPENING:
		status = "Opening";
		break;
	case SystemStatus::OPEN:
		status = "Open";
		break;
	case SystemStatus::CLOSING:
		status = "Closing";
		break;
	}

	sprintf(buffer, "Egress hatch: %s", status);
	skp->Text(x, y, buffer, strlen(buffer));
}

void Lander::DrawCargoHUD(int x, int y, oapi::Sketchpad* skp)
{
	sprintf(buffer, "Selected slot: %d", cargoData.selectedSlot);
	skp->Text(x, y, buffer, strlen(buffer));
	y += 20;

	sprintf(buffer, "Selected cargo to add: %s", ucso->GetAvailableCargoName(cargoData.selectedCargo));
	skp->Text(x, y, buffer, strlen(buffer));
	y += 35;

	skp->Text(x, y, "Up Arrow = Select the previous slot", 35);
	y += 20;

	skp->Text(x, y, "Down Arrow = Select the next slot", 33);
	y += 20;

	skp->Text(x, y, "Left Arrow = Select the previous cargo", 38);
	y += 20;

	skp->Text(x, y, "Right Arrow = Select the next cargo", 35);
	y += 25;

	skp->Text(x, y, "Shift + A = Add the selected cargo", 34);
	y += 20;

	skp->Text(x, y, "Shift + G = Grapple the nearest cargo", 37);
	y += 20;

	skp->Text(x, y, "Shift + R = Release the grappled cargo", 38);
	y += 20;

	skp->Text(x, y, "Shift + P = Pack the nearest cargo", 34);
	y += 20;

	skp->Text(x, y, "Shift + U = Unpack the nearest cargo", 36);
	y += 20;

	skp->Text(x, y, "Shift + F = Drain the nearest fuel resource", 43);
	y += 20;

	skp->Text(x, y, "Shift + D = Delete the grappled cargo", 37);

	if (cargoData.timer < 5) { y += 35; skp->Text(x, y, cargoData.message, strlen(cargoData.message)); }

	y += 35;

	UCSO::Vessel::CargoInfo cargoInfo = ucso->GetCargoInfo(cargoData.selectedSlot);
	if (!cargoInfo.valid)
	{
		skp->Text(x, y, "The selected slot is empty.", 27);
		return;
	}

	skp->Text(x, y, "Grappled cargo information", 26);
	y += 20;

	sprintf(buffer, "Name: %s", cargoInfo.name);
	skp->Text(x, y, buffer, strlen(buffer));
	y += 20;

	sprintf(buffer, "Mass: %gkg", cargoInfo.mass);
	skp->Text(x, y, buffer, strlen(buffer));
	y += 20;

	switch (cargoInfo.type)
	{
	case UCSO::Vessel::STATIC:
		skp->Text(x, y, "Type: Static", 12);
		break;

	case UCSO::Vessel::RESOURCE:
		skp->Text(x, y, "Type: Resource", 14);
		y += 20;

		sprintf(buffer, "Resource : %s", cargoInfo.resource);
		skp->Text(x, y, buffer, strlen(buffer));
		y += 20;

		sprintf(buffer, "Resource mass: %gkg", cargoInfo.resourceMass);
		skp->Text(x, y, buffer, strlen(buffer));
		break;

	case UCSO::Vessel::UNPACKABLE_ONLY:
		skp->Text(x, y, "Type: Unpackable only", 21);
		y += 20;

		sprintf(buffer, "Unpacked spawn count: %d cargo(es)", cargoInfo.spawnCount);
		skp->Text(x, y, buffer, strlen(buffer));

	case UCSO::Vessel::PACKABLE_UNPACKABLE:
		if (cargoInfo.type == UCSO::Vessel::PACKABLE_UNPACKABLE) skp->Text(x, y, "Type: Packable and unpackable", 29);
		y += 20;

		switch (cargoInfo.unpackingType)
		{
		case UCSO::Vessel::UCSO_RESOURCE:
			skp->Text(x, y, "Unpacking type: UCSO Resource", 29);
			y += 20;
			break;

		case UCSO::Vessel::UCSO_MODULE:
			skp->Text(x, y, "Unpacking type: UCSO Module", 27);
			y += 20;

			sprintf(buffer, "Breathable: %s", cargoInfo.breathable ? "Yes" : "No");
			skp->Text(x, y, buffer, strlen(buffer));
			break;

		case UCSO::Vessel::ORBITER_VESSEL:
			skp->Text(x, y, "Unpacking type: Orbiter vessel", 30);
			y += 20;

			sprintf(buffer, "Spawn module: %s", cargoInfo.spawnModule);
			skp->Text(x, y, buffer, strlen(buffer));
			y += 20;

			switch (cargoInfo.unpackingMode)
			{
			case UCSO::Vessel::LANDING:
				skp->Text(x, y, "Unpacking mode: Landing", 23);
				break;

			case UCSO::Vessel::DELAYING:
				skp->Text(x, y, "Unpacking mode: Delaying", 24);
				y += 20;

				sprintf(buffer, "Unpacking delay: %is", cargoInfo.unpackingDelay);
				skp->Text(x, y, buffer, strlen(buffer));
				break;

			case UCSO::Vessel::MANUAL:
				skp->Text(x, y, "Unpacking mode: Manual", 22);
				break;
			}

			break;
		case UCSO::Vessel::CUSTOM_CARGO:
			skp->Text(x, y, "Unpacking type: Custom cargo", 28);
			y += 20;

			sprintf(buffer, "Breathable: %s", cargoInfo.breathable ? "Yes" : "No");
			skp->Text(x, y, buffer, strlen(buffer));
			break;
		}

		break;
	}
}

void Lander::clbkPostStep(double simt, double simdt, double mjd)
{
	double fuelMass = GetFuelMass();

	// Main fuel full announcment
	if (fuelMass == DL_FUELMASS)
	{
		// This is to run it once
		if (!mainFuel.full)
		{
			xrSound->PlayWav(SoundID::MAIN_FULL);
			mainFuel.full = true;
		}
	}
	// Reset it to run it again if it became full again
	else if (mainFuel.full) mainFuel.full = false;

	// Main fuel low warning if fuel is less than 5% but greater than 0 (so as not to run it if the fuel is depleted)
	if (fuelMass <= 1.5e3 && fuelMass > 0.001)
	{
		if (!mainFuel.low)
		{
			xrSound->PlayWav(SoundID::MAIN_LOW);
			mainFuel.low = true;
		}
	}
	else if (mainFuel.low) mainFuel.low = false;

	// Main fuel depleted warning
	if (fuelMass == 0)
	{
		if (!mainFuel.depleted)
		{
			xrSound->PlayWav(SoundID::MAIN_DEPLETED);
			mainFuel.depleted = true;
		}
	}
	else if (mainFuel.depleted) mainFuel.depleted = false;

	if (hatchSystem.status == SystemStatus::OPENING || hatchSystem.status == SystemStatus::CLOSING)
	{
		xrSound->PlayWav(SoundID::HYDRAULICS);

		double da = simdt * hatchSystem.speed;

		if (hatchSystem.status == SystemStatus::CLOSING)
		{
			if (hatchSystem.proc > 0.0) hatchSystem.proc = max(0.0, hatchSystem.proc - da);
			else 
			{ 
				hatchSystem.status = SystemStatus::CLOSED;

				xrSound->StopWav(SoundID::HYDRAULICS);
				xrSound->PlayWav(SoundID::HATCH_CLOSED);
			}
		}
		else
		{
			if (hatchSystem.proc < 1.0) hatchSystem.proc = min(1.0, hatchSystem.proc + da);
			else 
			{
				hatchSystem.status = SystemStatus::OPEN;

				xrSound->StopWav(SoundID::HYDRAULICS);
				xrSound->PlayWav(SoundID::HATCH_OPEN);
			}
		}

		SetAnimation(hatchSystem.id, hatchSystem.proc);
	}

	if (GroundContact() && !(GetFlightStatus() & 1) && GetGroundspeed() <= 0.2)
	{
		VECTOR3 thrustVector;

		if (!GetThrustVector(thrustVector)) SetStatusLanded(this, 8.35928);
	}

	if (cargoData.timer < 5) cargoData.timer += simdt;
}

int Lander::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
{
	if (!down) return 0;

	if (hudMode == HUDMode::CARGO)
	{
		if (KEYMOD_SHIFT(kstate))
		{
			switch (key)
			{
			// Disable cargo mode
			case OAPI_KEY_C:
				hudMode = HUDMode::DEFAULT;

				oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS);
				return 1;

			case OAPI_KEY_A:
				switch (ucso->AddCargo(cargoData.selectedCargo, cargoData.selectedSlot))
				{
				case UCSO::Vessel::GRAPPLE_SUCCEEDED:
					cargoData.message = "Success: The cargo was added.";
					xrSound->PlayWav(SoundID::CARGO_GRAPPLED);
					break;

				case UCSO::Vessel::GRAPPLE_SLOT_OCCUPIED:
					cargoData.message = "Error: The slot is occupied.";
					xrSound->PlayWav(SoundID::SLOT_OCCUPIED);
					break;

				case UCSO::Vessel::GRAPPLE_FAILED:
					cargoData.message = "Error: Couldn't add the cargo.";
					break;

				default: break;
				}

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_G:
				switch (ucso->GrappleCargo(cargoData.selectedSlot))
				{
				case UCSO::Vessel::GRAPPLE_SUCCEEDED:
					cargoData.message = "Success: The cargo was grappled.";
					xrSound->PlayWav(SoundID::CARGO_GRAPPLED);
					break;

				case UCSO::Vessel::NO_CARGO_IN_RANGE:
					cargoData.message = "Error: No grappleable cargo in range.";
					xrSound->PlayWav(SoundID::CARGO_GRAPPLE_NORANGE);
					break;

				case UCSO::Vessel::GRAPPLE_SLOT_OCCUPIED:
					cargoData.message = "Error: The slot is occupied.";
					xrSound->PlayWav(SoundID::SLOT_OCCUPIED);
					break;

				case UCSO::Vessel::GRAPPLE_FAILED:
					cargoData.message = "Error: Couldn't grapple any cargo.";
					xrSound->PlayWav(SoundID::CARGO_GRAPPLE_FAILED);
					break;

				default: break;
				}

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_R:
				if (!(GetFlightStatus() & 1)) cargoData.message = "Error: The vessel must be landed.";
				else
				{
					switch (ucso->ReleaseCargo(cargoData.selectedSlot))
					{
					case UCSO::Vessel::RELEASE_SUCCEEDED:
						cargoData.message = "Success: The cargo was released.";
						xrSound->PlayWav(SoundID::CARGO_RELEASED);
						break;

					case UCSO::Vessel::NO_EMPTY_POSITION:
						cargoData.message = "Error: No empty position nearby.";
						break;

					case UCSO::Vessel::RELEASE_SLOT_EMPTY:
						cargoData.message = "Error: The slot is empty.";
						xrSound->PlayWav(SoundID::SLOT_EMPTY);
						break;

					case UCSO::Vessel::RELEASE_FAILED:
						cargoData.message = "Error: Couldn't release the cargo.";
						xrSound->PlayWav(SoundID::CARGO_RELEASE_FAILED);
						break;

					default: break;
					}
				}

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_P:
				if (ucso->PackCargo()) cargoData.message = "Success: The nearest cargo was packed.";
				else cargoData.message = "Error: No packable cargo in range.";

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_U:
				if (ucso->UnpackCargo()) cargoData.message = "Success: The nearest cargo was unpacked.";
				else cargoData.message = "Error: No unpackable cargo in range.";

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_F:
			{
				double requiredMass = GetMaxFuelMass() - GetFuelMass();

				// Drain the required mass to fill the tank, by subtracting the maximum mass from the current mass
				double drainedMass = ucso->DrainCargoResource("fuel", requiredMass);
				// If no resource cargo is available, drain from the nearest station or unpacked resource
				if (drainedMass == 0) drainedMass = ucso->DrainStationOrUnpackedResource("fuel", requiredMass);

				if (drainedMass > 0)
				{
					SetFuelMass(GetFuelMass() + drainedMass);
					sprintf(buffer, "Success: %gkg was drained", drainedMass);
					cargoData.message = _strdup(buffer);
				}
				else cargoData.message = "Error: Couldn't drain fuel.";

				cargoData.timer = 0;
				return 1;
			}
			case OAPI_KEY_D:
				switch (ucso->DeleteCargo(cargoData.selectedSlot))
				{
				case UCSO::Vessel::RELEASE_SUCCEEDED:
					cargoData.message = "Success: The cargo was deleted.";
					break;

				case UCSO::Vessel::RELEASE_SLOT_EMPTY:
					cargoData.message = "Error: The slot is empty.";
					xrSound->PlayWav(SoundID::SLOT_EMPTY);
					break;

				case UCSO::Vessel::RELEASE_FAILED:
					cargoData.message = "Error: The deletion failed.";
					break;

				default: break;
				}

				cargoData.timer = 0;
				return 1;

			default: return 0;
			}
		}
		else
		{
			switch (key)
			{
			// Previous slot
			case OAPI_KEY_UP:
				cargoData.selectedSlot > 0 ? cargoData.selectedSlot-- : cargoData.selectedSlot = 7;
				return 1;

			// Next slot
			case OAPI_KEY_DOWN:
				cargoData.selectedSlot < 7 ? cargoData.selectedSlot++ : cargoData.selectedSlot = 0;
				return 1;

			// Previous cargo
			case OAPI_KEY_LEFT:
				cargoData.selectedCargo > 0 ? cargoData.selectedCargo-- : cargoData.selectedCargo = ucso->GetAvailableCargoCount() - 1;
				return 1;

			// Next cargo
			case OAPI_KEY_RIGHT:
				cargoData.selectedCargo < ucso->GetAvailableCargoCount() - 1 ? cargoData.selectedCargo++ : cargoData.selectedCargo = 0;
				return 1;

			default: return 0;
			}
		}
	}

	// Toggle RCS config
	if (KEYMOD_ALT(kstate) && key == OAPI_KEY_J)
	{
		rcsConfig = !rcsConfig;
		SetRCSConfig();

		if (rcsConfig) xrSound->PlayWav(SoundID::RCS_DOCKING);
		else xrSound->PlayWav(SoundID::RCS_NORMAL);

		return 1;
	}

	// Enable cargo mode
	if (KEYMOD_SHIFT(kstate) && key == OAPI_KEY_C)
	{
		hudMode = HUDMode::CARGO;
		if (oapiGetHUDMode() == HUD_NONE || oapiGetHUDMode() == HUD_DOCKING) oapiSetHUDMode(HUD_ORBIT);

		oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS);
		return 1;
	}

	// Toggle egress hatch
	if (KEYMOD_CONTROL(kstate) && key == OAPI_KEY_Y)
	{
		switch (hatchSystem.status)
		{
		case SystemStatus::CLOSED:
			hatchSystem.status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::CLOSING:
			hatchSystem.status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::OPENING:
			hatchSystem.status = SystemStatus::CLOSING;
			return 1;

		case SystemStatus::OPEN:
			hatchSystem.status = SystemStatus::CLOSING;
			return 1;

		default:
			hatchSystem.status = SystemStatus::CLOSED;
			return 1;
		}
	}

	return 0;
}

bool Lander::clbkVCMouseEvent(int id, int ev, VECTOR3& p)
{
	int ctrlSet = HIWORD(id);
	int ctrlID = LOWORD(id);

	switch (ctrlSet)
	{
	case VCCtrlSet::MFD_KEYS:
		oapiProcessMFDButton(ctrlID / 12, ctrlID % 12, ev);

		if (ev == PANEL_MOUSE_LBUP) xrSound->PlayWav(SoundID::CLICK);
		return true;

	case VCCtrlSet::MFD_CTRLS:
		switch (ctrlID % 3)
		{
		// PWR KEY
		case 0: 
			oapiSendMFDKey(ctrlID / 3, OAPI_KEY_ESCAPE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// SEL KEY
		case 1:
			oapiSendMFDKey(ctrlID / 3, OAPI_KEY_F1);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// MNU KEY
		case 2:
			oapiSendMFDKey(ctrlID / 3, OAPI_KEY_GRAVE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	case VCCtrlSet::HUD_KEYS:
		// Default HUD seleceted
		if (ctrlID < 4 || ctrlID > 7) hudMode = HUDMode::DEFAULT;

		switch (ctrlID)
		{
		// OFF KEY
		case 0:
		case 11:
			oapiSetHUDMode(HUD_NONE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// SRF KEY
		case 1:
		case 10:
			oapiSetHUDMode(HUD_SURFACE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// ORB KEY
		case 2:
		case 9:
			oapiSetHUDMode(HUD_ORBIT);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// DCK KEY
		case 3:
		case 8:
			oapiSetHUDMode(HUD_DOCKING);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// INF KEY
		case 4:
		case 7:
			if (hudMode == HUDMode::INFO) return true;

			if (oapiGetHUDMode() == HUD_NONE || oapiGetHUDMode() == HUD_DOCKING) oapiSetHUDMode(HUD_ORBIT);
			hudMode = HUDMode::INFO;

			oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// CRG KEY
		case 5:
		case 6:
			if (hudMode == HUDMode::CARGO) return true;

			if (oapiGetHUDMode() == HUD_NONE || oapiGetHUDMode() == HUD_DOCKING) oapiSetHUDMode(HUD_ORBIT);
			hudMode = HUDMode::CARGO;

			oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	case VCCtrlSet::RCS_KEYS:
		switch (ctrlID)
		{
		// OFF KEY
		case 0: 
		case 5:
			SetAttitudeMode(RCS_NONE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// ROT KEY
		case 1: 
		case 4:
			SetAttitudeMode(RCS_ROT);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// LIN KEY
		case 2: 
		case 3:
			SetAttitudeMode(RCS_LIN);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	case VCCtrlSet::NAV_KEYS:
		switch (ctrlID)
		{
		// KILL ROT KEY
		case 0:
		case 10:
			ToggleNavmode(NAVMODE_KILLROT);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// HOR LVL KEY
		case 2:
		case 8:
			ToggleNavmode(NAVMODE_HLEVEL);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// PRO GRD KEY
		case 4:
		case 6:
			ToggleNavmode(NAVMODE_PROGRADE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// RET GRD KEY
		case 1:
		case 11:
			ToggleNavmode(NAVMODE_RETROGRADE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// NML + KEY
		case 3:
		case 9:
			ToggleNavmode(NAVMODE_NORMAL);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// NML - KEY
		case 5:
		case 7:
			ToggleNavmode(NAVMODE_ANTINORMAL);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	default: return false;
	};
}

void Lander::SetRCSConfig()
{
	if (rcsConfig)
		for (int mode = 1; mode < 8; mode++)
			if (GetNavmodeState(mode)) DeactivateNavmode(mode);

	DelThrusterGroup(THGROUP_ATT_PITCHUP);
	DelThrusterGroup(THGROUP_ATT_PITCHDOWN);
	DelThrusterGroup(THGROUP_ATT_BANKLEFT);
	DelThrusterGroup(THGROUP_ATT_BANKRIGHT);
	DelThrusterGroup(THGROUP_ATT_YAWLEFT);
	DelThrusterGroup(THGROUP_ATT_YAWRIGHT);

	DelThrusterGroup(THGROUP_ATT_UP);
	DelThrusterGroup(THGROUP_ATT_DOWN);
	DelThrusterGroup(THGROUP_ATT_LEFT);
	DelThrusterGroup(THGROUP_ATT_RIGHT);
	DelThrusterGroup(THGROUP_ATT_FORWARD);
	DelThrusterGroup(THGROUP_ATT_BACK);

	THRUSTER_HANDLE groupHandles[4];

	if (rcsConfig)
	{
		for (int thruster = 0; thruster < 8; thruster++) SetThrusterMax0(rcsHandles[thruster], DL_MAXRCSTH * 0.5);

		for (int thruster = 8; thruster < 12; thruster++) SetThrusterMax0(rcsHandles[thruster], DL_MAXRCSTH);

		for (int thruster = 12; thruster < 20; thruster++) SetThrusterMax0(rcsHandles[thruster], DL_MAXRCSTH * 0.5);

		groupHandles[0] = rcsHandles[1];
		groupHandles[1] = rcsHandles[3];
		groupHandles[2] = rcsHandles[4];
		groupHandles[3] = rcsHandles[6];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHDOWN); // Original: THGROUP_ATT_PITCHUP

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[2];
		groupHandles[2] = rcsHandles[5];
		groupHandles[3] = rcsHandles[7];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHUP); // Original: THGROUP_ATT_PITCHDOWN

		groupHandles[0] = rcsHandles[2];
		groupHandles[1] = rcsHandles[3];
		groupHandles[2] = rcsHandles[4];
		groupHandles[3] = rcsHandles[5];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_YAWLEFT); // Original: THGROUP_ATT_BANKRIGHT

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[1];
		groupHandles[2] = rcsHandles[6];
		groupHandles[3] = rcsHandles[7];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_YAWRIGHT); // Original: THGROUP_ATT_BANKLEFT

		groupHandles[0] = rcsHandles[8];
		groupHandles[1] = rcsHandles[11];
		CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BANKLEFT); // Original: THGROUP_ATT_YAWRIGHT

		groupHandles[0] = rcsHandles[9];
		groupHandles[1] = rcsHandles[10];
		CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BANKRIGHT); // Original: THGROUP_ATT_YAWLEFT

		groupHandles[0] = rcsHandles[4];
		groupHandles[1] = rcsHandles[5];
		groupHandles[2] = rcsHandles[6];
		groupHandles[3] = rcsHandles[7];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_FORWARD); // Original: THGROUP_ATT_UP

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[1];
		groupHandles[2] = rcsHandles[2];
		groupHandles[3] = rcsHandles[3];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_BACK); // Original: THGROUP_ATT_DOWN

		groupHandles[0] = rcsHandles[8];
		groupHandles[1] = rcsHandles[9];
		CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_LEFT); // Original: THGROUP_ATT_RIGHT

		groupHandles[0] = rcsHandles[10];
		groupHandles[1] = rcsHandles[11];
		CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_RIGHT); // Original: THGROUP_ATT_LEFT

		groupHandles[0] = rcsHandles[16];
		groupHandles[1] = rcsHandles[17];
		groupHandles[2] = rcsHandles[18];
		groupHandles[3] = rcsHandles[19];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_UP); // Original: THGROUP_ATT_FORWARD

		groupHandles[0] = rcsHandles[12];
		groupHandles[1] = rcsHandles[13];
		groupHandles[2] = rcsHandles[14];
		groupHandles[3] = rcsHandles[15];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_DOWN); // Original: THGROUP_ATT_BACK
	}
	else
	{
		for (int thruster = 0; thruster < 8; thruster++) SetThrusterMax0(rcsHandles[thruster], DL_MAXRCSTH);

		for (int thruster = 8; thruster < 12; thruster++) SetThrusterMax0(rcsHandles[thruster], DL_MAXRCSTH * 2);

		for (int thruster = 12; thruster < 20; thruster++) SetThrusterMax0(rcsHandles[thruster], DL_MAXRCSTH);

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

		groupHandles[0] = rcsHandles[16];
		groupHandles[1] = rcsHandles[17];
		groupHandles[2] = rcsHandles[18];
		groupHandles[3] = rcsHandles[19];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_FORWARD);

		groupHandles[0] = rcsHandles[12];
		groupHandles[1] = rcsHandles[13];
		groupHandles[2] = rcsHandles[14];
		groupHandles[3] = rcsHandles[15];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_BACK);
	}
}