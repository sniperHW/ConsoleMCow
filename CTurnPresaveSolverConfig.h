#pragma once

#ifndef CTURN_PRESAVE_SOLVER_CONFIG_H_
#define CTURN_PRESAVE_SOLVER_CONFIG_H_

#include "globledef.h"

class CTurnPresaveSolverConfig
{
public:
	bool Init(GameType gmType);
	bool IsPresaveSolver(const std::string sAbbrName);

private:
	std::set<std::string> m_sAbbrNames;
};


#endif