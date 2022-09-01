#pragma once
#ifndef CGAME_H_
#define CGAME_H_

#include "globledef.h"
#include <list>
#include "CPlayer.h"
#include "CBoard.h"

class CGame
{
public:
	int m_gmID;
	GameType m_gmType;
	float m_sBBSize;

	float m_fPot; //按多少个大盲计
	Round m_round;
	CBoard m_board;
	std::list<CPlayer> m_players;
	CPlayer* m_pHero;

	CGame(int gmID=0, GameType gmType= Max6_NL50_SD100, float sBBSize=0);
};

#endif 