#pragma once

#ifndef CSTRATEGY_H_
#define CSTRATEGY_H_

#include "globledef.h"
#include "json/json.h"

class CStrategyItem
{
public:
	Action m_action;
	StrategyData m_strategyData;
	EVData m_evData;
};

class CCommForSpecialProcessing
{
public:
	std::string m_sCommand;
	std::vector<std::string> m_sActions1;
	std::vector<std::string> m_sActions2;
	std::vector<std::string> m_range;
	bool m_blRangeExclude;
	std::vector<std::string> m_conditions;
};

class CStrategy
{
public:


	bool Load(GameType gmType, const std::string & sActionSquence, const Stacks& stacks, const OOPX oopx, const SuitReplace & suitReplace, const std::string & sIsoBoard); //从wizard读取
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy, const SuitReplace & suitReplace); //从solver读取

	void AlignmentByBetsize();
	void AlignmentByStackDepth();
	void AlignmentByexploit();

	static int MatchBetRatio(double dActuallyRatio, const std::vector<double>&candidateRatios); 
	static bool parseActionSquence(const std::string& sActionSquence, std::string& sPrefix, Round &round,std::vector<Action>& actions,std::string &actionStr);
	static const Json::Value *geActionNode(const Json::Value *node,const Action& action,const std::vector<Action>& actions,double stack);//const StackByStrategy& stack,bool last);
	static Action getActionByStr(const std::string &str); 
	static double CalcBetRatio(const double dPot, const std::vector<Action>& actions, int iLastIdx, const double dEstack = 0);
	static std::string ConvertOneHandcard(std::string& sCard, const SuitReplace& suitReplace);
private:
	std::vector<std::shared_ptr<CStrategyItem>> m_strategy;
	void ConvertIsomorphism(const SuitReplace & suitReplace); //将m_strategy进行同构转换
	void SpecialProcessing(const std::string& sCommand); //按spcial配置处理m_strategy

	std::vector<CCommForSpecialProcessing> GetCommands(const std::string& sCommands);

	
private:
	void ConvertIsomorphismOneMap(std::unordered_map<std::string, double>* pMap, const SuitReplace& suitReplace);
};

#endif