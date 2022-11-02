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
	std::vector<std::pair<double, std::shared_ptr<CRTTreeConfigItem>>> m_RTTreeConfigItems; //spr��Ӧ��
	std::vector<std::pair<std::string, CFlopTreeConfigItem>> m_FlopTreeConfigItems;	//���NodeNameʱ�ã�ֻ��flop��

	void ParseDigitals(const std::string& sDigitals, std::vector<double>& v);

public:
	bool Init(GameType gmType);
	void GetFlopCandidateRatios(const std::string& sPreflopName, std::vector<double>& candidateRatios, const SpaceMode mode);	//flop wizardģʽ��turn solve presaveģʽ�²��ã����ؿմ����ᱻʹ�õ�����
	std::shared_ptr<CRTTreeConfigItem> GetRTTreeConfig(const Stacks& stack);	//��sprƥ��
};

#endif
