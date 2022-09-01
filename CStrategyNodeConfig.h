#pragma once

#ifndef CSTRATEGY_NODE_CONFIG_H_
#define CSTRATEGY_NODE_CONFIG_H_

#include "CStrategyNodeConfigItem.h"
#include <memory>
#include <map>


class CStrategyNodeConfig
{
public:
	std::map<std::string, std::shared_ptr<CStrategyNodeConfigItem>> m_configItems;

	bool Init(GameType gmType); //从配置文件中加载
	const std::shared_ptr<CStrategyNodeConfigItem> GetItemByName(const std::string & sNodeName) const;
};

#endif