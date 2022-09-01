#pragma once

#ifndef CSTRATEGY_H_
#define CSTRATEGY_H_

#include "globledef.h"
#include "CStrategyItem.h"
#include "./json/include/json.h"

class CStrategy
{
public:
	std::vector<std::shared_ptr<CStrategyItem>> m_strategy;

	bool Load(GameType gmType, const std::string & sNodeName, const SuitReplace & suitReplace, const std::string & sIsoBoard); //从wizard读取
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const SuitReplace & suitReplace); //从solver读取

	void AlignmentByBetsize(float fBase, float fActually);
	void AlignmentByStackDepth(float fBase, float fActually);
	void AlignmentByexploit();

private:
	void ConvertIsomorphism(const SuitReplace & suitReplace); //将m_strategy进行同构转换
	void SpecialProcessing(); //按spcial配置处理m_strategy
	double MatchBetSize(double fActually, const std::set<double>&candidates); //匹配betsize
};

#endif