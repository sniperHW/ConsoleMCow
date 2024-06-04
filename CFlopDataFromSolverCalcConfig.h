#pragma once

#ifndef CFLOP_DATA_FROM_SOLVERCALC_H_
#define CFLOP_DATA_FROM_SOLVERCALC_H_

#include "globledef.h"

class CFlopDataFromSolverCalcConfig
{
public:
	bool Init(GameType gmType);
	bool IsFrSolverCalc(const std::string sAbbrName);

private:
	std::set<std::string> m_sAbbrNames;
};

#endif

