#pragma once

#ifndef CRANGE_NODE_CONFIG_H_
#define CRANGE_NODE_CONFIG_H_

#include "globledef.h"
#include <utility>

class CRangeNodeConfigItem
{
public:
	std::string m_sReplaceNodeName;
	bool blReplace;
};


class CRangeNodeConfig
{
private:
	std::map<std::string, std::shared_ptr<CRangeNodeConfigItem>> m_configItems;
	std::vector<std::pair<std::string, std::shared_ptr<CRangeNodeConfigItem>>> m_regexConfigItems;

public:
	bool Init(GameType gmType); //从配置文件中加载
	const std::shared_ptr<CRangeNodeConfigItem> GetItemByName(const std::string& sNodeName) const;

	//首先从map中查找，再逐个匹配正则
	std::string GetReplace(const std::string& sNodeName); //空代表没有替换，维持原名称

};

#endif