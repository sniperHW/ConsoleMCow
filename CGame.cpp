//#include "pch.h"
#include "CGame.h"
#include<algorithm>

using namespace std;

CGame::CGame()
{
	m_dBBSize = 0;
	m_dPot = 100;
	m_round = preflop;
	m_oopx = OOPA;

	m_gmTypeBound.first = GAMETYPE_none;
	m_gmTypeBound.second = GAMETYPE_none;
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
	else if (sSymbol == "Max6_NL50_SD20")
		return Max6_NL50_SD20;
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

//多人时hero位置（只分前中后）
Multi_Position CGame::GetHeroMultiPosition()
{
	map<Position, int>SquenceByPosition{ {SB,1},{BB,2},{UTG,3},{HJ,4},{CO,5},{BTN,6} };
	vector<int> tmp;
	int nHeroIdx = 0;
	for (auto player : m_players) {
		if (player.second.m_lastAction.actionType != fold) {
			if (player.second.m_blIsHero)
				nHeroIdx = SquenceByPosition[player.first];
			else
				tmp.push_back(SquenceByPosition[player.first]);
		}
	}

	if (tmp.empty())
		return multi_back;

	if (nHeroIdx > *max_element(tmp.begin(), tmp.end()))
		return multi_back;
	else if (nHeroIdx < *min_element(tmp.begin(), tmp.end()))
		return multi_front;
	else
		return multi_between;

}

