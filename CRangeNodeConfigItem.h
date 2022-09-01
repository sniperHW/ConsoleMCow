#pragma once

#ifndef CRANGE_NODE_CONFIG_ITEM_H_
#define CRANGE_NODE_CONFIG_ITEM_H_

#include "globledef.h"

class CRangeNodeConfigItem
{
public:
	std::string m_sReplaceNodeName;
	Round m_round;

	CRangeNodeConfigItem(std::string sReplaceNodeName, Round round);
	CRangeNodeConfigItem();
};

#endif