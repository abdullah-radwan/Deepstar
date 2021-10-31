// ==============================================================
//                 ORBITER MODULE: Deepstar Aerolander
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Aerolander.cpp
// Control module for Deepstar Aerolander vessel class
//
// ==============================================================

#include "Aerolander.h"

int Aerolander::vesselCount = 0;
oapi::Font* Aerolander::mfdFont = nullptr;
oapi::Font* Aerolander::systemFont = nullptr;

DLLCLBK VESSEL* ovcInit(OBJHANDLE hvessel, int flightmodel) { return new Aerolander(hvessel, flightmodel); }

DLLCLBK void ovcExit(VESSEL* vessel) { if (vessel) delete static_cast<Aerolander*>(vessel); }

Aerolander::Aerolander(OBJHANDLE hVessel, int flightmodel) : VESSEL4(hVessel, flightmodel) 
{
	ucso = UCSO::Vessel::CreateInstance(this);
	ucso->SetUnpackingRange(20);

	sprintf(buffer, "UCSO version: %s", ucso->GetUCSOVersion());
	cargoData.message = _strdup(buffer);

	if (!vesselCount)
	{
		mfdFont = oapiCreateFont(24, true, "Arial", FONT_BOLD);
		systemFont = oapiCreateFont(30, true, "Arial", FONT_BOLD);
	}

	vesselCount++;
}

Aerolander::~Aerolander() 
{
	delete ucso;

	vesselCount--;

	if (!vesselCount)
	{
		oapiReleaseFont(mfdFont);
		oapiReleaseFont(systemFont);
	}
}

void Aerolander::clbkSetClassCaps(FILEHANDLE cfg)
{
	SetSize(DA_SIZE);
	SetEmptyMass(DA_EMPTYMASS);
	SetPMI(DA_PMI);
	SetCrossSections(DA_CS);
	SetRotDrag(DA_RD);
	SetTouchdownPoints(tdVtxGearUp, tdVtxN);

	CreateAirfoil3(LIFT_VERTICAL, _V(0, 0, -0.1), VLiftCoeff, 0, 9.9, 200, 0.85);
	CreateAirfoil3(LIFT_HORIZONTAL, _V(0, 0, -3.8), HLiftCoeff, 0, 13.2, 69, 1);

	CreateVariableDragElement(&systems[SystemID::LANDING_GEAR].proc, 0.8, _V(0, -2.5, -4.5));     // Landing gear
	CreateVariableDragElement(&systems[SystemID::AIRBRAKE].proc, 6, _V(0, 2.51652, -3.08364));    // Airbrake
	CreateVariableDragElement(&systems[SystemID::PAYLOAD_BAY].proc, 1, _V(0, 2.72058, 0.307962)); // Payload bay
	CreateVariableDragElement(&systems[SystemID::HOVER_DOORS].proc, 0.2, _V(0, -2.5, -5.5));      // Hover doors
	CreateVariableDragElement(&systems[SystemID::EGRESS_HATCH].proc, 2, _V(0, -3.18531, 7.2709)); // Egress hatch

	DefineEngines();
	DefineSystems();
	DefineSlots();

	vcMesh = oapiLoadMesh("Deepstar/AerolanderVC");

	SetMeshVisibilityMode(AddMesh("Deepstar/Aerolander"), MESHVIS_EXTERNAL);
	SetMeshVisibilityMode(AddMesh(vcMesh), MESHVIS_VC);

	SetCameraOffset({ -0.44, 1.35, 10.15 });
}

void Aerolander::DefineEngines()
{
	PROPELLANT_HANDLE propHandle = CreatePropellantResource(DA_FUELMASS);

	// Main engine
	THRUSTER_HANDLE mainHandle = CreateThruster(_V(0, 0, 0), _V(0, 0, 1), DA_MAXMAINTH, propHandle, DA_ISP);
	CreateThrusterGroup(&mainHandle, 1, THGROUP_MAIN);
	AddExhaust(mainHandle, 4, 1, _V(0, -1.131, -19.13), _V(0, 0, -1));

	// Hover engine
	hoverEngine.handle = CreateThruster(_V(0, 0, 0), _V(0, 1, 0), DA_MAXHVRTH, propHandle, DA_ISP);
	AddExhaust(hoverEngine.handle, 4, 1, _V(0, -3.35, -3.03), _V(0, -1, 0));
	AddExhaust(hoverEngine.handle, 4, 1, _V(0, -3.35, -7.567), _V(0, -1, 0));

	// RCS engines
	// Up thrusters
	rcsHandles[0] = CreateThruster(_V(2.5, 0, 12.8), _V(0, -1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[0], 1, 0.1, _V(0.516, 0.374, 12.8), _V(0, 1, 0));
	AddExhaust(rcsHandles[0], 1, 0.1, _V(0, 0.456, 12.8), _V(0, 1, 0));

	rcsHandles[1] = CreateThruster(_V(-2.5, 0, 12.8), _V(0, -1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[1], 1, 0.1, _V(-0.516, 0.374, 12.8), _V(0, 1, 0));

	rcsHandles[2] = CreateThruster(_V(-2.5, 0, -12.8), _V(0, -1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[2], 1, 0.1, _V(-2.5, -1.054, -16.382), _V(0, 1, 0));

	rcsHandles[3] = CreateThruster(_V(2.5, 0, -12.8), _V(0, -1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[3], 1, 0.1, _V(2.5, -1.054, -16.382), _V(0, 1, 0));

	// Down thrusters
	rcsHandles[4] = CreateThruster(_V(2.5, 0, 12.8), _V(0, 1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[4], 1, 0.1, _V(0.516, -2.069, 12.8), _V(0, -1, 0));

	rcsHandles[5] = CreateThruster(_V(-2.5, 0, 12.8), _V(0, 1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[5], 1, 0.1, _V(-0.516, -2.069, 12.8), _V(0, -1, 0));
	AddExhaust(rcsHandles[5], 1, 0.1, _V(0, -2.103, 12.8), _V(0, -1, 0));

	rcsHandles[6] = CreateThruster(_V(-2.5, 0, -12.8), _V(0, 1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[6], 1, 0.1, _V(-2.5, -1.756, -16.382), _V(0, -1, 0));

	rcsHandles[7] = CreateThruster(_V(2.5, 0, -12.8), _V(0, 1, 0), DA_MAXRCSTH, propHandle, DA_ISP);
	AddExhaust(rcsHandles[7], 1, 0.1, _V(2.5, -1.756, -16.382), _V(0, -1, 0));

	// Right thrusters
	rcsHandles[8] = CreateThruster(_V(-1.558, 0, 12.8), _V(1, 0, 0), DA_MAXRCSTH * 2, propHandle, DA_ISP);
	AddExhaust(rcsHandles[8], 1, 0.1, _V(-1.558, -0.678, 12.8), _V(-1, 0, 0));

	rcsHandles[9] = CreateThruster(_V(-1.558, 0, -12.8), _V(1, 0, 0), DA_MAXRCSTH * 2, propHandle, DA_ISP);
	AddExhaust(rcsHandles[9], 1, 0.1, _V(-2.85, -1.405, -16.382), _V(-1, 0, 0));

	// Left thrusters
	rcsHandles[10] = CreateThruster(_V(1.558, 0, 12.8), _V(-1, 0, 0), DA_MAXRCSTH * 2, propHandle, DA_ISP);
	AddExhaust(rcsHandles[10], 1, 0.1, _V(1.558, -0.678, 12.8), _V(1, 0, 0));

	rcsHandles[11] = CreateThruster(_V(1.558, 0, -12.8), _V(-1, 0, 0), DA_MAXRCSTH * 2, propHandle, DA_ISP);
	AddExhaust(rcsHandles[11], 1, 0.1, _V(2.85, -1.405, -16.382), _V(1, 0, 0));

	// Forward thrusters
	rcsHandles[12] = CreateThruster(_V(0, 0, 12.112), _V(0, 0, -1), DA_MAXRCSTH * 1.333, propHandle, DA_ISP);
	AddExhaust(rcsHandles[12], 1, 0.1, _V(0, 0.868, 12.112), _V(0, 0, 1));

	rcsHandles[13] = CreateThruster(_V(-0.445, 0, 12.112), _V(0, 0, -1), DA_MAXRCSTH * 1.333, propHandle, DA_ISP);
	AddExhaust(rcsHandles[13], 1, 0.1, _V(-0.445, 0.777, 12.112), _V(0, 0, 1));

	rcsHandles[14] = CreateThruster(_V(0.445, 0, 12.112), _V(0, 0, -1), DA_MAXRCSTH * 1.333, propHandle, DA_ISP);
	AddExhaust(rcsHandles[14], 1, 0.1, _V(0.445, 0.777, 12.112), _V(0, 0, 1));

	// Aft thrusters
	rcsHandles[15] = CreateThruster(_V(-2.5, 0, -16.733), _V(0, 0, 1), DA_MAXRCSTH * 2, propHandle, DA_ISP);
	AddExhaust(rcsHandles[15], 1, 0.1, _V(-2.5, -1.405, -16.733), _V(0, 0, -1));

	rcsHandles[16] = CreateThruster(_V(2.5, 0, -16.733), _V(0, 0, 1), DA_MAXRCSTH * 2, propHandle, DA_ISP);
	AddExhaust(rcsHandles[16], 1, 0.1, _V(2.5, -1.405, -16.733), _V(0, 0, -1));
}

void Aerolander::DefineSystems()
{
	// Transponder and docking port
	int channelFactor = 10 * (vesselCount - 1);

	EnableTransponder(true);
	SetTransponderChannel(560 + channelFactor);

	DOCKHANDLE dockPort = CreateDock({ 0, 3.407, 5.55 }, { 0, 1, 0 }, { 0, 0, 1 });
	EnableIDS(dockPort, true);
	SetIDSChannel(dockPort, 562 + channelFactor);

	// Landing gear
	systems[SystemID::LANDING_GEAR].infoName = "Landing gear";
	systems[SystemID::LANDING_GEAR].scnName = "LANDING_GEAR";

	systems[SystemID::LANDING_GEAR].id = CreateAnimation(0);
	systems[SystemID::LANDING_GEAR].status = SystemStatus::CLOSED;
	systems[SystemID::LANDING_GEAR].proc = 0;
	systems[SystemID::LANDING_GEAR].speed = 0.1;

	static UINT PortOutDoorGrp[1] = { 27 };
	static MGROUP_ROTATE PortOutDoorRot(0, PortOutDoorGrp, 1, _V(-6.09, -2.631, -7.696), _V(0, 0, 1), (float)(-PI05));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0, 0.5, &PortOutDoorRot);

	static UINT PortInDoorGrp[1] = { 24 };
	static MGROUP_ROTATE PortInDoorRot(0, PortInDoorGrp, 1, _V(-2.882, -3.008, -7.693), _V(0, 0, 1), (float)(97 * RAD));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0, 0.5, &PortInDoorRot);

	static UINT StbOutDoorGrp[1] = { 28 };
	static MGROUP_ROTATE StbOutDoorRot(0, StbOutDoorGrp, 1, _V(6.09, -2.631, -7.696), _V(0, 0, 1), (float)(PI05));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0, 0.5, &StbOutDoorRot);

	static UINT StbInDoorGrp[1] = { 23 };
	static MGROUP_ROTATE StbInDoorRot(0, StbInDoorGrp, 1, _V(2.882, -3.008, -7.693), _V(0, 0, 1), (float)(-97 * RAD));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0, 0.5, &StbInDoorRot);

	static UINT StbNoseDoorGrp[1] = { 11 };
	static MGROUP_ROTATE StbNoseDoorRot(0, StbNoseDoorGrp, 1, _V(0.743, -3.239, 4.108), _V(-0.004, 0, 1), (float)(95 * RAD));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0, 0.5, &StbNoseDoorRot);

	static UINT PortNoseDoorGrp[1] = { 12 };
	static MGROUP_ROTATE PortNoseDoorRot(0, PortNoseDoorGrp, 1, _V(-0.743, -3.239, 4.108), _V(0.004, 0, 1), (float)(-95 * RAD));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0, 0.5, &PortNoseDoorRot);

	static UINT NoseGearGrp[1] = { 7 };
	static MGROUP_ROTATE NoseGearRot(0, NoseGearGrp, 1, _V(0, -1.924, 1.76), _V(1, 0, 0), (float)(80 * RAD));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0.4, 1, &NoseGearRot);

	static UINT MainGearGrp[1] = { 6 };
	static MGROUP_ROTATE MainGearRot(0, MainGearGrp, 1, _V(0, -1.924, -10.326), _V(1, 0, 0), (float)(94.5 * RAD));
	AddAnimationComponent(systems[SystemID::LANDING_GEAR].id, 0.4, 1, &MainGearRot);

	// Airbrake
	systems[SystemID::AIRBRAKE].infoName = "Airbrake";
	systems[SystemID::AIRBRAKE].scnName = "AIRBRAKE";

	systems[SystemID::AIRBRAKE].id = CreateAnimation(0);
	systems[SystemID::AIRBRAKE].status = SystemStatus::CLOSED;
	systems[SystemID::AIRBRAKE].proc = 0;
	systems[SystemID::AIRBRAKE].speed = 1;

	static UINT AirBrkGrp1[1] = { 36 };
	static MGROUP_ROTATE AirBrkRot1(0, AirBrkGrp1, 1, _V(0, 2.657, -1.127), _V(1, 0, 0), (float)(45 * RAD));
	ANIMATIONCOMPONENT_HANDLE AirBrk1 = AddAnimationComponent(systems[SystemID::AIRBRAKE].id, 0, 1, &AirBrkRot1);

	static UINT AirBrkGrp2[1] = { 37 };
	static MGROUP_ROTATE AirBrkRot2(0, AirBrkGrp2, 1, _V(0, 2.41, -2.596), _V(1, 0, 0), (float)(-102 * RAD));
	ANIMATIONCOMPONENT_HANDLE AirBrk2 = AddAnimationComponent(systems[SystemID::AIRBRAKE].id, 0, 1, &AirBrkRot2, AirBrk1);

	static UINT AirBrkGrp3[1] = { 38 };
	static MGROUP_ROTATE AirBrkRot3(0, AirBrkGrp3, 1, _V(0, 2.293, -3.907), _V(1, 0, 0), (float)(55 * RAD));
	AddAnimationComponent(systems[SystemID::AIRBRAKE].id, 0, 1, &AirBrkRot3, AirBrk2);

	// Payload bay doors and docking port
	systems[SystemID::PAYLOAD_BAY].infoName = "Payload bay";
	systems[SystemID::PAYLOAD_BAY].scnName = "PAYLOAD_BAY";

	systems[SystemID::PAYLOAD_BAY].id = CreateAnimation(0);
	systems[SystemID::PAYLOAD_BAY].status = SystemStatus::CLOSED;
	systems[SystemID::PAYLOAD_BAY].proc = 0;
	systems[SystemID::PAYLOAD_BAY].speed = 0.1;

	static UINT PortGrp[1] = { 4 };
	static MGROUP_ROTATE PortRot(0, PortGrp, 1, _V(-3.401, 0.75, 3.618), _V(0.165, 0, 1), (float)(125 * RAD));
	AddAnimationComponent(systems[SystemID::PAYLOAD_BAY].id, 0, 1, &PortRot);

	static UINT StbGrp[1] = { 5 };
	static MGROUP_ROTATE StbRot(0, StbGrp, 1, _V(3.401, 0.75, 3.618), _V(-0.165, 0, 1), (float)(-125 * RAD));
	AddAnimationComponent(systems[SystemID::PAYLOAD_BAY].id, 0, 1, &StbRot);

	static UINT DockGrp[3] = { 1,2,3 };
	static MGROUP_TRANSLATE DockTrans(0, DockGrp, 3, _V(0, 1, 0));
	AddAnimationComponent(systems[SystemID::PAYLOAD_BAY].id, 0.4, 1, &DockTrans);

	// Hover doors
	systems[SystemID::HOVER_DOORS].infoName = "Hover doors";
	systems[SystemID::HOVER_DOORS].scnName = "HOVER_DOORS";

	systems[SystemID::HOVER_DOORS].id = CreateAnimation(0);
	systems[SystemID::HOVER_DOORS].status = SystemStatus::CLOSED;
	systems[SystemID::HOVER_DOORS].proc = 0;
	systems[SystemID::HOVER_DOORS].speed = 0.25;

	static UINT PortHoverGrp[1] = { 13 };
	static MGROUP_ROTATE PortHoverRot(0, PortHoverGrp, 1, _V(-1.45, -3.214, -5.299), _V(0, 0, 1), (float)(-110 * RAD));
	AddAnimationComponent(systems[SystemID::HOVER_DOORS].id, 0, 0.8, &PortHoverRot);

	static UINT StbHoverGrp[1] = { 10 };
	static MGROUP_ROTATE StbHoverRot(0, StbHoverGrp, 1, _V(1.45, -3.214, -5.299), _V(0, 0, 1), (float)(110 * RAD));
	AddAnimationComponent(systems[SystemID::HOVER_DOORS].id, 0, 0.8, &StbHoverRot);

	static UINT EngBellGrp[2] = { 19,20 };
	static MGROUP_TRANSLATE EngBellTrans(0, EngBellGrp, 2, _V(0, -0.3, 0));
	AddAnimationComponent(systems[SystemID::HOVER_DOORS].id, 0.7, 1, &EngBellTrans);

	// Egress hatch
	systems[SystemID::EGRESS_HATCH].infoName = "Egress hatch";
	systems[SystemID::EGRESS_HATCH].scnName = "EGRESS_HATCH";

	systems[SystemID::EGRESS_HATCH].id = CreateAnimation(0);
	systems[SystemID::EGRESS_HATCH].status = SystemStatus::CLOSED;
	systems[SystemID::EGRESS_HATCH].proc = 0;
	systems[SystemID::EGRESS_HATCH].speed = 0.1;

	static UINT InHatchGrp[1] = { 16 };
	static MGROUP_ROTATE InHatchRot(0, InHatchGrp, 1, _V(0, -0.664, 9.179), _V(1, 0, 0), (float)(-PI05));
	AddAnimationComponent(systems[SystemID::EGRESS_HATCH].id, 0, 0.3, &InHatchRot);

	static UINT HullHatchGrp[1] = { 17 };
	static MGROUP_ROTATE HullHatchRot(0, HullHatchGrp, 1, _V(0, -2.965, 9.279), _V(1, 0, 0), (float)(-PI05));
	AddAnimationComponent(systems[SystemID::EGRESS_HATCH].id, 0.3, 0.6, &HullHatchRot);

	static UINT LadderGrp[2] = { 14,15 };
	static MGROUP_TRANSLATE LadderTrans(0, LadderGrp, 2, _V(0, -1.5, 0));
	AddAnimationComponent(systems[SystemID::EGRESS_HATCH].id, 0.5, 0.75, &LadderTrans);

	static UINT LowLadderGrp[1] = { 15 };
	static MGROUP_TRANSLATE LowLadderTrans(0, LowLadderGrp, 1, _V(0, -1.51, 0));
	AddAnimationComponent(systems[SystemID::EGRESS_HATCH].id, 0.75, 1, &LowLadderTrans);

	// Rudder
	UINT RudderAnim = CreateAnimation(0.5);

	static UINT LRudderGrp[1] = { 21 };
	static MGROUP_ROTATE LRudderRot(0, LRudderGrp, 1, _V(-10.836, 1.577, -12.663), _V(0, 0.3, 0), (float)(-50 * RAD));
	AddAnimationComponent(RudderAnim, 0, 1, &LRudderRot);

	static UINT RRudderGrp[1] = { 22 };
	static MGROUP_ROTATE RRudderRot(0, RRudderGrp, 1, _V(10.836, 1.577, -12.663), _V(0, 0.3, 0), (float)(-50 * RAD));
	AddAnimationComponent(RudderAnim, 0, 1, &RRudderRot);

	CreateControlSurface(AIRCTRL_RUDDER, 2.5, 1.7, _V(0, 0, -15.9238), AIRCTRL_AXIS_YPOS, RudderAnim);

	// Elevator
	UINT ElevatorAnim = CreateAnimation(0.5);

	static UINT RElevGrp[1] = { 25 };
	static MGROUP_ROTATE RElevRot(0, RElevGrp, 1, _V(-7.945, -0.903, -12.576), _V(1, 0, 0), (float)(40 * RAD));
	AddAnimationComponent(ElevatorAnim, 0.5, 1, &RElevRot);

	static UINT LElevGrp[1] = { 26 };
	static MGROUP_ROTATE LElevRot(0, LElevGrp, 1, _V(7.945, -0.903, -12.576), _V(1, 0, 0), (float)(40 * RAD));
	AddAnimationComponent(ElevatorAnim, 0.5, 1, &LElevRot);

	static UINT DownElevGrp[1] = { 18 };
	static MGROUP_ROTATE DownElevRot(0, DownElevGrp, 1, _V(0, -3.211, -12.576), _V(1, 0, 0), (float)(40 * RAD));
	AddAnimationComponent(ElevatorAnim, 0, 0.5, &DownElevRot);

	CreateControlSurface(AIRCTRL_ELEVATOR, 3.23, 1.7, _V(0, 0, -15.9238), AIRCTRL_AXIS_XPOS, ElevatorAnim);

	// Elevator Trim
	UINT ElevatorTrimAnim = CreateAnimation(0.5);

	static MGROUP_ROTATE RElevTrimRot(0, RElevGrp, 1, _V(-7.945, -0.903, -12.576), _V(1, 0, 0), (float)(10 * RAD));
	AddAnimationComponent(ElevatorTrimAnim, 0.5, 1, &RElevTrimRot);

	static MGROUP_ROTATE LElevTrimRot(0, LElevGrp, 1, _V(7.945, -0.903, -12.576), _V(1, 0, 0), (float)(10 * RAD));
	AddAnimationComponent(ElevatorTrimAnim, 0.5, 1, &LElevTrimRot);

	static MGROUP_ROTATE DownElevTrimRot(0, DownElevGrp, 1, _V(0, -3.211, -12.576), _V(1, 0, 0), (float)(10 * RAD));
	AddAnimationComponent(ElevatorTrimAnim, 0, 0.5, &DownElevTrimRot);

	CreateControlSurface(AIRCTRL_ELEVATORTRIM, 0.69, 1.7, _V(0, 0, -15.9238), AIRCTRL_AXIS_XPOS, ElevatorTrimAnim);

	// Ailerons
	UINT RAileronAnim = CreateAnimation(0.5);
	static MGROUP_ROTATE RAilRot(0, LElevGrp, 1, _V(-7.945, -0.903, -12.576), _V(1, 0, 0), (float)(20 * RAD));
	AddAnimationComponent(RAileronAnim, 0.5, 1, &RAilRot);

	UINT LAileronAnim = CreateAnimation(0.5);
	static MGROUP_ROTATE LAilRot(0, RElevGrp, 1, _V(7.945, -0.903, -12.576), _V(1, 0, 0), (float)(-20 * RAD));
	AddAnimationComponent(LAileronAnim, 0, 0.5, &LAilRot);

	CreateControlSurface(AIRCTRL_AILERON, 0.69, 1.7, _V(-8.42264, 0, -15.9238), AIRCTRL_AXIS_XNEG, RAileronAnim);
	CreateControlSurface(AIRCTRL_AILERON, 0.69, 1.7, _V(8.42264, 0, -15.9238), AIRCTRL_AXIS_XPOS, LAileronAnim);
}

void Aerolander::DefineSlots()
{
	ucso->SetSlotAttachment(0, CreateAttachment(false, { 0, -0.531, 2.627 }, { 0, 1, 0 }, { 0, 0, 1 }, "Centeral Slot"));

	ucso->SetSlotAttachment(1, CreateAttachment(false, { -2.014, -0.551, 5.592 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 1"));
	ucso->SetSlotAttachment(2, CreateAttachment(false, { 2.014, -0.551, 5.592 }, { 0, 1, 0 }, { 0, 0, 1 }, "Slot 2"));

	// First column
	for (int slot = 3; slot < 7; slot++)
	{
		sprintf(buffer, "Slot %d", slot);

		ucso->SetSlotAttachment(slot, CreateAttachment(false, { -2.1 + ((slot - 3) * 1.4), -0.551, 3.74 }, { 0, 1, 0 }, { 0, 0, 1 }, buffer));
	}

	// Second column
	for (int slot = 7; slot < 11; slot++)
	{
		sprintf(buffer, "Slot %d", slot);

		ucso->SetSlotAttachment(slot, CreateAttachment(false, { -2.1 + ((slot - 7) * 1.4), -0.551, 2.39 }, { 0, 1, 0 }, { 0, 0, 1 }, buffer));
	}

	// Third column
	for (int slot = 11; slot < 16; slot++)
	{
		sprintf(buffer, "Slot %d", slot);

		ucso->SetSlotAttachment(slot, CreateAttachment(false, { -2.8 + ((slot - 11) * 1.4), -0.551, 1.04 }, { 0, 1, 0 }, { 0, 0, 1 }, buffer));
	}
}

void Aerolander::clbkLoadStateEx(FILEHANDLE scn, void* status)
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

void Aerolander::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	for (auto& system : systems)
	{
		sprintf(buffer, "%d %0.4f", system.status, system.proc);
		oapiWriteScenario_string(scn, system.scnName, buffer);
	}

	oapiWriteScenario_int(scn, "RCS_CONFIG", rcsConfig);
}

bool Aerolander::clbkLoadVC(int id)
{
	static VCHUDSPEC hud = { 1, 15, { -0.44, 1.4, 10.935 }, 1 };

	switch (id)
	{
	case VCView::SEAT_LEFT:
	{
		oapiVCSetNeighbours(-1, VCView::SEAT_RIGHT, VCView::TOP_LEFT, -1);

		SetCameraOffset({ -0.44, 1.35, 10.15 });

		SetCameraDefaultDirection({ 0, 0, 1 });

		hud.hudcnt = { -0.44, 1.4, 10.935 };

		break;
	}
	case VCView::SEAT_RIGHT:
	{
		oapiVCSetNeighbours(VCView::SEAT_LEFT, -1, VCView::TOP_RIGHT, -1);

		SetCameraOffset({ 0.44, 1.35, 10.15 });

		SetCameraDefaultDirection({ 0, 0, 1 });

		hud.hudcnt = { 0.44, 1.4, 10.935 };

		break;
	}
	case VCView::TOP_LEFT:
		oapiVCSetNeighbours(-1, VCView::TOP_RIGHT, -1, VCView::SEAT_LEFT);

		SetCameraOffset({ -0.39, 2, 8.5 });

		SetCameraDefaultDirection({ 0, 0.887, 0.4617 }); // 0, sin(62.5 * RAD), cos(62.5 * RAD)

		return true;

	case VCView::TOP_RIGHT:
		oapiVCSetNeighbours(VCView::TOP_LEFT, -1, -1, VCView::SEAT_RIGHT);

		SetCameraOffset({ 0.39, 2, 8.5 });

		SetCameraDefaultDirection({ 0, 0.887, 0.4617 }); // 0, sin(62.5 * RAD), cos(62.5 * RAD)

		return true;
	}

	oapiVCRegisterHUD(&hud);

	static VCMFDSPEC mfd1 = { 1, 9 };
	static VCMFDSPEC mfd2 = { 1, 10 };
	static VCMFDSPEC mfd3 = { 1, 12 };
	static VCMFDSPEC mfd4 = { 1, 13 };

	oapiVCRegisterMFD(MFD_LEFT, &mfd1);
	oapiVCRegisterMFD(MFD_RIGHT, &mfd2);
	oapiVCRegisterMFD(MFD_USER1, &mfd3);
	oapiVCRegisterMFD(MFD_USER2, &mfd4);

	static const VECTOR3 mfdKeyPositions[48] =
	{
		_V(-0.835, 0.978215, 10.8894), // MFD 1 KEY 1
		_V(-0.835, 0.93311, 10.873),   // MFD 1 KEY 2
		_V(-0.835, 0.888004, 10.8566), // MFD 1 KEY 3
		_V(-0.835, 0.842899, 10.8402), // MFD 1 KEY 4
		_V(-0.835, 0.797794, 10.8237), // MFD 1 KEY 5
		_V(-0.835, 0.752689, 10.8073), // MFD 1 KEY 6

		_V(-0.485, 0.978215, 10.8894), // MFD 1 KEY 7
		_V(-0.485, 0.93311, 10.873),   // MFD 1 KEY 8
		_V(-0.485, 0.888004, 10.8566), // MFD 1 KEY 9
		_V(-0.485, 0.842899, 10.8402), // MFD 1 KEY 10
		_V(-0.485, 0.797794, 10.8237), // MFD 1 KEY 11
		_V(-0.485, 0.752689, 10.8073), // MFD 1 KEY 12

		_V(-0.395, 0.978215, 10.8894), // MFD 2 KEY 1
		_V(-0.395, 0.93311, 10.873),   // MFD 2 KEY 2
		_V(-0.395, 0.888004, 10.8566), // MFD 2 KEY 3
		_V(-0.395, 0.842899, 10.8402), // MFD 2 KEY 4
		_V(-0.395, 0.797794, 10.8237), // MFD 2 KEY 5
		_V(-0.395, 0.752689, 10.8073), // MFD 2 KEY 6

		_V(-0.045, 0.978215, 10.8894), // MFD 2 KEY 7
		_V(-0.045, 0.93311, 10.873),   // MFD 2 KEY 8
		_V(-0.045, 0.888004, 10.8566), // MFD 2 KEY 9
		_V(-0.045, 0.842899, 10.8402), // MFD 2 KEY 10
		_V(-0.045, 0.797794, 10.8237), // MFD 2 KEY 11
		_V(-0.045, 0.752689, 10.8073), // MFD 2 KEY 12

		_V(0.045, 0.978215, 10.8894),  // MFD 3 KEY 1
		_V(0.045, 0.93311, 10.873),    // MFD 3 KEY 2
		_V(0.045, 0.888004, 10.8566),  // MFD 3 KEY 3
		_V(0.045, 0.842899, 10.8402),  // MFD 3 KEY 4
		_V(0.045, 0.797794, 10.8237),  // MFD 3 KEY 5
		_V(0.045, 0.752689, 10.8073),  // MFD 3 KEY 6

		_V(0.395, 0.978215, 10.8894),  // MFD 3 KEY 7
		_V(0.395, 0.93311, 10.873),    // MFD 3 KEY 8
		_V(0.395, 0.888004, 10.8566),  // MFD 3 KEY 9
		_V(0.395, 0.842899, 10.8402),  // MFD 3 KEY 10
		_V(0.395, 0.797794, 10.8237),  // MFD 3 KEY 11
		_V(0.395, 0.752689, 10.8073),  // MFD 3 KEY 12

		_V(0.485, 0.978215, 10.8894),  // MFD 4 KEY 1
		_V(0.485, 0.93311, 10.873),    // MFD 4 KEY 2
		_V(0.485, 0.888004, 10.8566),  // MFD 4 KEY 3
		_V(0.485, 0.842899, 10.8402),  // MFD 4 KEY 4
		_V(0.485, 0.797794, 10.8237),  // MFD 4 KEY 5
		_V(0.485, 0.752689, 10.8073),  // MFD 4 KEY 6

		_V(0.835, 0.978215, 10.8894),  // MFD 4 KEY 7
		_V(0.835, 0.93311, 10.873),    // MFD 4 KEY 8
		_V(0.835, 0.888004, 10.8566),  // MFD 4 KEY 9
		_V(0.835, 0.842899, 10.8402),  // MFD 4 KEY 10
		_V(0.835, 0.797794, 10.8237),  // MFD 4 KEY 11
		_V(0.835, 0.752689, 10.8073)   // MFD 4 KEY 12
	};

	for (int key = 0; key < 48; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::MFD_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_LBUP, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::MFD_KEYS << 16) | (key & 0xFFFF), mfdKeyPositions[key], 0.01);
	}

	static const VECTOR3 mfdCtrlPositions[12] =
	{
		_V(-0.78, 0.701006, 10.7885),     // MFD 1 PWR
		_V(-0.731021, 0.701006, 10.7885), // MFD 1 SEL
		_V(-0.54, 0.701006, 10.7885),     // MFD 1 MNU

		_V(-0.34, 0.701006, 10.7885),     // MFD 2 PWR
		_V(-0.291021, 0.701006, 10.7885), // MFD 2 SEL
		_V(-0.1, 0.701006, 10.7885),      // MFD 2 MNU

		_V(0.1, 0.701006, 10.7885),       // MFD 3 PWR
		_V(0.148979, 0.701006, 10.7885),  // MFD 3 SEL
		_V(0.34, 0.701006, 10.7885),      // MFD 3 MNU

		_V(0.54, 0.701006, 10.7885),      // MFD 4 PWR
		_V(0.588979, 0.701006, 10.7885),  // MFD 4 SEL
		_V(0.78, 0.701006, 10.7885)       // MFD 4 MNU
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::MFD_CTRLS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::MFD_CTRLS << 16) | (key & 0xFFFF), mfdCtrlPositions[key], 0.01);
	}

	static const VECTOR3 hudKeyPos[12] =
	{
		_V(-0.483529, 1.01837, 10.9041), // LEFT OFF
		_V(-0.465937, 1.01837, 10.9041), // LEFT SRF
		_V(-0.449048, 1.01837, 10.9041), // LEFT ORB
		_V(-0.431456, 1.01837, 10.9041), // LEFT DCK
		_V(-0.414, 1.01837, 10.9041),    // LEFT INF
		_V(-0.396471, 1.01837, 10.9041), // LEFT CRG

		_V(0.483529, 1.01837, 10.9041),  // RIGHT OFF
		_V(0.465937, 1.01837, 10.9041),  // RIGHT SRF
		_V(0.449048, 1.01837, 10.9041),  // RIGHT ORB
		_V(0.431456, 1.01837, 10.9041),  // RIGHT DCK
		_V(0.414, 1.01837, 10.9041),	 // RIGHT INF
		_V(0.396471, 1.01837, 10.9041)   // RIGHT CRG
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::HUD_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::HUD_KEYS << 16) | (key & 0xFFFF), hudKeyPos[key], 0.01);
	}

	static const VECTOR3 rcsKeyPos[6] =
	{
		_V(-0.4648, 0.561886, 10.7378), // LEFT OFF
		_V(-0.4398, 0.561886, 10.7378), // LEFT ROT
		_V(-0.4148, 0.561886, 10.7378), // LEFT LIN

		_V(0.4648, 0.561886, 10.7378), // RIGHT OFF
		_V(0.4398, 0.561886, 10.7378), // RIGHT ROT
		_V(0.4148, 0.561886, 10.7378)  // RIGHT LIN
	};

	for (int key = 0; key < 6; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::RCS_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::RCS_KEYS << 16) | (key & 0xFFFF), rcsKeyPos[key], 0.01);
	}

	static const VECTOR3 navKeyPos[12] =
	{
		_V(-0.4648, 0.621915, 10.7597), // LEFT KILL ROT
		_V(-0.4648, 0.596918, 10.7505), // LEFT HOR LVL
		_V(-0.4398, 0.621915, 10.7597), // LEFT PRO GRD
		_V(-0.4398, 0.596918, 10.7505), // LEFT RET GRD
		_V(-0.4148, 0.621915, 10.7597), // LEFT NML +
		_V(-0.4148, 0.596918, 10.7505), // LEFT NML -

		_V(0.4648, 0.621915, 10.7597), // RIGHT KILL ROT
		_V(0.4648, 0.596918, 10.7505), // RIGHT HOR LVL
		_V(0.4398, 0.621915, 10.7597), // RIGHT PRO GRD
		_V(0.4398, 0.596918, 10.7505), // RIGHT RET GRD
		_V(0.4148, 0.621915, 10.7597), // RIGHT NML +
		_V(0.4148, 0.596918, 10.7505)  // RIGHT NML -
	};

	for (int key = 0; key < 12; ++key)
	{
		oapiVCRegisterArea((VCCtrlSet::NAV_KEYS << 16) | (key & 0xFFFF), _R(0, 0, 0, 0), PANEL_REDRAW_NEVER, PANEL_MOUSE_LBDOWN, PANEL_MAP_NONE, NULL);
		oapiVCSetAreaClickmode_Spherical((VCCtrlSet::NAV_KEYS << 16) | (key & 0xFFFF), navKeyPos[key], 0.01);
	}

	SURFHANDLE mfdTex = oapiGetTextureHandle(vcMesh, 9);
	SURFHANDLE systemTex = oapiGetTextureHandle(vcMesh, 8);

	oapiVCRegisterArea(VCArea::MFD_KEYS, _R(0, 0, 512, 512), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, mfdTex);
	oapiVCRegisterArea(VCArea::HUD_KEYS, _R(512, 261, 1024, 345), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);
	oapiVCRegisterArea(VCArea::RCS_KEYS, _R(512, 429, 765, 512), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);
	oapiVCRegisterArea(VCArea::NAV_KEYS, _R(512, 345, 1018, 429), PANEL_REDRAW_USER, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, systemTex);

	return true;
}

void Aerolander::clbkPostCreation() 
{
	dockPort.handle = GetDockHandle(0);
	dockPort.idsChannel = oapiGetNavChannel(GetIDS(dockPort.handle));
	
	SetRCSConfig();
	mainFuel.full = GetFuelMass() == DA_FUELMASS;

	xrSound = XRSound::CreateInstance(this);
	xrSound->LoadWav(XRSound::MainEngines, "XRSound/Deepstar/Engine.wav", XRSound::BothViewFar);
	xrSound->LoadWav(XRSound::HoverEngines, "XRSound/Deepstar/Engine.wav", XRSound::BothViewFar);

	xrSound->LoadWav(SoundID::HYDRAULICS, "XRSound/Default/Hydraulics1.wav", XRSound::BothViewClose);
	xrSound->LoadWav(SoundID::CLICK, "XRSound/Default/SwitchOn1.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::RCS_NORMAL, "XRSound/Default/RCS Config Normal.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::RCS_DOCKING, "XRSound/Default/RCS Config Docking.wav", XRSound::InternalOnly);

	xrSound->LoadWav(SoundID::GEAR_WHINE, "XRSound/Default/Gear Whine.wav", XRSound::BothViewClose);
	xrSound->LoadWav(SoundID::GEAR_DOWN_LOCKED, "XRSound/Default/Gear Down and Locked.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::GEAR_DOWN, "XRSound/Default/Gear Down.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::GEAR_UP_LOCKED, "XRSound/Default/Gear Up and Locked.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::GEAR_UP, "XRSound/Default/Gear Up.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::HOVER_DOORS_CLOSED, "XRSound/Default/Hover doors are closed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::HATCH_OPEN, "XRSound/Default/Hatch Open.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::HATCH_CLOSED, "XRSound/Default/Hatch Closed.wav", XRSound::InternalOnly);

	xrSound->LoadWav(SoundID::MAIN_FULL, "XRSound/Default/Main Fuel Tanks Full.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::MAIN_LOW, "XRSound/Default/Warning Main Fuel Low.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::MAIN_DEPLETED, "XRSound/Default/Warning Main Fuel Depleted.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::WARN_GEAR_UP, "XRSound/Default/Warning Gear is Up.wav", XRSound::InternalOnly);

	xrSound->LoadWav(SoundID::BAY_DOORS_CLOSED, "XRSound/Default/Bay Doors Are Closed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::SLOT_EMPTY, "XRSound/Default/Slot is Empty.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::SLOT_OCCUPIED, "XRSound/Default/Slot Is Full.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_RELEASED, "XRSound/Default/Cargo Deployed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_RELEASE_FAILED, "XRSound/Default/Cargo Deployment Failed.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_GRAPPLED, "XRSound/Default/Cargo Latched In Bay.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_GRAPPLE_NORANGE, "XRSound/Default/No Cargo in Grapple Range.wav", XRSound::InternalOnly);
	xrSound->LoadWav(SoundID::CARGO_GRAPPLE_FAILED, "XRSound/Default/Auto-Grapple Failed.wav", XRSound::InternalOnly);
}

void Aerolander::clbkMFDMode(int mfd, int mode) { oapiVCTriggerRedrawArea(-1, VCArea::MFD_KEYS); }

void Aerolander::clbkHUDMode(int mode) { oapiVCTriggerRedrawArea(-1, VCArea::HUD_KEYS); }

void Aerolander::clbkRCSMode(int mode) { oapiVCTriggerRedrawArea(-1, VCArea::RCS_KEYS); }

void Aerolander::clbkNavMode(int mode, bool active) 
{ 
	if (active && rcsConfig) DeactivateNavmode(mode);

	oapiVCTriggerRedrawArea(-1, VCArea::NAV_KEYS);
}

bool Aerolander::clbkVCRedrawEvent(int id, int ev, SURFHANDLE surf)
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

bool Aerolander::clbkDrawHUD(int mode, const HUDPAINTSPEC* hps, oapi::Sketchpad* skp)
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

void Aerolander::DrawInfoHUD(int x, int y, oapi::Sketchpad* skp)
{
	skp->Text(x, y, "G = Toggle the landing gear", 27);
	y += 20;

	skp->Text(x, y, "Ctrl + B = Toggle the airbrake", 30);
	y += 20;

	skp->Text(x, y, "Ctrl + U = Toggle the payload bay", 33);
	y += 20;

	skp->Text(x, y, "Ctrl + V = Toggle the hover doors", 33);
	y += 20;

	skp->Text(x, y, "Ctrl + Y = Toggle the egress hatch", 34);
	y += 20;

	skp->Text(x, y, "Alt + J = Toggle the RCS config", 31);
	y += 20;

	skp->Text(x, y, "Shift + C = Toggle the cargo mode", 33);

	y += 35;

	for (auto& system : systems)
	{
		const char* status;

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

		sprintf(buffer, "%s: %s", system.infoName, status);
		skp->Text(x, y, buffer, strlen(buffer));

		y += 20;
	}
}

void Aerolander::DrawCargoHUD(int x, int y, oapi::Sketchpad* skp)
{
	if (cargoData.selectedSlot == 0) skp->Text(x, y, "Selected slot: Centeral", 23);
	else
	{
		sprintf(buffer, "Selected slot: %d", cargoData.selectedSlot);
		skp->Text(x, y, buffer, strlen(buffer));
	}

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

void Aerolander::clbkPostStep(double simt, double simdt, double mjd)
{
	double fuelMass = GetFuelMass();

	// Main fuel full announcment
	if (fuelMass == DA_FUELMASS)
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

	// Set systems
	for (auto& system : systems)
	{
		if (system.status == SystemStatus::OPENING || system.status == SystemStatus::CLOSING)
		{
			// To run gear whine instead of normal hydraulics and gear up and locked and gear down sounds
			bool gearAnim = system.id == systems[SystemID::LANDING_GEAR].id;
			// To run hatch open and hatch closed sounds
			bool hatchAnim = (gearAnim ? false : system.id == systems[SystemID::EGRESS_HATCH].id);

			xrSound->PlayWav((gearAnim ? SoundID::GEAR_WHINE : SoundID::HYDRAULICS));

			double da = simdt * system.speed;

			if (system.status == SystemStatus::CLOSING)
			{
				if (system.proc > 0.0) system.proc = max(0.0, system.proc - da);
				else 
				{
					system.status = SystemStatus::CLOSED;

					if (gearAnim) xrSound->PlayWav(SoundID::GEAR_UP_LOCKED);
					else if (hatchAnim) xrSound->PlayWav(SoundID::HATCH_CLOSED);
				}
			}
			else
			{
				if (gearAnim && system.proc == 0) xrSound->PlayWav(SoundID::GEAR_DOWN);

				if (system.proc < 1.0) system.proc = min(1.0, system.proc + da);
				else 
				{
					system.status = SystemStatus::OPEN;
					if (hatchAnim) xrSound->PlayWav(SoundID::HATCH_OPEN);
				}
			}

			SetAnimation(system.id, system.proc);

			if (system.status == SystemStatus::OPEN || system.status == SystemStatus::CLOSED) xrSound->StopWav((gearAnim ? SoundID::GEAR_WHINE : SoundID::HYDRAULICS));
		}
	}

	// Set touchdown points based on landing gear status
	if (!gearDown && systems[SystemID::LANDING_GEAR].status == SystemStatus::OPEN)
	{
		SetTouchdownPoints(tdVtxGearDown, tdVtxN);
		SetMaxWheelbrakeForce(4e5);
		gearDown = true;
		xrSound->PlayWav(SoundID::GEAR_DOWN_LOCKED);
	}
	else if (gearDown && systems[SystemID::LANDING_GEAR].status != SystemStatus::OPEN)
	{
		xrSound->PlayWav(SoundID::GEAR_UP);
		SetTouchdownPoints(tdVtxGearUp, tdVtxN);
		SetMaxWheelbrakeForce(0);
		gearDown = false;
	}

	// Play gear is down warning if the gear isn't down below 300 meters with negative vertical speed
	if (!gearDown && GetAltitude(ALTMODE_GROUND) <= 300)
	{
		VECTOR3 vel;
		GetGroundspeedVector(FRAME_HORIZON, vel);

		if (vel.y <= 0) xrSound->PlayWav(SoundID::WARN_GEAR_UP);
	}

	// Set docking port based on payload doors status
	if (!dockPort.handle && systems[SystemID::PAYLOAD_BAY].status == SystemStatus::OPEN)
	{
		dockPort.handle = CreateDock(_V(0, 3.407, 5.55), _V(0, 1, 0), _V(0, 0, 1));
		EnableIDS(dockPort.handle, true);
		SetIDSChannel(dockPort.handle, dockPort.idsChannel);

		ucso->SetSlotDoor(true);
	}
	else if (dockPort.handle && systems[SystemID::PAYLOAD_BAY].status != SystemStatus::OPEN)
	{
		dockPort.idsChannel = oapiGetNavChannel(GetIDS(dockPort.handle));
		EnableIDS(dockPort.handle, false);
		DelDock(dockPort.handle);
		dockPort.handle = nullptr;

		ucso->SetSlotDoor(false);
	}

	// Set hover engine based on hover doors status
	if (!hoverEngine.groupHandle && systems[SystemID::HOVER_DOORS].status == SystemStatus::OPEN) hoverEngine.groupHandle = CreateThrusterGroup(&hoverEngine.handle, 1, THGROUP_HOVER);
	else if (hoverEngine.groupHandle && systems[SystemID::HOVER_DOORS].status != SystemStatus::OPEN)
	{
		SetThrusterGroupLevel(THGROUP_HOVER, 0);
		DelThrusterGroup(hoverEngine.groupHandle);
		hoverEngine.groupHandle = nullptr;
	}

	// Set vessel status to landed
	if (GroundContact() && !(GetFlightStatus() & 1) && GetGroundspeed() <= 0.2)
	{
		VECTOR3 thrustVector;

		if (!GetThrustVector(thrustVector)) SetStatusLanded(this, gearDown ? 6.52676 : 3.091);
	}

	if (cargoData.timer < 5) cargoData.timer += simdt;
}

int Aerolander::clbkConsumeBufferedKey(DWORD key, bool down, char* kstate)
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
				if (SlotAvailable())
				{
					switch (ucso->AddCargo(cargoData.selectedCargo, cargoData.selectedSlot))
					{
					case UCSO::Vessel::GRAPPLE_SUCCEEDED:
						cargoData.message = "Success: The cargo was added.";
						xrSound->PlayWav(SoundID::CARGO_GRAPPLED);
						break;

					case UCSO::Vessel::GRAPPLE_SLOT_CLOSED:
						cargoData.message = "Error: The payload bay doors are closed.";
						xrSound->PlayWav(SoundID::BAY_DOORS_CLOSED);
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
				if (SlotAvailable())
				{
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

					case UCSO::Vessel::GRAPPLE_SLOT_CLOSED:
						cargoData.message = "Error: The payload bay doors are closed.";
						xrSound->PlayWav(SoundID::BAY_DOORS_CLOSED);
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
				}

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_R:
				switch (ucso->ReleaseCargo(cargoData.selectedSlot))
				{
				case UCSO::Vessel::RELEASE_SUCCEEDED:
					cargoData.message = "Success: The cargo was released.";
					xrSound->PlayWav(SoundID::CARGO_RELEASED);
					break;

				case UCSO::Vessel::NO_EMPTY_POSITION:
					cargoData.message = "Error: No empty position nearby.";
					break;

				case UCSO::Vessel::RELEASE_SLOT_CLOSED:
					cargoData.message = "Error: The payload bay doors are closed.";
					xrSound->PlayWav(SoundID::BAY_DOORS_CLOSED);
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
				else cargoData.message = "Error: No packable cargo in range or the packing failed.";

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_U:
				if (ucso->UnpackCargo()) cargoData.message = "Success: The nearest cargo was unpacked.";
				else cargoData.message = "Error: No unpackable cargo in range or the unpacking failed.";

				cargoData.timer = 0;
				return 1;

			case OAPI_KEY_F:
			{
				double requiredMass = GetMaxFuelMass() - GetFuelMass();

				double drainedMass = ucso->DrainCargoResource("fuel", requiredMass);

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
				cargoData.selectedSlot > 0 ? cargoData.selectedSlot-- : cargoData.selectedSlot = 15;
				return 1;

			// Next slot
			case OAPI_KEY_DOWN:
				cargoData.selectedSlot < 15 ? cargoData.selectedSlot++ : cargoData.selectedSlot = 0;
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

	System* system = nullptr;

	if (KEYMOD_CONTROL(kstate))
	{
		switch (key)
		{
		// Airbrake
		case OAPI_KEY_B:
			system = &systems[SystemID::AIRBRAKE];
			break;

		// Payload bay
		case OAPI_KEY_U:
			system = &systems[SystemID::PAYLOAD_BAY];
			break;

		// Hover doors
		case OAPI_KEY_V:
			system = &systems[SystemID::HOVER_DOORS];
			break;

		// Egress hatch
		case OAPI_KEY_Y:
			system = &systems[SystemID::EGRESS_HATCH];
			break;

		default: return 0;
		}
	}
	else
	{
		switch (key)
		{
		// Landing gear
		case OAPI_KEY_G:
			system = &systems[SystemID::LANDING_GEAR];
			break;

		case OAPI_KEY_DECIMAL:
		case OAPI_KEY_NUMPAD0:
			if (!hoverEngine.groupHandle) xrSound->PlayWav(SoundID::HOVER_DOORS_CLOSED);
			return 0;

		default: return 0;
		}
	}

	if (!system) return 0;

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

bool Aerolander::clbkVCMouseEvent(int id, int ev, VECTOR3& p)
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
	}
}

void Aerolander::SetRCSConfig()
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
		for (int thruster = 0; thruster < 8; thruster++) SetThrusterMax0(rcsHandles[thruster], DA_MAXRCSTH * 0.5);

		for (int thruster = 8; thruster < 12; thruster++) SetThrusterMax0(rcsHandles[thruster], DA_MAXRCSTH);

		for (int thruster = 12; thruster < 15; thruster++) SetThrusterMax0(rcsHandles[thruster], DA_MAXRCSTH * 0.667);

		SetThrusterMax0(rcsHandles[15], DA_MAXRCSTH);
		SetThrusterMax0(rcsHandles[16], DA_MAXRCSTH);

		groupHandles[0] = rcsHandles[2];
		groupHandles[1] = rcsHandles[3];
		groupHandles[2] = rcsHandles[4];
		groupHandles[3] = rcsHandles[5];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHDOWN); // Original: THGROUP_ATT_PITCHUP

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[1];
		groupHandles[2] = rcsHandles[6];
		groupHandles[3] = rcsHandles[7];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHUP); // Original: THGROUP_ATT_PITCHDOWN

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[3];
		groupHandles[2] = rcsHandles[5];
		groupHandles[3] = rcsHandles[6];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_YAWLEFT); // Original: THGROUP_ATT_BANKRIGHT

		groupHandles[0] = rcsHandles[1];
		groupHandles[1] = rcsHandles[2];
		groupHandles[2] = rcsHandles[4];
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

		groupHandles[0] = rcsHandles[15];
		groupHandles[1] = rcsHandles[16];
		CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_UP); // Original: THGROUP_ATT_FORWARD

		groupHandles[0] = rcsHandles[12];
		groupHandles[1] = rcsHandles[13];
		groupHandles[2] = rcsHandles[14];
		CreateThrusterGroup(groupHandles, 3, THGROUP_ATT_DOWN); // Original: THGROUP_ATT_BACK
	}
	else
	{
		for (int thruster = 0; thruster < 8; thruster++) SetThrusterMax0(rcsHandles[thruster], DA_MAXRCSTH);

		for (int thruster = 8; thruster < 12; thruster++) SetThrusterMax0(rcsHandles[thruster], DA_MAXRCSTH * 2);

		for (int thruster = 12; thruster < 15; thruster++) SetThrusterMax0(rcsHandles[thruster], DA_MAXRCSTH * 1.33);

		SetThrusterMax0(rcsHandles[15], DA_MAXRCSTH * 2);
		SetThrusterMax0(rcsHandles[16], DA_MAXRCSTH * 2);

		groupHandles[0] = rcsHandles[2];
		groupHandles[1] = rcsHandles[3];
		groupHandles[2] = rcsHandles[4];
		groupHandles[3] = rcsHandles[5];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHUP);

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[1];
		groupHandles[2] = rcsHandles[6];
		groupHandles[3] = rcsHandles[7];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_PITCHDOWN);

		groupHandles[0] = rcsHandles[0];
		groupHandles[1] = rcsHandles[3];
		groupHandles[2] = rcsHandles[5];
		groupHandles[3] = rcsHandles[6];
		CreateThrusterGroup(groupHandles, 4, THGROUP_ATT_BANKRIGHT);

		groupHandles[0] = rcsHandles[1];
		groupHandles[1] = rcsHandles[2];
		groupHandles[2] = rcsHandles[4];
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

		groupHandles[0] = rcsHandles[15];
		groupHandles[1] = rcsHandles[16];
		CreateThrusterGroup(groupHandles, 2, THGROUP_ATT_FORWARD);

		groupHandles[0] = rcsHandles[12];
		groupHandles[1] = rcsHandles[13];
		groupHandles[2] = rcsHandles[14];
		CreateThrusterGroup(groupHandles, 3, THGROUP_ATT_BACK);
	}
}

bool Aerolander::SlotAvailable()
{
	bool available = true;
	
	// If a normal slot (except 1 and 2) is selected and there is cargo in the centeral slot
	if (cargoData.selectedSlot > 2 && ucso->GetCargoInfo(0).valid) available = false;
	else if (cargoData.selectedSlot == 0)
	{
		// If the centeral slot is seleceted, check every normal slot (except 1 and 2) for cargo
		for (int slot = 3; slot < 16; slot++)
			if (ucso->GetCargoInfo(slot).valid) { available = false; break; }
	}

	if (!available)
	{
		xrSound->PlayWav(SoundID::SLOT_OCCUPIED);

		if (cargoData.selectedSlot == 0) cargoData.message = "Error: Slots 3 to 15 must be empty.";
		else cargoData.message = "Error: The centeral slot must be empty.";

		cargoData.timer = 0;
	}

	return available;
}

void Aerolander::VLiftCoeff(VESSEL* v, double aoa, double M, double Re, void* context, double* cl, double* cm, double* cd)
{
	int i;
	const int nabsc = 9;
	static const double AOA[nabsc] = { -PI, -60 * RAD, -30 * RAD, -2 * RAD, 15 * RAD, 20 * RAD, 25 * RAD, 60 * RAD, PI };
	static const double CL[nabsc] = { 0,      0,   -0.4,      0,    0.7,     1,   0.8,     0,      0 };
	static const double CM[nabsc] = { 0,      0,  0.014, 0.0039, -0.006,-0.008,-0.010,     0,      0 };
	for (i = 0; i < nabsc - 1 && AOA[i + 1] < aoa; i++);
	double f = (aoa - AOA[i]) / (AOA[i + 1] - AOA[i]);
	*cl = CL[i] + (CL[i + 1] - CL[i]) * f;  // aoa-dependent lift coefficient
	*cm = CM[i] + (CM[i + 1] - CM[i]) * f;  // aoa-dependent moment coefficient
	double saoa = sin(aoa);
	double pd = 0.015 + 0.4 * saoa * saoa;  // profile drag
	*cd = pd + oapiGetInducedDrag(*cl, 0.85, 0.7) + oapiGetWaveDrag(M, 0.75, 1.0, 1.1, 0.04);
}

void Aerolander::HLiftCoeff(VESSEL* v, double beta, double M, double Re, void* context, double* cl, double* cm, double* cd)
{
	int i;
	const int nabsc = 8;
	static const double BETA[nabsc] = { -PI, -135 * RAD, -PI05, -45 * RAD, 45 * RAD, PI05, 135 * RAD, PI };
	static const double CL[nabsc] = { 0,    +0.3,      0,   -0.3,  +0.3,     0,   -0.3,      0 };
	for (i = 0; i < nabsc - 1 && BETA[i + 1] < beta; i++);
	*cl = CL[i] + (CL[i + 1] - CL[i]) * (beta - BETA[i]) / (BETA[i + 1] - BETA[i]);
	*cm = 0.0;
	*cd = 0.015 + oapiGetInducedDrag(*cl, 1, 0.6) + oapiGetWaveDrag(M, 0.75, 1.0, 1.1, 0.04);
}