#pragma once

#ifndef CSTRATEGY_TREE_CONFIG_H_
#define CSTRATEGY_TREE_CONFIG_H_

#include"globledef.h"
#include <utility>


class CRTTreeConfigItem
{
public:
	std::vector<double> m_turnBet;
	std::vector<double> m_turnRaise;
	std::vector<double> m_turnDonk;
	std::vector<double> m_riverBet;
	std::vector<double> m_riverRaise;
	std::vector<double> m_riverDonk;
};

class CFlopTreeConfigItem
{
public:
	std::vector<double> m_flopBet;
	std::vector<double> m_flopRaise;
	std::vector<double> m_flopDonk;
};

class CStrategyTreeConfig
{
private:
	std::vector<std::pair<double, std::shared_ptr<CRTTreeConfigItem>>> m_RTTreeConfigItems; //spr对应树
	std::vector<std::pair<std::string, CFlopTreeConfigItem>> m_FlopTreeConfigItems;	//组合NodeName时用，只在flop用

	void ParseDigitals(const std::string& sDigitals, std::vector<double>& v);

public:
	bool Init(GameType gmType);
	void GetFlopCandidateRatios(const std::string& sPreflopName, std::vector<double>& candidateRatios, const SpaceMode mode);	//flop wizard模式和turn solve presave模式下才用，返回空代表不会被使用的数据
	std::shared_ptr<CRTTreeConfigItem> GetRTTreeConfig(const Stacks& stack);	//按spr匹配
};

#endif
