#pragma once

#ifndef CSTRATEGY_H_
#define CSTRATEGY_H_

#include "globledef.h"
#include "CStrategyItem.h"
#include "json/json.h"

class CStrategy
{
public:
	std::vector<std::shared_ptr<CStrategyItem>> m_strategy;

	bool Load(GameType gmType, const std::string & sActionSquence, const StackByStrategy& stack, const SuitReplace & suitReplace, const std::string & sIsoBoard); //从wizard读取
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const StackByStrategy& stack, const SuitReplace & suitReplace); //从solver读取

	void AlignmentByBetsize(float fBase, float fActually);
	void AlignmentByStackDepth(float fBase, float fActually);
	void AlignmentByexploit();

private:
	void load(const Json::Value& node,std::vector<Action> &actions,int pos);

	void ConvertIsomorphism(const SuitReplace & suitReplace); //将m_strategy进行同构转换
	void SpecialProcessing(); //按spcial配置处理m_strategy

	
	int MatchBetSize(double dActuallySize, const std::vector<double>& candidateSizes, GameType gmType, const StackByStrategy& stack);	//按实际下注bb数，匹配子节下注空间，用于sover解计算，参数都为实际size，返回为匹配的siz
	int MatchBetRatio(double dActuallyRatio, const std::vector<double>& candidateRatios, GameType gmType, const StackByStrategy& stack);	//按实际下注比例，匹配子节下注空间，用于wizard解计算，需要先将size转为比例，候选在策略树设置中，返回为匹配的比例
};

#endif