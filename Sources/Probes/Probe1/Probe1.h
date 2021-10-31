// ==============================================================
//                 ORBITER MODULE: Deepstar Probe 1
//          Copyright (C) 2021 80mileshigh and Abdullah Radwan
//                   All rights reserved
//
// Probe1.h
// Class interface for Deepstar Probe 1 vessel class
//
// ==============================================================

#pragma once
#include "../BaseProbe.h"

class Probe1 : public BaseProbe
{
public:
	Probe1(OBJHANDLE hVessel, int flightmodel) : BaseProbe(hVessel, flightmodel) {}
	void clbkSetClassCaps(FILEHANDLE cfg);

private:
	void DefineEngines();
	void DefineSystems();
};

// ==============================================================
// Vessel parameters
// ==============================================================
const double  DP_SIZE = 6;                   // mean radius [m]
const VECTOR3 DP_CS = { 2.70, 2.79, 1.06 };  // x,y,z cross sections [m^2]
const VECTOR3 DP_PMI = { 0.62, 0.64, 0.17 }; // principal moments of inertia (mass-normalised) [m^2]
const double  DP_EMPTYMASS = 2e3;            // empty vessel mass [kg]
const double  DP_FUELMASS = 3e3;             // max fuel mass [kg]
const double  DP_ISP = 2e4;                  // fuel-specific impulse [m/s]
const double  DP_MAXMAINTH = 3e3;            // max main engien thrust [n]
const double  DP_MAXRCSTH = 2e2;             // max RCS engine thrust [n]

// Define impact convex hull
static const DWORD tdVtxN = 12;
static TOUCHDOWNVTX tdVtx[tdVtxN] =
{
	{ _V(0, -0.344509, 1.37311),          4.9e7, 8.9e5, 3.0, 3},
	{ _V(-0.149879, -0.515298, -1.1288),  4.9e7, 8.9e5, 3.0, 3},
	{ _V(0.149879, -0.515298, -1.1288),   4.9e7, 8.9e5, 3.0, 3},
	{ _V(-0.52384, -0.141337, -1.1288),   4.9e7, 8.9e5, 3.0},
	{ _V(0.52384, -0.141337, -1.1288),    4.9e7, 8.9e5, 3.0},
	{ _V(-0.523839, 0.158421, 0.289343),  4.9e7, 8.9e5, 3.0},
	{ _V(0.523839, 0.158421, 0.289343),   4.9e7, 8.9e5, 3.0},
	{ _V(-0.149879, -0.515298, -1.1288),  4.9e7, 8.9e5, 3.0},
	{ _V(0.149879, -0.515298, -1.1288),   4.9e7, 8.9e5, 3.0},
	{ _V(-0.149878, -0.515298, 0.289343), 4.9e7, 8.9e5, 3.0},
	{ _V(0.149878, -0.515298, 0.289343),  4.9e7, 8.9e5, 3.0},
	{ _V(0, 0.008542, 1.47535),           4.9e7, 8.9e5, 3.0}
};