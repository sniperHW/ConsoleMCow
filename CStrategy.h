#pragma once

#ifndef CSTRATEGY_H_
#define CSTRATEGY_H_

#include "globledef.h"
#include "json/json.h"
#include "CGame.h"

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


struct Condition {
	std::string sObj; //目前只有EV,""代表无条件
	bool	blLess;
	bool	blEqueal;
	double	dlValue;
};

class CStrategy
{
public:

	bool Load(GameType gmType, const std::string& sActionSquence); //从自定义策略文件读取
	bool Load(GameType gmType, const std::string & sActionSquence, const Stacks& stacks, const OOPX oopx, const SuitReplace & suitReplace, const std::string & sIsoBoard); //从wizard读取
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy, const SuitReplace & suitReplace); //从solver读取

	void AlignmentByBetsize();
	void AlignmentByStackDepth();
	void AlignmentByexploit();

	static int MatchBetRatio(double dActuallyRatio, const std::vector<double>&candidateRatios); 
	static bool parseActionSquence(const std::string& sActionSquence, std::string& sPrefix, Round &round,std::vector<Action>& actions,std::string &actionStr);
	static const Json::Value *geActionNode(const Json::Value *node,const Action& action,const std::vector<Action>& actions, std::vector<Action>& actionsByStrategy, const Stacks& stacks, const Stacks& stacksByStrategy);//double stack);//const StackByStrategy& stack,bool last);
	static Action getActionByStr(const std::string &str); 
	static double CalcBetRatio(const double dPot, const std::vector<Action>& actions, int iLastIdx, const double dEstack = 0); //只有allin才填写dEstack

	//多人对抗函数
	//static std::vector<std::pair<Position, Action>> parseMultiActionSquence(const std::string& sOriActionInfo, std::vector<std::pair<Position, Action>> posActions);
	static std::vector<std::pair<Position, Action>> parseMultiActionSquence(const std::string& sOriActionInfo);
	static double CalcMultiBetRatio(const double dPot, const std::vector<std::pair<Position, Action>>& posActions);

	void LoadMacro(std::string path);

	static std::string ConvertOneHandcard(std::string& sCard, const SuitReplace& suitReplace);

	void DumpStrategy(const std::string& sComment,  const std::vector<std::shared_ptr<CStrategyItem>>* pStrategy = NULL);
	bool ReadStrategy(const std::string& sPath,  std::vector<std::shared_ptr<CStrategyItem>>& strategy);

//for test
void SpecialProcessing(const GameType gmType, const std::string& sCommand); //按spcial配置处理m_strategy
std::vector<std::shared_ptr<CStrategyItem>> m_strategy;
	void Assign(const std::string &action,const std::unordered_map<std::string, bool> &rangeMap);//std::vector<std::string> &range);
	void Discard(const std::string &action,const std::unordered_map<std::string, bool> &rangeMap, const std::vector<Condition>& conditions);
	void Replace(const std::string &action1,const std::string &action2,const std::unordered_map<std::string, bool> &rangeMap, const std::vector<Condition>& conditions);

private:

	void ConvertIsomorphism(const SuitReplace & suitReplace); //将m_strategy进行同构转换
	
	

	
	std::vector<CCommForSpecialProcessing> GetCommands(const std::string& sCommands);
	void DiscardOne(std::shared_ptr<CStrategyItem>& strategyItem, std::shared_ptr<CStrategyItem>& foldItem, const std::unordered_map<std::string, bool>& rangeMap, const Condition& condition);
	void ReplaceOne(std::shared_ptr<CStrategyItem>& srcStrategy, std::shared_ptr<CStrategyItem>& desStrategy, std::shared_ptr<CStrategyItem>& allinStrategy, const std::unordered_map<std::string, bool>& rangeMap, const Condition& condition);



	
private:
	void ConvertIsomorphismOneMap(std::unordered_map<std::string, double>* pMap, const SuitReplace& suitReplace);
};

#endif