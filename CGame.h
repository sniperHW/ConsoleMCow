#pragma once
#ifndef CGAME_H_
#define CGAME_H_

#include "globledef.h"
#include "CPlayer.h"
#include "CBoard.h"
 

class CGame
{
public:
	std::string m_sID;
	GameType m_gmType;
	double m_dBBSize;

	double m_dPot; //按多少个大盲计
	Round m_round;
	CBoard m_board;

	std::map<Position, CPlayer> m_players;

	CPlayer* GetHero();
	CPlayer* GetRival(); //只对双人对抗时

	static GameType GetGameTypeBySymbol(const std::string& sSymbol);
	static Position GetPositionBySymbol(const std::string& sSymbol);
};

#endif 