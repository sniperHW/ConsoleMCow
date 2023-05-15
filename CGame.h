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

	std::pair<GameType, GameType> m_gmTypeBound; 	//只有一个有效则second为GAMETYPE_none
	double m_dSegmentRatio; //离gmTypeLow的占比

	double m_dPot; //按多少个大盲计
	Round m_round;
	CBoard m_board;
	OOPX m_oopx;

	std::map<Position, CPlayer> m_players;

	CPlayer* GetHero();
	CPlayer* GetRival(); //只对双人对抗时

	static GameType GetGameTypeBySymbol(const std::string& sSymbol);
	static Position GetPositionBySymbol(const std::string& sSymbol);

	Position GetNextPlayerPosition(const Position curPosition);

	Multi_Position GetHeroMultiPosition();

	CGame();
};

#endif 