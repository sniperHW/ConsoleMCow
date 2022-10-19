//#include "pch.h"
#include "CGame.h"

using namespace std;

CGame::CGame()
{
	m_dBBSize = 0;
	m_dPot = 100;
	m_round = preflop;
	m_oopx = OOPA;
}

CPlayer* CGame::GetHero()
{
	for (auto p = m_players.begin(); p != m_players.end(); p++) {
		if (p->second.m_blIsHero)
			return &p->second;
	}

	return nullptr;
}

CPlayer* CGame::GetRival()
{
	for (auto p = m_players.begin(); p != m_players.end(); p++) {
		if (!p->second.m_blIsHero)
			return &p->second;
	}

	return nullptr;
}

GameType CGame::GetGameTypeBySymbol(const string& sSymbol)
{
	if (sSymbol == "Max6_NL50_SD150")
		return Max6_NL50_SD150;
	else if (sSymbol == "Max6_NL50_SD100")
		return Max6_NL50_SD100;
	else if (sSymbol == "Max6_NL50_SD75")
		return Max6_NL50_SD75;
	else if (sSymbol == "Max6_NL50_SD50")
		return Max6_NL50_SD50;
	else if (sSymbol == "Max6_NL50_SD30")
		return Max6_NL50_SD30;
	else
		return Max6_NL50_SD100;

}

Position CGame::GetPositionBySymbol(const string& sSymbol)
{
	if (sSymbol == "UTG")
		return UTG;
	else if (sSymbol == "HJ")
		return HJ;
	else if (sSymbol == "CO")
		return CO;
	else if (sSymbol == "BTN")
		return BTN;
	else if (sSymbol == "SB")
		return SB;
	else if (sSymbol == "BB")
		return BB;
	else
		return UTG;
}

Position CGame::GetNextPlayerPosition(const Position curPosition)
{
	if (m_players.size() == 0)
		return nonepos;

	auto p = m_players.find(curPosition);

	if (p == m_players.end())
		return nonepos;
	else if (p == prev(m_players.end()))
		return m_players.begin()->first;
	else
		return next(p)->first;
}

