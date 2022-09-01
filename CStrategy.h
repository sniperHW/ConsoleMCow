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

	bool Load(GameType gmType, const std::string & sNodeName, const SuitReplace & suitReplace, const std::string & sIsoBoard); //��wizard��ȡ
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const SuitReplace & suitReplace); //��solver��ȡ

	void AlignmentByBetsize(float fBase, float fActually);
	void AlignmentByStackDepth(float fBase, float fActually);
	void AlignmentByexploit();

private:
	void ConvertIsomorphism(const SuitReplace & suitReplace); //��m_strategy����ͬ��ת��
	void SpecialProcessing(); //��spcial���ô���m_strategy
	double MatchBetSize(double fActually, const std::set<double>&candidates); //ƥ��betsize
};

#endif