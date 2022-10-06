#pragma once
#ifndef CACTIONLINE_H_
#define CACTIONLINE_H_

#include "globledef.h"
#include "CGame.h"

class CActionLine
{
public:
	std::string m_sActionSquence; 
	std::string m_sAbbrName; 
	std::string m_sNodeName; //只在turn换街时有效

	bool Parse(const std::string& sActionLine, CGame & game); //offline或多人返回false,更新game相关，取得NodeName, 记录raisesize，记录preflop类型，sActionLine为增量
	static std::string GetNodeNameWithoutBoard(const std::string& sNodeName);
	static std::string GetNodeName(const GameType gmType, const Stacks& stacks, const OOPX oopx,const std::vector<Action>& actions, const std::string& sPrefix);
	static void EraseBlank(std::string& sSource);

//for test
	std::string GetPreflopRowActionSquence();

private:
	std::string m_sPreflopRowActionSquence;
	std::vector<Position> m_limps; //limp替换模式标志：Limps.size()>0

	std::pair<Position, Action> ToPosActionPair(const std::string& sSymbol);
	std::string ToActionSymbol(const Action& a, bool blRemainSize=true);

	GameType GetCurGameType(const CGame& game);
	int MatchStackDepth(double dEStack);
	GameType GetGameTypeByStackDepth(int nStackDepth);

	std::string GetOOPXSymbol(const Position hero, const Position rival); //对hero
	OOPX GetOOPX(const CGame& game);	//对游戏
};

#endif
