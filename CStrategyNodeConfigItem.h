#pragma once

#ifndef CSTRATEGY_NODE_CONFIG_ITEM_H_
#define CSTRATEGY_NODE_CONFIG_ITEM_H_

#include "globledef.h"

class CStrategyNodeConfigItem
{
public:
	std::string m_sReplaceNodeName;
	std::string m_sSpecialProcessing; //待定
	Round m_round;

	CStrategyNodeConfigItem(std::string sReplaceNodeName, std::string sSpecialProcessing, Round round);
	CStrategyNodeConfigItem();
};

#endif