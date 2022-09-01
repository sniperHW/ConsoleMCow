#pragma once

#ifndef CRANGE_NODE_CONFIG_H_
#define CRANGE_NODE_CONFIG_H_

#include "CRangeNodeConfigItem.h"
#include <memory>
#include <map>


class CRangeNodeConfig
{
public:
	std::map<std::string, std::shared_ptr<CRangeNodeConfigItem>> m_configItems;

	bool Init(GameType gmType); //从配置文件中加载
	const std::shared_ptr<CRangeNodeConfigItem> GetItemByName(const std::string& sNodeName) const;
};

#endif