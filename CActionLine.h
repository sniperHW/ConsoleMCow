#pragma once
#ifndef CACTIONLINE_H_
#define CACTIONLINE_H_

#include "globledef.h"
#include "CGame.h"

class CActionLine
{
public:
	std::string m_sActionSquence; //preflop_nodeName + board + 动作序列（R后为size）,记忆之前的动作
	std::string m_sAbbrName; //抽象了preflop的position和board
	std::string m_sNodeName; //将m_sActionSquence的动作序列转为标准下注空间的比例(R后为比例，无预存解设置的保持原比例)，只在turn换街时有效

	MultiCondition m_multiCondition; //多人条件
	RankGroup m_rankGroup; //牌型评价
	std::string m_sOriActionInfo; //用于多人，记录原始的动作信息，格式: <>[position]actionsize,...<>...
	PokerEvaluate m_pokerEv; //牌型评价

	bool Parse(const std::string& sActionLine, CGame & game); //offline或多人返回false,更新game相关，取得NodeName, 记录raisesize，记录preflop类型，sActionLine为增量
	static std::string GetNodeNameWithoutBoard(const std::string& sNodeName);
	static std::string GetNodeName(const GameType gmType, const Stacks& stacks, const OOPX oopx,const std::vector<Action>& actions, const std::string& sPrefix);
	static void EraseBlank(std::string& sSource);

	static std::string getOOPXString(const OOPX oopx);
	static std::string getRoundString(const Round round);
	static std::string getRelativePositionString(const RelativePosition rp);
	static std::pair<Position, Action> ToPosActionPair(const std::string& sSymbol);
	static std::string ToActionSymbol(const Action& a, bool blRemainSize = true);
	static Round getNextRound(const Round round);

private:
	std::string m_sPreflopRowActionSquence;
	Position m_firstLimperPositionByPresent; //由于limp的玩家只在第一圈计算，所以要记忆到成员变量
	std::vector<Position> m_limps; //limp替换模式标志：Limps.size()>0
	bool m_blIsMultiPlayer = false; //是否flop后为多人

	GameType GetCurGameType(const CGame& game);
	int MatchStackDepth(double dEStack);
	GameType GetGameTypeByStackDepth(int nStackDepth);

	std::string GetOOPXSymbol(const Position hero, const Position rival); //对hero
	OOPX GetOOPX(const CGame& game);	//对游戏
	
	std::string GetBetCountStr(); //返回m_sOriActionInfo nB格式
	MyCards ToMyCards(const std::string sSymbol); //将字符串格式转为MyCards

};

#endif
