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
	CompareSymbol m_PlayerCountComparer = cs_large;

	double m_dlBetSize = 0;
	CompareSymbol m_BetSizeComparer = cs_large;

	double m_dlSpr = 0;
	CompareSymbol m_SprComparer = cs_large;

	Multi_Position m_Position;
	Multi_HeroActive m_HeroActive;

	std::vector<PublicCondition> m_PublicConditions; //��������     ��ʽ��&&nNeedtoFlush=2,&&nNeedtoStraight>2,||nPair=0  ��һ���߼���Ϊ||��Ĭ��Ϊtrue����ǰ����������ٺͺ�������㣬�൱��ǰ��ļ�������

	//actions
	ActionType m_ActionType;
	double m_dlHeroBetSize = 0;
	RankGroup m_ActionRank; //>=���ȡ�ж�
	RankGroup m_AmbushRank; //>=��������ж���Ϊcall

	double m_dlBluffRatio = 0;	//��û�����г����ƣ��ж�Ϊfoldʱ�����˱���bet��raise
	double m_dlBluffSize = 0;
};

//Ϊ��������ģʽ�������ж���¼
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
	Action AdjustActionByMode(const Action& oldAction, const MultiCondition& multiCondition, const PokerEvaluate& pokerEv); //ʶ��ģʽ����������

public:
	bool Init();
	Action GetHeroAction(const MultiCondition& multiCondition, const PokerEvaluate& pokerEv);
	static std::vector<PublicCondition> str2PublicConditions(const std::string& sSymbol);

};

#endif


