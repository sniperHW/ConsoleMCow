#pragma once

#ifndef CWDEBUG_H_
#define CWDEBUG_H_

#include "globledef.h"
#include "CStrategyItem.h"
#include <vector>

class CWdebug
{
public:
	CWdebug();
	~CWdebug();
	void AddLog(const std::string & sLog);
	void AddRange(const RangeData & rangeData);
	void AddAction(const StrategyData & strategyData);
	void Add(const std::vector<CStrategyItem>&strategy);
	void Clear(bool log = 1, bool data = 1);
};

#endif