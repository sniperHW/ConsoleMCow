#pragma once

#ifndef CSOLVER_CONFIG_H_
#define CSOLVER_CONFIG_H_

#include "globledef.h"
#include "json/json.h"
#include "CRange.h"

class CSolverConfig
{
public:
	Stacks m_stacks;
	std::string m_sBoard;
	CRange* m_pRange;
	Json::Value m_strategyTree;
};

#endif