#pragma once

#ifndef CSOLUTION_H_
#define CSOLUTION_H_

#include "globledef.h"
#include "CGame.h"
#include "CActionLine.h"
#include "CDataFrom.h"
#include "json/json.h"
#include "CStrategy.h"

class CSolution
{
public:
	Action HeroAction(const std::string & sActionLine);
	void FlopReady(const std::string & sActionLine);
	void TurnReady(const std::string & sActionLine);
	void RiverReady(const std::string & sActionLine);


private:
	std::string m_sWholeNodeName; //完整节点名
	std::string m_sCurStreetNodeName; //当前街的节点名称
	std::string m_sPreflopType; //节点类型，用于solver匹配策略树设置
	StackByStrategy m_curStackByStrategy; //当前策略对应的筹码

	CGame m_game;
	Json::Value m_solverResult; //当前街的solver解
	bool m_blIsSolverPresave; //当前街是否使用预存solver解
	CActionLine m_actionLine;
	CDataFrom m_dataFrom;

	Action CalcHeroAction(const CStrategy& strategy);

};

#endif