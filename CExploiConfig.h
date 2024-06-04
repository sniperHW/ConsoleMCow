#pragma once
#ifndef CEXPLOI_CONFIG_H_
#define CEXPLOI_CONFIG_H_

#include "globledef.h"

class CExploiConfigItem
{
public:
	std::string m_sPublicCondition;
	std::string m_sActionSquenceCondition;
	std::string m_sPositionCondition;
	std::string m_sGmTypeCondition;

	std::string m_sCommand;
};


class CExploiConfig
{
private:
	std::vector<CExploiConfigItem> m_ExploiItems;

	bool MatchPublicExploi(const PublicStruct& publicStruct, const std::string& sPublicCondition);
	bool matchPublicExploi_OneParam(const int& iInput, const std::string& sCondition);
	bool matchPublicExploi_OneParam(const PokerClass& pokerClass, const std::string& sCondition);

	bool MatchActionLineExploi(const std::string& sActionSquenceRatio, const std::string& sActionSquenceCondition);

	bool MatchPositionExploi(const Position posHeroToMatch, const Position posRivalToMatch, const std::string& sPositionCondition);

	bool MatchStackDepthExploi(const GameType gmTypeToMatch, const std::string& sGmTypeCondition);

public:
	bool Init();
	std::string GetCommand(const std::string& sBoard, const std::string& sActionSquenceRatio, const Position posHero, const Position posRival, const GameType gmType); //返回空代表不匹配

	static bool matchActionExploi(const Action& srcAction, const Action& desAction);
	static bool matchActionSecExploi(const std::string& sToMatch, const std::string& sCondition);
};

#endif

