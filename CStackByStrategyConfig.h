#pragma once
#ifndef CSTACK_BY_STRATEGY_CONFIG_H_
#define CSTACK_BY_STRATEGY_CONFIG_H_

#include "globledef.h"
#include <memory>
#include <map>

//与solver预存解对应，用于将solver解的size转为比例
//数据只保持preflop策略名称对应的筹码值，而获取数据的参数可为preflop或flop策略名称，如果是preflop则直接取值，而flop则用preflop初始值和flop的序列来计算
//配置数据从范围文件中加载
class CStackByStrategyConfig
{
private:
	std::map<std::string, std::shared_ptr<Stacks>> m_configItems; //key为preflop或flop策略名称

public:
	bool Init(GameType gmType); 
	const std::shared_ptr<Stacks> GetItemByName(const std::string& sNodeName) const; //只接受preflop和flop
};

#endif