#pragma once

#ifndef CSTRATEGY_TREE_CONFIG_H_
#define CSTRATEGY_TREE_CONFIG_H_

#include "json/json.h"
#include"globledef.h"

class CStrategyTreeConfig
{
public:
	Json::Value m_Config;

	bool Init(GameType gmType);
	Json::Value GetConfigItem(const std::string & sPreflopType);
};

#endif