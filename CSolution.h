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

class CSolution
{
public:
	Action HeroAction(const std::string & sActionLine);
	void FlopReady(const std::string & sActionLine);
	void TurnReady(const std::string & sActionLine);
	void RiverReady(const std::string & sActionLine);

	void InitGame(const std::string & sInitGame);
	void HeroHands(const std::string & sHands); //某些平台初始不能看到hero的手牌，需要单独接口

private:
	CGame m_game;
	Json::Value m_solverResult; //当前街的solver解
	bool m_blIsSolverPresave; //当前街是否使用预存solver解
	CActionLine m_actionLine;

	Action CalcHeroAction(const CStrategy& strategy);

};

#endif