// ==============================================================
//                 ORBITER MODULE: Deepstar Spacehab
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Spacehab.h
// Class interface for Deepstar Spacehab vessel class
//
// ==============================================================

#pragma once
#include <UCSO/CustomCargo.h>

class Spacehab : public VESSEL4, public UCSO::CustomCargo
{
public:
	Spacehab(OBJHANDLE hVessel, int flightmodel);

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostStep(double simt, double simdt, double mjd);

	CargoInfo GetCargoInfo() override;
	OBJHANDLE GetCargoHandle() override;
	ATTACHMENTHANDLE GetCargoAttachmentHandle() override;

private:
	UCSO::CustomCargo::CargoInfo cargoInfo;
	ATTACHMENTHANDLE attachHandle = nullptr;
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DH_SIZE = 5;                     // mean radius [m]
const VECTOR3 DH_CS = { 10.77, 21.49, 14.87 }; // x,y,z cross sections [m^2]
const VECTOR3 DH_PMI = { 1.76, 3.55, 2.87 };   // principal moments of inertia (mass-normalised) [m^2]
const double  DH_EMPTYMASS = 2.5e3;            // empty vessel mass [kg]
const double  DH_FUELMASS = 50;                // max fuel mass [kg]
const double  DH_ISP = 1e8;                    // fuel-specific impulse [m/s]
const double  DH_MAXRCSTH = 3e2;		   	   // max RCS engine thrust [n]

// Define impact convex hull
static const DWORD tdVtxN = 12;
static TOUCHDOWNVTX tdVtx[tdVtxN] =
{
	{ _V(-0.013532, -1.24484, 1.92571),  2.5e7, 4.5e5, 3.0, 3},
	{ _V(-3.04674, -1.24572, -1.87336),  2.5e7, 4.5e5, 3.0, 3},
	{ _V(3.01968, -1.24572, -1.87336),   2.5e7, 4.5e5, 3.0, 3},
	{ _V(-3.052, 0.565784, -1.896),      2.5e7, 4.5e5, 3.0},
	{ _V(3.02494, 0.565784, -1.896),     2.5e7, 4.5e5, 3.0},
	{ _V(-2.4414, 0.554874, 1.9458),     2.5e7, 4.5e5, 3.0},
	{ _V(2.41434, 0.554874, 1.9458),     2.5e7, 4.5e5, 3.0},
	{ _V(-1.08367, 1.47289, -1.87412),   2.5e7, 4.5e5, 3.0},
	{ _V(1.05661, 1.47289, -1.87411),    2.5e7, 4.5e5, 3.0},
	{ _V(-1.08347, 1.47289, 1.92341),    2.5e7, 4.5e5, 3.0},
	{ _V(1.05641, 1.47289, 1.92341),     2.5e7, 4.5e5, 3.0},
	{ _V(-0.013689, -0.021371, 1.97227), 2.5e7, 4.5e5, 3.0}
};