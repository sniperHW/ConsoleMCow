#pragma once

#ifndef CMULTI_STRATEGY_H_
#define CMULTI_STRATEGY_H_

#include "globledef.h"
#include "util.h"

class CMultiStrategyConfigItem
{
public:
	//conditions
	Round m_Round;
	std::string m_sActionLineCondition; //�ж���ƥ����������������

	int m_nPlayerCount = 0;;
	CompareSymbol m_PlayerCountComparer = cs_equal;

	double m_dlBetSize = 0;
	CompareSymbol m_BetSizeComparer = cs_equal;

	double m_dlSpr = 0;
	CompareSymbol m_SprComparer = cs_equal;

	Multi_Position m_Position;
	Multi_HeroActive m_HeroActive;

	//actions
	ActionType m_ActionType;
	double m_dlHeroBetSize;
	RankGroup m_ActionRank; //>=���ȡ�ж�
	RankGroup m_AmbushRank; //>=��������ж���Ϊcall

	double m_dlBluffRatio = 0;	//��û�����г����ƣ��ж�Ϊfoldʱ�����˱���bet��raise
	double m_dlBluffSize = 0;
};

class CMultiStrategy
{
private:
	std::vector<CMultiStrategyConfigItem>  m_MultiStrategyConfigItems;
public:
	bool Init();
	Action GetHeroAction(const MultiCondition& multiCondition, const PokerEvaluate& pokerEv);

};

#endif


