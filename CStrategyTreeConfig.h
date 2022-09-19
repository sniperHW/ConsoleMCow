#pragma once

#ifndef CSTRATEGY_TREE_CONFIG_H_
#define CSTRATEGY_TREE_CONFIG_H_

#include "json/json.h"
#include"globledef.h"
#include <vector>

class CStrategyTreeConfig
{
private:
	Json::Value m_Config;

public:
	bool Init(GameType gmType);
	Json::Value GetConfigItem(const std::string & sAbbrName);

};

#endif