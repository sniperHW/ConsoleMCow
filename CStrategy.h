#pragma once

#ifndef CSTRATEGY_H_
#define CSTRATEGY_H_

#include "globledef.h"
#include "CStrategyItem.h"
#include "json/json.h"

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

struct condition {
	double     m_ev;
	bool       m_less;//if true <,else >	
};

class CStrategy
{
public:
	std::vector<std::shared_ptr<CStrategyItem>> m_strategy;
	std::unordered_map<std::string,std::string> m_macro;

	bool Load(GameType gmType, const std::string & sActionSquence, const Stacks& stacks, const SuitReplace & suitReplace, const std::string & sIsoBoard); //从wizard读取
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy, const SuitReplace & suitReplace); //从solver读取

	void AlignmentByBetsize(float fBase, float fActually);
	void AlignmentByStackDepth(float fBase, float fActually);
	void AlignmentByexploit();


//	static int MatchBetSize(double dActuallySize, const std::vector<double>& candidateSizes, const double dEStatck);	//按实际下注bb数，返回最后一个代表allin
	static int MatchBetRatio(double dActuallyRatio, const std::vector<double>&candidateRatios); 
	static std::string getNodeName(const GameType gmType, const Stacks& stacks, const std::vector<Action>& actions, const std::string& sPrefix);
	static bool parseActionSquence(const std::string& sActionSquence, std::string& sPrefix, Round &round,std::vector<Action>& actions,std::string &actionStr);
	static const Json::Value *geActionNode(const Json::Value *node,const Action& action,const std::vector<Action>& actions,double stack);//const StackByStrategy& stack,bool last);
	static Action getActionByStr(const std::string &str); 
	static double CalcBetRatio(const double dPot, const std::vector<Action>& actions, int iLastIdx, const double dEstack = 0);

	void LoadMacro(std::string path);
	void DoMacro(std::string macro);

private:
	
	void ConvertIsomorphism(const SuitReplace & suitReplace); //将m_strategy进行同构转换
	
	
	void Assign(const std::string &action,const std::unordered_map<std::string, bool> &rangeMap);//std::vector<std::string> &range);
	void Discard(const std::string &action,const std::unordered_map<std::string, bool> &rangeMap,condition *cond);
	void Replace(const std::string &action1,const std::string &action2,const std::unordered_map<std::string, bool> &rangeMap,condition *cond);
	void SpecialProcessing(const std::string& sCommand); //按spcial配置处理m_strategy



	std::vector<CCommForSpecialProcessing> GetCommands(const std::string& sCommands);

	

};

#endif