#pragma once

#ifndef CSOLUTION_H_
#define CSOLUTION_H_

#include "globledef.h"
#include "CGame.h"
#include "CActionLine.h"
#include "json/json.h"
#include "CStrategy.h"
#include <typeinfo>
#include "CPlayer.h"
#include "CRange.h"

class CSolution
{
public:
	Action HeroAction(const std::string & sActionLine);
	bool ChangeRound(const std::string& sActionLine);
	void InitGame(const std::string & sInitGame);
	void HeroHands(const std::string & sHands); //某些平台初始不能看到hero的手牌，需要单独接口

	time_t GetBeginTm();
	static std::string getDataFromString(const StrategyFrom fr); //for test

	CSolution();


private:
	bool m_blNotOffline;	//数据异常作脱线处理
	time_t m_tmBegin;

	CGame m_game;
	StrategyFrom m_strategyFrom; //获取解的模式（放弃先预存再实时模式）

	//Json::Value m_solverResult; //当前街的solver解
	Json::Value m_solverResultLow; //当前街的solver解
	Json::Value m_solverResultUp;

	CActionLine m_actionLine;
	CRange m_range;

	Action CalcHeroAction(const CStrategy& strategy);
	Stacks GetStacks();
	Action ProcessingOffline();

	bool IsMultiPlayers();

	//for current range(exploi)
	RangeData m_heroCurRange;
	std::vector<std::shared_ptr<CStrategyItem>> m_heroLastStrategy;
	void updataHeroCurRange(const std::string& sSelA);

	std::string DumpSelAction(const std::string& sActionLine);
	void DumpActionBeforeChangeRound(const bool blDumpHero = true);
};

#endif