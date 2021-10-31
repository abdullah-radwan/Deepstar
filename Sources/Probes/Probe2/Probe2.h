// ==============================================================
//                 ORBITER MODULE: Deepstar Probe 2
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Probe2.h
// Class interface for Deepstar Probe 2 vessel class
//
// ==============================================================

#pragma once
#include "../BaseProbe.h"

class Probe2 : public BaseProbe
{
public:
	Probe2(OBJHANDLE hVessel, int flightmodel) : BaseProbe(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	void DefineEngines();
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DP_SIZE = 6;                   // mean radius [m]
const VECTOR3 DP_CS = { 3.34, 3.05, 1.12 };  // x,y,z cross sections [m^2]
const VECTOR3 DP_PMI = { 0.59, 0.58, 0.16 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DP_EMPTYMASS = 2e3;            // empty vessel mass [kg]
const double  DP_FUELMASS = 3e3;             // max fuel mass [kg]
const double  DP_ISP = 2e4;                  // fuel-specific impulse [m/s]
const double  DP_MAXMAINTH = 3e3;            // max main engien thrust [n]
const double  DP_MAXRCSTH = 2e2;             // max RCS engine thrust [n]

// Define impact convex hull
static const DWORD tdVtxN = 12;
static TOUCHDOWNVTX tdVtx[tdVtxN] =
{
	{ _V(0, -0.675503, 1.34759),          4.9e7, 8.9e5, 3.0, 3},
	{ _V(-0.04135, -0.676327, -0.644303), 4.9e7, 8.9e5, 3.0, 3},
	{ _V(0.04135, -0.676327, -0.644303),  4.9e7, 8.9e5, 3.0, 3},
	{ _V(-0.573858, 0, -0.667761),        4.9e7, 8.9e5, 3.0},
	{ _V(0.573858, 0, -0.667761),         4.9e7, 8.9e5, 3.0},
	{ _V(-0.573858, 0, 0.925197),         4.9e7, 8.9e5, 3.0},
	{ _V(0.573858, 0, 0.925197),          4.9e7, 8.9e5, 3.0},
	{ _V(-0.04135, 0.676327, -0.644303),  4.9e7, 8.9e5, 3.0},
	{ _V(0.04135, 0.676327, -0.644303),   4.9e7, 8.9e5, 3.0},
	{ _V(-0.04135, 0.676327, 0.92518),    4.9e7, 8.9e5, 3.0},
	{ _V(0.04135, 0.676327, 0.92518),     4.9e7, 8.9e5, 3.0},
	{ _V(0, 0, 1.55203),                  4.9e7, 8.9e5, 3.0}
};