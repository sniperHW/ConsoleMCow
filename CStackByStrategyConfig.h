#pragma once
#ifndef CSTACK_BY_STRATEGY_CONFIG_H_
#define CSTACK_BY_STRATEGY_CONFIG_H_

#include "globledef.h"


//与solver预存解对应，用于将solver解的size转为比例
class CStackByStrategyConfig
{
private:
	std::map<std::string, std::shared_ptr<Stacks>> m_configItems; //key为preflop或flop策略名称

public:
	bool Init(GameType gmType); 
	const std::shared_ptr<Stacks> GetItemByName(const std::string& sNodeName) const; //只接受preflop和flop,sNodeName转为withoutBoard匹配
};

#endif