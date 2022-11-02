#pragma once

#ifndef CSTRATEGY_NODE_CONFIG_H_
#define CSTRATEGY_NODE_CONFIG_H_

#include "globledef.h"
#include <utility>

class CStrategyNodeConfigItem
{
public:
	std::string m_sReplaceNodeName;
	std::string m_sSpecialProcessing;
	bool blReplace;
};

class CStrategyNodeConfig
{
private:
	std::map<std::string, std::shared_ptr<CStrategyNodeConfigItem>> m_configItems;
	std::vector<std::pair<std::string, std::shared_ptr<CStrategyNodeConfigItem>>> m_regexConfigItems;

public:
	bool Init(GameType gmType); //从配置文件中加载

	//首先从map中查找，再逐个匹配正则
	std::string GetReplace(const std::string& sNodeName); //空代表没有替换，维持原名称，返回"special"代表无需加载策略文件（special中指定）
	std::string GetSpecial(const std::string& sNodeName); //空代表没有处理
};

#endif