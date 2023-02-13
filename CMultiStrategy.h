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
	std::string m_sActionLineCondition; //行动线匹配条件的正则条件

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
	RankGroup m_ActionRank; //>=则采取行动
	RankGroup m_AmbushRank; //>=则埋伏，行动变为call

	double m_dlBluffRatio = 0;	//当没有命中成手牌，行动为fold时，按此比例bet或raise
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


