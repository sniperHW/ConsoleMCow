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
	CompareSymbol m_PlayerCountComparer = cs_large;

	double m_dlBetSize = 0;
	CompareSymbol m_BetSizeComparer = cs_large;

	double m_dlSpr = 0;
	CompareSymbol m_SprComparer = cs_large;

	Multi_Position m_Position;
	Multi_HeroActive m_HeroActive;

	std::vector<PublicCondition> m_PublicConditions; //公牌条件     格式：&&nNeedtoFlush=2,&&nNeedtoStraight>2,||nPair=0  第一个逻辑符为||（默认为true），前面的运算结果再和后面的运算，相当于前面的加了括号

	//actions
	ActionType m_ActionType;
	double m_dlHeroBetSize = 0;
	RankGroup m_ActionRank; //>=则采取行动
	RankGroup m_AmbushRank; //>=则埋伏，行动变为call

	double m_dlBluffRatio = 0;	//当没有命中成手牌，行动为fold时，按此比例bet或raise
	double m_dlBluffSize = 0;
};

//为分析对手模式保留的行动记录
class CActionHistoryItem
{
public:
	MultiCondition m_multiCondition;
	PokerEvaluate m_pokerEv;
	Action m_action;
};

class CMultiStrategy
{
private:
	std::vector<CMultiStrategyConfigItem>  m_MultiStrategyConfigItems;
	std::vector<CActionHistoryItem> m_ActionHistorys;

	double GetRemainStack(double dEstack, const std::vector<std::pair<Position, Action>>& posActions, const Position heroPosition);
	bool ComparePublicCondition(const PublicStruct& publicStruct, std::vector<PublicCondition>& PublicConditions);
	Action AdjustActionByMode(const Action& oldAction, const MultiCondition& multiCondition, const PokerEvaluate& pokerEv); //识别模式并调整策略

public:
	bool Init();
	Action GetHeroAction(const MultiCondition& multiCondition, const PokerEvaluate& pokerEv);
	static std::vector<PublicCondition> str2PublicConditions(const std::string& sSymbol);

};

#endif


