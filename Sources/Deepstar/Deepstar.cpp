// ==============================================================
//                 ORBITER MODULE: Deepstar
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Deepstar.cpp
// Control module for Deepstar vessel class
//
// ==============================================================

#include "Deepstar.h"

int Deepstar::vesselCount = 0;
oapi::Font* Deepstar::mfdFont = nullptr;
oapi::Font* Deepstar::systemFont = nullptr;
const int Deepstar::mfdUpdateLineup[4] = { MFD_USER1, MFD_LEFT, MFD_RIGHT, MFD_USER2 };

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Deepstar(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Deepstar*>(vessel); }

Deepstar::Deepstar(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel)
{
	ucso = UCSO::Vessel::CreateInstance(this);
	ucso->SetUnpackingRange(200);

	sprintf(buffer, "UCSO version: %s", ucso->GetUCSOVersion());
	cargoData.message = _strdup(buffer);

	if (!vesselCount)
	{
		mfdFont = oapiCreateFont(24, true, "Arial", FONT_BOLD);
		systemFont = oapiCreateFont(30, true, "Arial", FONT_BOLD);
	}

	vesselCount++;
}

Deepstar::~Deepstar()
{
	delete ucso;

	vesselCount--;

	if (!vesselCount)
	{
		oapiReleaseFont(mfdFont);
		oapiReleaseFont(systemFont);
	}
}

void Deepstar::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DS_SIZE);
	SetEmptyMass(DS_EMPTYMASS);
	SetPMI(DS_PMI);
	SetCrossSections(DS_CS);

	DefineEngines();
	DefineSystems();
	DefineSlots();

	vcMesh = oapiLoadMesh("Deepstar/DeepstarVC");

	SetMeshVisibilityMode(AddMesh("Deepstar/Deepstar"), MESHVIS_EXTERNAL);
	SetMeshVisibilityMode(AddMesh(vcMesh), MESHVIS_VC);

	SetCameraOffset({ 0, 6.04, 132.55 });
}

void Deepstar::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DS_FUELMASS);

	// Main engine
	THRUSTER_HANDLE mainHandle = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DS_MAXMAINTH, propHandle, DS_ISP);;
	CreateThrusterGroup(&mainHandle, 1, THGROUP_MAIN);

	AddExhaust(mainHandle, 1, 3, _V(0, 7.716, -222.66), _V(0, 0, -1));
	AddExhaust(mainHandle, 1, 3, _V(0, -7.716, -222.66), _V(0, 0, -1));
	AddExhaust(mainHandle, 1, 3, _V(7.716, 0, -222.66), _V(0, 0, -1));
	AddExhaust(mainHandle, 1, 3, _V(-7.716, 0, -222.66), _V(0, 0, -1));

	// RCS engines
	THRUSTER_HANDLE groupHandles[3];

	// Up thrusters
	rcsHandles[0] = CreateThruster(_V(0, 11.135, 81.593), _V(0, -1, 0), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[0], 1, 0.2, _V(0, 12.85, 121.962), _V(0, 1, 0));

	rcsHandles[1] = CreateThruster(_V(-22.992, 11.135, -81.593), _V(0, -1, 0), DS_MAXRCSTH * 0.5, propHandle, DS_ISP);
	AddExhaust(rcsHandles[1], 1, 0.2, _V(-22.992, 11.135, -81.593), _V(0, 1, 0));
	AddExhaust(rcsHandles[1], 1, 0.2, _V(-22.992, 11.135, -161.345), _V(0, 1, 0));

	rcsHandles[2] = CreateThruster(_V(22.992, 11.135, -81.593), _V(0, -1, 0), DS_MAXRCSTH * 0.5, propHandle, DS_ISP);
	AddExhaust(rcsHandles[2], 1, 0.2, _V(22.992, 11.135, -81.593), _V(0, 1, 0));
	AddExhaust(rcsHandles[2], 1, 0.2, _V(22.992, 11.135, -161.345), _V(0, 1, 0));

	// Down thrusters
	rcsHandles[3] = CreateThruster(_V(0, -11.135, 81.593), _V(0, 1, 0), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[3], 1, 0.2, _V(0, -12.85, 121.962), _V(0, -1, 0));

	rcsHandles[4] = CreateThruster(_V(-22.992, -11.135, -81.593), _V(0, 1, 0), DS_MAXRCSTH * 0.5, propHandle, DS_ISP);
	AddExhaust(rcsHandles[4], 1, 0.2, _V(-22.992, -11.135, -81.593), _V(0, -1, 0));
	AddExhaust(rcsHandles[4], 1, 0.2, _V(-22.992, -11.135, -161.345), _V(0, -1, 0));

	rcsHandles[5] = CreateThruster(_V(22.992, -11.135, -81.593), _V(0, 1, 0), DS_MAXRCSTH * 0.5, propHandle, DS_ISP);
	AddExhaust(rcsHandles[5], 1, 0.2, _V(22.992, -11.135, -81.593), _V(0, -1, 0));
	AddExhaust(rcsHandles[5], 1, 0.2, _V(22.992, -11.135, -161.345), _V(0, -1, 0));

	// Right thrusters
	rcsHandles[6] = CreateThruster(_V(-12.85, 0, 81.593), _V(1, 0, 0), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[6], 1, 0.2, _V(-12.85, 0, 121.962), _V(-1, 0, 0));

	rcsHandles[7] = CreateThruster(_V(-12.85, 0, -81.593), _V(1, 0, 0), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[7], 1, 0.2, _V(-34.084, 0, -81.593), _V(-1, 0, 0));
	AddExhaust(rcsHandles[7], 1, 0.2, _V(-34.084, 0, -161.345), _V(-1, 0, 0));

	// Left thrusters
	rcsHandles[8] = CreateThruster(_V(12.85, 0, 81.593), _V(-1, 0, 0), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[8], 1, 0.2, _V(12.85, 0, 121.962), _V(1, 0, 0));

	rcsHandles[9] = CreateThruster(_V(12.85, 0, -81.593), _V(-1, 0, 0), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[9], 1, 0.2, _V(34.084, 0, -81.593), _V(1, 0, 0));
	AddExhaust(rcsHandles[9], 1, 0.2, _V(34.084, 0, -161.345), _V(1, 0, 0));

	// Forward thrusters
	rcsHandles[10] = CreateThruster(_V(-22.992, 0, -67.302), _V(0, 0, -1), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[10], 1, 0.2, _V(-22.992, 0, -67.302), _V(0, 0, 1));

	rcsHandles[11] = CreateThruster(_V(22.992, 0, -67.302), _V(0, 0, -1), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[11], 1, 0.2, _V(22.992, 0, -67.302), _V(0, 0, 1));

	// Aft thrusters
	rcsHandles[12] = CreateThruster(_V(-22.992, 0, -175.626), _V(0, 0, 1), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[12], 1, 0.2, _V(-22.992, 0, -175.626), _V(0, 0, -1));

	rcsHandles[13] = CreateThruster(_V(22.992, 0, -175.626), _V(0, 0, 1), DS_MAXRCSTH, propHandle, DS_ISP);
	AddExhaust(rcsHandles[13], 1, 0.2, _V(22.992, 0, -175.626), _V(0, 0, -1));

	groupHandles[0] = rcsHandles[1];
	groupHandles[1] = rcsHandles[2];
	groupHandles[2] = rcsHandles[3];
	CreateThrusterGroup(groupHandles, 3, THGROUP_ATT_PITCHUP);

	groupHandles[0] = rcsHandles[0];
	groupHandles[1] = rcsHandles[4];
	groupHandles[2] = rcsHandles[5];
	CreateThrusterGroup(groupHandles, 3, THGROUP_ATT_PITCHDOWN);

	groupHandles[0] = rcsHandles[2];
	groupHandles[1] = rcsHandles[4];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BANKRIGHT);

	groupHandles[0] = rcsHandles[1];
	groupHandles[1] = rcsHandles[5];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BANKLEFT);

	groupHandles[0] = rcsHandles[6];
	groupHandles[1] = rcsHandles[9];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_YAWRIGHT);

	groupHandles[0] = rcsHandles[7];
	groupHandles[1] = rcsHandles[8];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_YAWLEFT);

	groupHandles[0] = rcsHandles[3];
	groupHandles[1] = rcsHandles[4];
	groupHandles[2] = rcsHandles[5];
	CreateThrusterGroup(groupHandles, 3, THGROUP_ATT_UP);

	groupHandles[0] = rcsHandles[0];
	groupHandles[1] = rcsHandles[1];
	groupHandles[2] = rcsHandles[2];
	CreateThrusterGroup(groupHandles, 3, THGROUP_ATT_DOWN);

	groupHandles[0] = rcsHandles[6];
	groupHandles[1] = rcsHandles[7];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_RIGHT);

	groupHandles[0] = rcsHandles[8];
	groupHandles[1] = rcsHandles[9];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_LEFT);

	groupHandles[0] = rcsHandles[12];
	groupHandles[1] = rcsHandles[13];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_FORWARD);

	groupHandles[0] = rcsHandles[10];
	groupHandles[1] = rcsHandles[11];
	CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_BACK);
}

void Deepstar::DefineSystems()
{
	// Transponder and docking ports
	EnableTransponder(true);
	SetTransponderChannel(520);

	DOCKHANDLE dockPorts[4];

	dockPorts[0] = CreateDock({ 0, 4.225, 0 }, { 0, 1, 0 }, { 0, 0, 1 });
	dockPorts[1] = CreateDock({ 0, -4.225, 0 }, { 0, -1, 0 }, { 0, 0, 1 });
	dockPorts[2] = CreateDock({ -4.225, 0, 0 }, { -1, 0, 0 }, { 0, 1, 0 });
	dockPorts[3] = CreateDock({ 4.225, 0, 0 }, { 1, 0, 0 }, { 0, 1, 0 });

	for (auto& dockPort : dockPorts) EnableIDS(dockPort, true);

	SetIDSChannel(dockPorts[0], 522);
	SetIDSChannel(dockPorts[1], 524);
	SetIDSChannel(dockPorts[2], 526);
	SetIDSChannel(dockPorts[3], 528);

	noseDock = CreateDock({ 0, 0, 136.267 }, { 0, 0, 1 }, { 0, 1, 0 });
	EnableIDS(noseDock, true);
	SetIDSChannel(noseDock, 530);

	dockLight = static_cast<SpotLight*>(AddSpotLight(_V(0, 8, 134.5), _V(0, 0, 1), 150, 1e-3, 0, 1e-3, RAD * 30, RAD * 60, { 1,1,1,0 }, { 1,1,1,0 }, { 0,0,0,0 }));
	dockLight->Activate(false);

	// Port dish
	systems[SystemID::PORT_DISH].infoName = "Port dish";
	systems[SystemID::PORT_DISH].scnName = "PORT_DISH";

	systems[SystemID::PORT_DISH].id = CreateAnimation(0);
	systems[SystemID::PORT_DISH].status = SystemStatus::CLOSED;
	systems[SystemID::PORT_DISH].proc = 0;
	systems[SystemID::PORT_DISH].speed = 0.05;
	systems[SystemID::PORT_DISH].repeat = true;

	static UINT PortGrp[3] = { 28,29,30 };
	static MGROUP_ROTATE PortRot(0, PortGrp, 3, _V(-70.541, 4.943, -28.524), _V(0, 1, 0), (float)(2 * PI));
	AddAnimationComponent(systems[SystemID::PORT_DISH].id, 0, 1, &PortRot);

	// Starboard dish
	systems[SystemID::STRB_DISH].infoName = "Starboard dish";
	systems[SystemID::STRB_DISH].scnName = "STRB_DISH";

	systems[SystemID::STRB_DISH].id = CreateAnimation(0);
	systems[SystemID::STRB_DISH].status = SystemStatus::CLOSED;
	systems[SystemID::STRB_DISH].proc = 0;
	systems[SystemID::STRB_DISH].speed = 0.05;
	systems[SystemID::STRB_DISH].repeat = true;

	static UINT StrbGrp[3] = { 31,32,33 };
	static MGROUP_ROTATE StrbRot(0, StrbGrp, 3, _V(70.541, 4.943, -28.524), _V(0, 1, 0), (float)(2 * PI));
	AddAnimationComponent(systems[SystemID::STRB_DISH].id, 0, 1, &StrbRot);

	// Spin gravity
	systems[SystemID::SPIN_GRAVITY].infoName = "Spin gravity";
	systems[SystemID::SPIN_GRAVITY].scnName = "SPIN_GRAVITY";

	systems[SystemID::SPIN_GRAVITY].id = CreateAnimation(0);
	systems[SystemID::SPIN_GRAVITY].status = SystemStatus::CLOSED;
	systems[SystemID::SPIN_GRAVITY].proc = 0;
	systems[SystemID::SPIN_GRAVITY].speed = 0.0331;
	systems[SystemID::SPIN_GRAVITY].repeat = true;

	static UINT SpinGrp[8] = { 0,1,2,12,13,14,15,18 };
	static MGROUP_ROTATE SpinRot(0, SpinGrp, 8, _V(0, 0, 0), _V(0, 0, 1), (float)(2 * PI));
	AddAnimationComponent(systems[SystemID::SPIN_GRAVITY].id, 0, 1, &SpinRot);

	// Nose docking port doors
	systems[SystemID::NOSE_DOCK_DOORS].infoName = "Nose docking port doors";
	systems[SystemID::NOSE_DOCK_DOORS].scnName = "NOSE_DOCK_DOORS";

	systems[SystemID::NOSE_DOCK_DOORS].id = CreateAnimation(0);
	systems[SystemID::NOSE_DOCK_DOORS].status = SystemStatus::CLOSED;
	systems[SystemID::NOSE_DOCK_DOORS].proc = 0;
	systems[SystemID::NOSE_DOCK_DOORS].speed = 0.067;
	systems[SystemID::NOSE_DOCK_DOORS].repeat = false;

	static UINT NoseRot1Grp[1] = { 5 };
	static MGROUP_ROTATE NoseRot1(0, NoseRot1Grp, 1, _V(0.995, 0, 135.426), _V(0, 1, 0), (float)(160 * RAD));
	AddAnimationComponent(systems[SystemID::NOSE_DOCK_DOORS].id, 0, 0.3, &NoseRot1);

	static UINT NoseRot2Grp[1] = { 4 };
	static MGROUP_ROTATE NoseRot2(0, NoseRot2Grp, 1, _V(-0.995, 0, 135.426), _V(0, 1, 0), (float)(-160 * RAD));
	AddAnimationComponent(systems[SystemID::NOSE_DOCK_DOORS].id, 0, 0.3, &NoseRot2);

	static MGROUP_TRANSLATE NoseTrans1(0, NoseRot1Grp, 1, _V(0.04, 0, 0));
	AddAnimationComponent(systems[SystemID::NOSE_DOCK_DOORS].id, 0.3, 0.4, &NoseTrans1);

	static MGROUP_TRANSLATE NoseTrans2(0, NoseRot2Grp, 1, _V(-0.04, 0, 0));
	AddAnimationComponent(systems[SystemID::NOSE_DOCK_DOORS].id, 0.3, 0.4, &NoseTrans2);

	static UINT NoseTrans3Grp[3] = { 9,10,11 };
	static MGROUP_TRANSLATE NoseTrans3(0, NoseTrans3Grp, 3, _V(0, 0, 1));
	AddAnimationComponent(systems[SystemID::NOSE_DOCK_DOORS].id, 0.3, 1, &NoseTrans3);
}

void Deepstar::DefineSlots()
{
	ucso->SetSlotAttachment(0, CreateAttachment(false, { 2.3, 0, 18 }, { 1, 0, 0 }, { 0, 0, 1 }, "Starboard Probe"));
	ucso->SetSlotAttachment(1, CreateAttachment(false, { -2.3, 0, 18 }, { -1, 0, 0 }, { 0, 0, 1 }, "Port Probe"));

	int attachSlot = 2;

	// Starboard slots
	for (int slotSection = 0; slotSection < 9; slotSection++)
	{
		if (slotSection == 3) continue;

		double yValue = 66.8311 - (slotSection * 13.2491) - (slotSection >= 5 ? 4.01059 : 0);

		for (int slot = 0; slot < 9; slot++)
		{
			sprintf(buffer, "Starboard Slot %d", attachSlot - 1);

			ucso->SetSlotAttachment(attachSlot, CreateAttachment(false, { 1.5047, 0, yValue - (slot * 1.3) }, { 1, 0, 0 }, { 0, -1, 0 }, buffer));
			attachSlot++;
		}
	}

	// Port slots
	for (int slotSection = 0; slotSection < 9; slotSection++)
	{
		if (slotSection == 3) continue;

		double yValue = 66.8311 - (slotSection * 13.2491) - (slotSection >= 5 ? 4.01059 : 0);

		for (int slot = 0; slot < 9; slot++)
		{
			sprintf(buffer, "Port Slot %d", attachSlot - 73);

			ucso->SetSlotAttachment(attachSlot, CreateAttachment(false, { -1.5047, 0, yValue - (slot * 1.3) }, { -1, 0, 0 }, { 0, -1, 0 }, buffer));
			attachSlot++;
		}
	}
}

void Deepstar::clbkLoadStateEx(FILEHANDLE scn, void* status)
{
	char* line;

	while (oapiReadScenario_nextline(scn, line))
	{
		if (!_strnicmp(line, "RCS_CONFIG", 10))
		{
			int rcsConfig;
			sscanf(line + 10, "%d", &rcsConfig);

			this->rcsConfig = rcsConfig;
		}

		else if (!_strnicmp(line, "DOCK_LIGHT", 10))
		{
			int active;
			sscanf(line + 10, "%d", &active);

			dockLight->Activate(active);
		}

		else
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
}

void Deepstar::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	for (auto& system : systems)
	{
		sprintf(buffer, "%d %0.4f", system.status, system.proc);
		oapiWriteScenario_string(scn, system.scnName, buffer);
	}

	oapiWriteScenario_int(scn, "RCS_CONFIG", rcsConfig);
	oapiWriteScenario_int(scn, "DOCK_LIGHT", dockLight->IsActive());
}

bool Deepstar::clbkLoadVC(int id)
{
	static VCHUDSPEC hud = { 1, 20, { 0, 6.067, 133.541 }, 1 };
	oapiVCRegisterHUD(&hud);

	static VCMFDSPEC mfd1 = { 1, 15 };
	static VCMFDSPEC mfd2 = { 1, 13 };
	static VCMFDSPEC mfd3 = { 1, 12 };
	static VCMFDSPEC mfd4 = { 1, 16 };

	oapiVCRegisterMFD(MFD_USER1, &mfd1);
	oapiVCRegisterMFD(MFD_LEFT, &mfd2);
	oapiVCRegisterMFD(MFD_RIGHT, &mfd3);
	oapiVCRegisterMFD(MFD_USER2, &mfd4);

	static const VECTOR3 mfdKeyPos[48] =
	{
		_V(-0.835, 5.90316, 133.495), // MFD 1 KEY 1
		_V(-0.835, 5.85806, 133.479), // MFD 1 KEY 2
		_V(-0.835, 5.81295, 133.461), // MFD 1 KEY 3
		_V(-0.835, 5.76785, 133.445), // MFD 1 KEY 4
		_V(-0.835, 5.72274, 133.429), // MFD 1 KEY 5
		_V(-0.835, 5.67764, 133.413), // MFD 1 KEY 6

		_V(-0.485, 5.90316, 133.495), // MFD 1 KEY 7
		_V(-0.485, 5.85806, 133.479), // MFD 1 KEY 8
		_V(-0.485, 5.81295, 133.461), // MFD 1 KEY 9
		_V(-0.485, 5.76785, 133.445), // MFD 1 KEY 10
		_V(-0.485, 5.72274, 133.429), // MFD 1 KEY 11
		_V(-0.485, 5.67764, 133.413), // MFD 1 KEY 12

		_V(-0.395, 5.90316, 133.495), // MFD 2 KEY 1
		_V(-0.395, 5.85806, 133.479), // MFD 2 KEY 2
		_V(-0.395, 5.81295, 133.461), // MFD 2 KEY 3
		_V(-0.395, 5.76785, 133.445), // MFD 2 KEY 4
		_V(-0.395, 5.72274, 133.429), // MFD 2 KEY 5
		_V(-0.395, 5.67764, 133.413), // MFD 2 KEY 6

		_V(-0.045, 5.90316, 133.495), // MFD 2 KEY 7
		_V(-0.045, 5.85806, 133.479), // MFD 2 KEY 8
		_V(-0.045, 5.81295, 133.461), // MFD 2 KEY 9
		_V(-0.045, 5.76785, 133.445), // MFD 2 KEY 10
		_V(-0.045, 5.72274, 133.429), // MFD 2 KEY 11
		_V(-0.045, 5.67764, 133.413), // MFD 2 KEY 12

		_V(0.045, 5.90316, 133.495),  // MFD 3 KEY 1
		_V(0.045, 5.85806, 133.479),  // MFD 3 KEY 2
		_V(0.045, 5.81295, 133.461),  // MFD 3 KEY 3
		_V(0.045, 5.76785, 133.445),  // MFD 3 KEY 4
		_V(0.045, 5.72274, 133.429),  // MFD 3 KEY 5
		_V(0.045, 5.67764, 133.413),  // MFD 3 KEY 6

		_V(0.395, 5.90316, 133.495),  // MFD 3 KEY 7
		_V(0.395, 5.85806, 133.479),  // MFD 3 KEY 8
		_V(0.395, 5.81295, 133.461),  // MFD 3 KEY 9
		_V(0.395, 5.76785, 133.445),  // MFD 3 KEY 10
		_V(0.395, 5.72274, 133.429),  // MFD 3 KEY 11
		_V(0.395, 5.67764, 133.413),  // MFD 3 KEY 12

		_V(0.485, 5.90316, 133.495),  // MFD 4 KEY 1
		_V(0.485, 5.85806, 133.479),  // MFD 4 KEY 2
		_V(0.485, 5.81295, 133.461),  // MFD 4 KEY 3
		_V(0.485, 5.76785, 133.445),  // MFD 4 KEY 4
		_V(0.485, 5.72274, 133.429),  // MFD 4 KEY 5
		_V(0.485, 5.67764, 133.413),  // MFD 4 KEY 6

		_V(0.835, 5.90316, 133.495),  // MFD 4 KEY 7
		_V(0.835, 5.85806, 133.479),  // MFD 4 KEY 8
		_V(0.835, 5.81295, 133.461),  // MFD 4 KEY 9
		_V(0.835, 5.76785, 133.445),  // MFD 4 KEY 10
		_V(0.835, 5.72274, 133.429),  // MFD 4 KEY 11
		_V(0.835, 5.67764, 133.413)   // MFD 4 KEY 12
	};

	for (int key = 0; key < 48; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::MFD_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::MFD_KEYS << 16) | (key & 0xFFFF), mfdKeyPos[key], 0.01);
	}

	static const VECTOR3 mfdCtrlPos[12] =
	{
		_V(-0.78, 5.62595, 133.393),     // MFD 1 PWR
		_V(-0.731021, 5.62595, 133.393), // MFD 1 SEL
		_V(-0.54, 5.62595, 133.393),     // MFD 1 MNU

		_V(-0.34, 5.62595, 133.393),     // MFD 2 PWR
		_V(-0.291021, 5.62595, 133.393), // MFD 2 SEL
		_V(-0.1, 5.62595, 133.393),      // MFD 2 MNU

		_V(0.1, 5.62595, 133.393),       // MFD 3 PWR
		_V(0.148979, 5.62595, 133.393),  // MFD 3 SEL
		_V(0.34, 5.62595, 133.393),      // MFD 3 MNU

		_V(0.54, 5.62595, 133.393),      // MFD 4 PWR
		_V(0.588979, 5.62595, 133.393),  // MFD 4 SEL
		_V(0.78, 5.62595, 133.393)       // MFD 4 MNU
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::MFD_CTRLS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::MFD_CTRLS << 16) | (key & 0xFFFF), mfdCtrlPos[key], 0.01);
	}

	static const VECTOR3 hudKeyPos[6] =
	{
		_V(-0.043529, 5.94331, 133.509), // OFF
		_V(-0.025937, 5.94331, 133.509), // SRF
		_V(-0.009048, 5.94331, 133.509), // ORB
		_V(0.008544, 5.94331, 133.509),  // DCK
		_V(0.026,    5.94331, 133.509),  // INF
		_V(0.043529, 5.94331, 133.509)   // CRG
	};

	for (int key = 0; key < 6; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::HUD_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::HUD_KEYS << 16) | (key & 0xFFFF), hudKeyPos[key], 0.01);
	}

	static const VECTOR3 rcsKeyPos[3] =
	{
		_V(-0.025, 5.48683, 133.344), // OFF
		_V(0, 5.48683, 133.344),      // ROT
		_V(0.025, 5.48683, 133.344)   // LIN
	};

	for (int key = 0; key < 3; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::RCS_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::RCS_KEYS << 16) | (key & 0xFFFF), rcsKeyPos[key], 0.01);
	}

	static const VECTOR3 navKeyPos[6] =
	{
		_V(-0.025, 5.54686, 133.365), // KILL ROT
		_V(-0.025, 5.52186, 133.355), // HOR LVL
		_V(0, 5.54686, 133.365),      // PRO GRD
		_V(0, 5.52186, 133.355),      // RET GRD
		_V(0.025, 5.54686, 133.365),  // NML +
		_V(0.025, 5.52186, 133.355)   // NML -
	};

	for (int key = 0; key < 6; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::NAV_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::NAV_KEYS << 16) | (key & 0xFFFF), navKeyPos[key], 0.01);
	}

	SURFHANDLE mfdTex = oapiGetTextureHandle(vcMesh, 13);
	SURFHANDLE systemTex = oapiGetTextureHandle(vcMesh, 12);

	oapiVCRegisterArea(VCArea::MFD_KEYS, _R(0, 0, 512, 512), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, mfdTex);
	oapiVCRegisterArea(VCArea::HUD_KEYS, _R(512, 261, 1024, 345), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);
	oapiVCRegisterArea(VCArea::RCS_KEYS, _R(512, 429, 765, 512), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);
	oapiVCRegisterArea(VCArea::NAV_KEYS, _R(512, 345, 1018, 429), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);

	return true;
}

void Deepstar::clbkPostCreation()
{
	if (rcsConfig) SetRCSConfig();
	mainFuel.full = GetFuelMass() == DS_FUELMASS;

	xrSound = XRSound::CreateInstance(this);
	xrSound->LoadWav(XRSound::MainEngines, "XRSound/Deepstar/Engine.wav", XRSound::BothViewFar);

	xrSound->LoadWav(SoundID::HYDRAULICS, "XRSound/Default/Hydraulics1.wav", XRSound::BothViewClose);
	xrSound->LoadWav(SoundID::CLICK, "XRSound/Default/SwitchOn1.wav", XRSound::InternalOnly);
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

void Deepstar::clbkMFDMode(int mfd, int mode) { oapiVCTriggerRedrawArea(-1, VCArea::MFD_KEYS); }

void Deepstar::clbkHUDMode(int mode) { oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS); }

void Deepstar::clbkRCSMode(int mode) { oapiVCTriggerRedrawArea(-1, VCArea::RCS_KEYS); }

void Deepstar::clbkNavMode(int mode, bool active) 
{ 
	if (active && rcsConfig) DeactivateNavmode(mode);

	oapiVCTriggerRedrawArea(-1, VCArea::NAV_KEYS);
}

bool Deepstar::clbkVCRedrawEvent(int id, int ev, SURFHANDLE surf)
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
				const char* label = oapiMFDButtonLabel(mfdUpdateLineup[mfd], button);

				if (!label) continue;

				sp->Text(32 + x, 39 + (button * 64), label, strlen(label));
			}

			x += 64;

			for (int button = 6; button < 12; button++)
			{
				const char* label = oapiMFDButtonLabel(mfdUpdateLineup[mfd], button);

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

bool Deepstar::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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

void Deepstar::DrawInfoHUD(int x, int y, oapi::Sketchpad* skp)
{
	skp->Text(x, y, "Ctrl + 1 = Port dish", 20);
	y += 20;

	skp->Text(x, y, "Ctrl + 2 = Starboard dish", 25);
	y += 20;

	skp->Text(x, y, "Ctrl + G = Spin gravity", 23);
	y += 20;

	skp->Text(x, y, "Ctrl + K = Nose docking port doors", 34);
	y += 20;

	skp->Text(x, y, "Ctrl + L = Docking light", 24);
	y += 20;

	skp->Text(x, y, "Alt + J = RCS config", 20);
	y += 20;

	skp->Text(x, y, "Shift + C = Cargo mode", 22);

	y += 35;

	for (auto& system : systems)
	{
		const char* status;

		if (system.repeat)
		{
			if (system.status == SystemStatus::CLOSED) status = "Stopped";
			else status = "Active";
		}
		else
		{
			switch (system.status)
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
		}

		sprintf(buffer, "%s: %s", system.infoName, status);
		skp->Text(x, y, buffer, strlen(buffer));

		y += 20;
	}

	if (dockLight->IsActive()) skp->Text(x, y, "Docking light: On", 17);
	else skp->Text(x, y, "Docking light: Off", 18);
}

void Deepstar::DrawCargoHUD(int x, int y, oapi::Sketchpad* skp)
{
	sprintf(buffer, "Selected side: %s", cargoData.sideIndex == 0 ? "Starboard" : "Port");
	skp->Text(x, y, buffer, strlen(buffer));
	y += 20;

	if (cargoData.sectionIndex < 8)
	{
		sprintf(buffer, "Selected section: %d", cargoData.sectionIndex + 1);
		skp->Text(x, y, buffer, strlen(buffer));
		y += 20;

		sprintf(buffer, "Selected slot: %d", cargoData.slotIndex + 1);
		skp->Text(x, y, buffer, strlen(buffer));
		y += 20;
	}
	else
	{
		skp->Text(x, y, "Selected slot: Probe", 20);
		y += 20;
	}

	sprintf(buffer, "Selected cargo to add: %s", ucso->GetAvailableCargoName(cargoData.selectedCargo));
	skp->Text(x, y, buffer, strlen(buffer));
	y += 35;

	if (!cargoData.displayInfo)
	{
		skp->Text(x, y, "A = Select the starboard side", 29);
		y += 20;

		skp->Text(x, y, "D = Select the port side", 24);
		y += 20;

		skp->Text(x, y, "W = Select the previous section", 31);
		y += 20;

		skp->Text(x, y, "S = Select the next section", 27);
		y += 25;

		skp->Text(x, y, "Up Arrow = Select the previous slot", 35);
		y += 20;

		skp->Text(x, y, "Down Arrow = Select the next slot", 33);
		y += 20;

		skp->Text(x, y, "Left Arrow = Select the previous cargo", 38);
		y += 20;

		skp->Text(x, y, "Right Arrow = Select the next cargo", 35);
		y += 25;
	}

	skp->Text(x, y, "Shift + N = Display slot information", 36);
	y += 20;

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

	if (!cargoData.displayInfo) return ;

	y += 35;

	UCSO::Vessel::CargoInfo cargoInfo = ucso->GetCargoInfo(cargoData.sectionIndex < 8 ? cargoData.selectedSlot : cargoData.sideIndex);
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

void Deepstar::clbkPostStep(double simt, double simdt, double mjd)
{
	double fuelMass = GetFuelMass();

	// Main fuel full announcment
	if (fuelMass == DS_FUELMASS)
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
	if (fuelMass <= 6e5 && fuelMass > 0.001)
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

	// Set systems
	for (auto& system : systems)
	{
		if (system.repeat)
		{
			if (system.status == SystemStatus::OPEN)
			{
				double da = simdt * system.speed;

				system.proc = fmod(system.proc + da, 1);
				if (system.id == systems[SystemID::SPIN_GRAVITY].id) xrSound->PlayWav(SoundID::HYDRAULICS, false, 0.15f);

				SetAnimation(system.id, system.proc);
			}
		}
		else if (system.status == SystemStatus::OPENING || system.status == SystemStatus::CLOSING)
		{
			xrSound->PlayWav(SoundID::HYDRAULICS);

			double da = simdt * system.speed;

			if (system.status == SystemStatus::CLOSING)
			{
				if (system.proc > 0.0) system.proc = max(0.0, system.proc - da);
				else system.status = SystemStatus::CLOSED;
			}
			else
			{
				if (system.proc < 1.0) system.proc = min(1.0, system.proc + da);
				else system.status = SystemStatus::OPEN;
			}

			SetAnimation(system.id, system.proc);

			if (system.status == SystemStatus::OPEN || system.status == SystemStatus::CLOSED) xrSound->StopWav(SoundID::HYDRAULICS);
		}
	}

	// Set nose docking port based on the nose docking port doors
	if (!noseDock && systems[SystemID::NOSE_DOCK_DOORS].status == SystemStatus::OPEN)
	{
		noseDock = CreateDock({ 0, 0, 136.267 }, { 0, 0, 1 }, { 0, 1, 0 });
		EnableIDS(noseDock, true);
		SetIDSChannel(noseDock, 530);
	}
	else if (noseDock && systems[SystemID::NOSE_DOCK_DOORS].status != SystemStatus::OPEN)
	{
		EnableIDS(noseDock, false);
		DelDock(noseDock);
		noseDock = nullptr;
		dockLight->Activate(false);
	}

	if (cargoData.timer < 5) cargoData.timer += simdt;
}

int Deepstar::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
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

			case OAPI_KEY_N:
				cargoData.displayInfo = !cargoData.displayInfo;
				return 1;

			case OAPI_KEY_A:
				if (cargoData.sectionIndex != 8 && strncmp(ucso->GetAvailableCargoName(cargoData.selectedCargo), "DeepstarProbe", 13) == 0)
					cargoData.message = "Error: Probe can't be added to cargo slots.";
				else if (cargoData.sectionIndex == 8 && strncmp(ucso->GetAvailableCargoName(cargoData.selectedCargo), "DeepstarProbe", 13) != 0)
					cargoData.message = "Error: Cargo can't be added to probe slots.";
				else
				{
					int slot = (cargoData.sectionIndex == 8 ? cargoData.sideIndex : cargoData.selectedSlot);
					switch (ucso->AddCargo(cargoData.selectedCargo, slot))
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
				}

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_G:
				switch (ucso->GrappleCargo(cargoData.sectionIndex < 8 ? cargoData.selectedSlot : cargoData.sideIndex))
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
				switch (ucso->ReleaseCargo(cargoData.sectionIndex < 8 ? cargoData.selectedSlot : cargoData.sideIndex))
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
				switch (ucso->DeleteCargo(cargoData.sectionIndex < 8 ? cargoData.selectedSlot : cargoData.sideIndex))
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
			// Starboard side
			case OAPI_KEY_A:
				cargoData.sideIndex = 0;
				break;

			// Port side
			case OAPI_KEY_D:
				cargoData.sideIndex = 1;
				break;

			// Previous section
			case OAPI_KEY_W:
				cargoData.sectionIndex > 0 ? cargoData.sectionIndex-- : cargoData.sectionIndex = 8;
				break;

			// Next section
			case OAPI_KEY_S:
				cargoData.sectionIndex < 8 ? cargoData.sectionIndex++ : cargoData.sectionIndex = 0;
				break;

			// Previous slot
			case OAPI_KEY_UP:
				cargoData.slotIndex > 0 ? cargoData.slotIndex-- : cargoData.slotIndex = 8;
				break;

			// Next slot
			case OAPI_KEY_DOWN:
				cargoData.slotIndex < 8 ? cargoData.slotIndex++ : cargoData.slotIndex = 0;
				break;

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

			cargoData.selectedSlot = (((cargoData.sectionIndex + 1) * 9) - (-(cargoData.slotIndex + 1) + 9)) + (cargoData.sideIndex == 0 ? 1 : 73);
			return 1;
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

	DSSystem* system = nullptr;

	if (KEYMOD_CONTROL(kstate))
	{
		switch (key)
		{
		// Port dish
		case OAPI_KEY_1:
			system = &systems[SystemID::PORT_DISH];
			break;

		// Starboard dish
		case OAPI_KEY_2:
			system = &systems[SystemID::STRB_DISH];
			break;

		// Hab rotation
		case OAPI_KEY_G:
			system = &systems[SystemID::SPIN_GRAVITY];
			break;

		// Nose docking port
		case OAPI_KEY_K:
			if (!(noseDock && GetDockStatus(noseDock))) system = &systems[SystemID::NOSE_DOCK_DOORS];
			break;

		// Docking light
		case OAPI_KEY_L:
			if (noseDock) dockLight->Activate(!dockLight->IsActive());
			return 1;

		default: return 0;
		}
	}
	else return 0;

	if (!system) return 0;

	if (system->repeat)
		switch (system->status)
		{
		default:
		case SystemStatus::OPEN:
			system->status = SystemStatus::CLOSED;

			if (system->id == systems[SystemID::SPIN_GRAVITY].id) xrSound->StopWav(SoundID::HYDRAULICS);
			return 1;

		case SystemStatus::CLOSED:
			system->status = SystemStatus::OPEN;
			return 1;
		}
	else
		switch (system->status)
		{
		case SystemStatus::CLOSED:
			system->status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::CLOSING:
			system->status = SystemStatus::OPENING;
			return 1;

		case SystemStatus::OPENING:
			system->status = SystemStatus::CLOSING;
			return 1;

		case SystemStatus::OPEN:
			system->status = SystemStatus::CLOSING;
			return 1;

		default:
			system->status = SystemStatus::CLOSED;
			return 1;
		}
}

bool Deepstar::clbkVCMouseEvent(int id, int ev, VECTOR3& p)
{
	int ctrlSet = HIWORD(id);
	int ctrlID = LOWORD(id);

	switch (ctrlSet)
	{
	case VCCtrlSet::MFD_KEYS:
		oapiProcessMFDButton(mfdUpdateLineup[ctrlID / 12], ctrlID % 12, ev);

		if (ev == PANEL_MOUSE_LBUP) xrSound->PlayWav(SoundID::CLICK);
		return true;

	case VCCtrlSet::MFD_CTRLS:
		switch (ctrlID % 3)
		{
		case 0: // PWR KEY
			oapiSendMFDKey(mfdUpdateLineup[ctrlID / 3], OAPI_KEY_ESCAPE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		case 1: // SEL KEY
			oapiSendMFDKey(mfdUpdateLineup[ctrlID / 3], OAPI_KEY_F1);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		case 2: // MNU KEY
			oapiSendMFDKey(mfdUpdateLineup[ctrlID / 3], OAPI_KEY_GRAVE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	case VCCtrlSet::HUD_KEYS:
		// Default HUD seleceted
		if (ctrlID < 4) hudMode = HUDMode::DEFAULT;

		switch (ctrlID)
		{
		case 0: // OFF KEY
			oapiSetHUDMode(HUD_NONE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		case 1: // SRF KEY
			oapiSetHUDMode(HUD_SURFACE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		case 2: // ORB KEY
			oapiSetHUDMode(HUD_ORBIT);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		case 3: // DCK KEY
			oapiSetHUDMode(HUD_DOCKING);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// INF KEY
		case 4:
			if (hudMode == HUDMode::INFO) return true;

			if (oapiGetHUDMode() == HUD_NONE || oapiGetHUDMode() == HUD_DOCKING) oapiSetHUDMode(HUD_ORBIT);
			hudMode = HUDMode::INFO;

			oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// CRG KEY
		case 5:
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
			SetAttitudeMode(RCS_NONE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// ROT KEY
		case 1:
			SetAttitudeMode(RCS_ROT);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// LIN KEY
		case 2:
			SetAttitudeMode(RCS_LIN);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	case VCCtrlSet::NAV_KEYS:
		switch (ctrlID)
		{
		// KILL ROT KEY
		case 0:
			ToggleNavmode(NAVMODE_KILLROT);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// HOR LVL KEY
		case 2:
			ToggleNavmode(NAVMODE_HLEVEL);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// PRO GRD KEY
		case 4:
			ToggleNavmode(NAVMODE_PROGRADE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// RET GRD KEY
		case 1:
			ToggleNavmode(NAVMODE_RETROGRADE);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// NML + KEY
		case 3:
			ToggleNavmode(NAVMODE_NORMAL);

			xrSound->PlayWav(SoundID::CLICK);
			return true;

		// NML - KEY
		case 5:
			ToggleNavmode(NAVMODE_ANTINORMAL);

			xrSound->PlayWav(SoundID::CLICK);
			return true;
		}

	default:
		return false;
	}
}

void Deepstar::SetRCSConfig()
{
	if (rcsConfig)
		for (int mode = 1; mode < 8; mode++)
			if (GetNavmodeState(mode)) DeactivateNavmode(mode);

	for (int thruster = 0; thruster < 14; thruster++) SetThrusterMax0(rcsHandles[thruster], (rcsConfig ? DS_MAXRCSTH * 0.5 : DS_MAXRCSTH));

	SetThrusterMax0(rcsHandles[1], (rcsConfig ? DS_MAXRCSTH * 0.25 : DS_MAXRCSTH * 0.5));
	SetThrusterMax0(rcsHandles[2], (rcsConfig ? DS_MAXRCSTH * 0.25 : DS_MAXRCSTH * 0.5));
	SetThrusterMax0(rcsHandles[4], (rcsConfig ? DS_MAXRCSTH * 0.25 : DS_MAXRCSTH * 0.5));
	SetThrusterMax0(rcsHandles[5], (rcsConfig ? DS_MAXRCSTH * 0.25 : DS_MAXRCSTH * 0.5));
}