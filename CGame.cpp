//#include "pch.h"
#include "CGame.h"

using namespace std;

CPlayer* CGame::GetHero()
{
	return nullptr;
}

CPlayer* CGame::GetRival()
{
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

