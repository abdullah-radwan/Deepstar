#pragma once
#include <Orbitersdk.h>

struct System
{
	const char* infoName;
	char* scnName;

	UINT id;
	int status;
	double proc;
	double speed;
};

struct MainFuel
{
	bool full = false, low = false, depleted = false;
};

struct CargoData
{
	int selectedSlot = 0, selectedCargo = 0;
	const char* message = nullptr;
	double timer = 0;
};

namespace SystemStatus { enum { CLOSED, OPEN, OPENING, CLOSING }; }

namespace HUDMode { enum { DEFAULT, INFO, CARGO }; }

namespace VCCtrlSet { enum { MFD_KEYS = 101, MFD_CTRLS, HUD_KEYS, RCS_KEYS, NAV_KEYS }; }

namespace VCArea { enum { MFD_KEYS = 1, HUD_KEYS, RCS_KEYS, NAV_KEYS }; }

namespace VCView { enum { SEAT_LEFT, SEAT_RIGHT, TOP_LEFT, TOP_RIGHT}; }

static MATRIX3 RotationMatrix(VECTOR3 angles)
{
	MATRIX3 m;
	MATRIX3 RM_X, RM_Y, RM_Z;

	RM_X = _M(1, 0, 0, 0, cos(angles.x), -sin(angles.x), 0, sin(angles.x), cos(angles.x));
	RM_Y = _M(cos(angles.y), 0, sin(angles.y), 0, 1, 0, -sin(angles.y), 0, cos(angles.y));
	RM_Z = _M(cos(angles.z), -sin(angles.z), 0, sin(angles.z), cos(angles.z), 0, 0, 0, 1);

	m = mul(RM_X, mul(RM_Y, RM_Z));

	return m;
}

static void SetGroundRotation(VESSELSTATUS2& status, double height)
{
	MATRIX3 rot1 = RotationMatrix({ 0, PI05 - status.surf_lng, 0 });
	MATRIX3 rot2 = RotationMatrix({ -status.surf_lat, 0, 0 });
	MATRIX3 rot3 = RotationMatrix({ 0, 0, PI + status.surf_hdg });
	MATRIX3 rot4 = RotationMatrix({ PI05, 0, 0 });
	MATRIX3 RotMatrix_Def = mul(rot1, mul(rot2, mul(rot3, rot4)));

	status.arot.x = atan2(RotMatrix_Def.m23, RotMatrix_Def.m33);
	status.arot.y = -asin(RotMatrix_Def.m13);
	status.arot.z = atan2(RotMatrix_Def.m12, RotMatrix_Def.m11);

	status.vrot.x = height;
}

static void SetStatusLanded(VESSEL4* vessel, double height)
{
	VESSELSTATUS2 status;
	memset(&status, 0, sizeof(status));
	status.version = 2;
	vessel->GetStatusEx(&status);
	status.status = 1;

	SetGroundRotation(status, height);

	vessel->DefSetStateEx(&status);
}