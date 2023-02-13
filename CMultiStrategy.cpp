#include "CMultiStrategy.h"
#include <vector>
#include <regex>
#include <fstream>
#include "CActionLine.h"

using namespace std;

//返回的动作类型：raise，call，none,如果是C则要判断是call还是check
Action CMultiStrategy::GetHeroAction(const MultiCondition& multiCondition, const PokerEvaluate& pokerEv)
{
	Action retAction;
	retAction.actionType = none;

	RankGroup rankGroup = pokerEv.rankGroup;

	vector<Action> possibleActions;

	for (auto item : m_MultiStrategyConfigItems) {
		if (multiCondition.Round != item.m_Round)
			continue;
		if(!compareBySymbol(multiCondition.nPlayerCount, item.m_PlayerCountComparer, item.m_nPlayerCount))
			continue;
		if (!compareBySymbol(multiCondition.dlBetSize, item.m_BetSizeComparer, item.m_dlBetSize))
			continue;
		if (!compareBySymbol(multiCondition.dlSpr, item.m_SprComparer, item.m_dlSpr))
			continue;
		if(multiCondition.Position != item.m_Position && item.m_Position != multi_none)
			continue;
		if(multiCondition.HeroActive != item.m_HeroActive && item.m_HeroActive != hero_none)
			continue;

		regex reg;
		reg = item.m_sActionLineCondition;
		if (!regex_match(multiCondition.sActionLine, reg))
			continue;

		if (!(rankGroup >= item.m_ActionRank)) { //牌型不符合大小要求
			if (item.m_dlBluffRatio > 0) {
				double dlRand = (double)(rand() % (100 + 1)) / (double)100;
				if (dlRand < item.m_dlBluffRatio) { //中bluff的概率
					Action actionTmp;
					actionTmp.actionType = raise;
					actionTmp.fBetSizeByPot = (float)item.m_dlBluffSize;
					possibleActions.push_back(actionTmp);
				}
			}
		}
		else { //牌型符合大小要求
			Action actionTmp;
			actionTmp.actionType = item.m_ActionType;
			actionTmp.fBetSizeByPot = (float)item.m_dlHeroBetSize;
			if (rankGroup >= item.m_AmbushRank)
				actionTmp.actionType = call;
			possibleActions.push_back(actionTmp);
		}
	}

	for (auto one_action : possibleActions) {
		retAction = one_action;
		if (one_action.actionType == raise)
			break;
	}

	return retAction;
}

//flop;	.*-X;	<=3;<=0.75;	;front	;initiative;	raise;	0.5;	ev_nuts,evsub_large;	ev_nuts;	0;
//round;行动线匹配条件;人数;size;spr;hero相对位置;hero主动/被动;动作;size;牌型;埋伏牌型;bluff比例
bool CMultiStrategy::Init()
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" ;
	string sFilePath = sConfigFolder + "MultiStrategyConfig.txt";

	string sLine;
	regex sep(R"(\s*;\s*)");
	regex reg_blank(R"(^\s*$)");
	vector<string> vTmp;
	smatch m;

	regex sepCompareCombol(R"([<=>]+)");
	regex sepDigit(R"([0-9.]+)");

	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	while(getline(fin, sLine)) {
		if (regex_match(sLine, reg_blank))
			continue;
		sregex_token_iterator p(sLine.cbegin(), sLine.cend(), sep, -1);
		sregex_token_iterator e;
		vTmp.clear();
		for (; p != e; ++p) {
			string s = p->str();
			CActionLine::EraseBlank(s);
			vTmp.push_back(s);
		}

		CMultiStrategyConfigItem configItem;
		configItem.m_Round = str2round(vTmp[0]);
		configItem.m_sActionLineCondition = vTmp[1];

		if (regex_search(vTmp[2], m, sepCompareCombol))
			configItem.m_PlayerCountComparer = str2compareSymbol(m[0]);
		if (regex_search(vTmp[2], m, sepDigit))
			configItem.m_nPlayerCount = stoi(m[0]);

		if (regex_search(vTmp[3], m, sepCompareCombol))
			configItem.m_BetSizeComparer = str2compareSymbol(m[0]);
		if (regex_search(vTmp[3], m, sepDigit))
			configItem.m_dlBetSize = stod(m[0]);

		if (regex_search(vTmp[4], m, sepCompareCombol))
			configItem.m_SprComparer = str2compareSymbol(m[0]);
		if (regex_search(vTmp[4], m, sepDigit))
			configItem.m_dlSpr = stod(m[0]);

		configItem.m_Position = str2multiPosition(vTmp[5]);
		configItem.m_HeroActive = str2heroActive(vTmp[6]);

		configItem.m_ActionType = str2ActionType(vTmp[7]);
		configItem.m_dlHeroBetSize = stod(vTmp[8]);

		configItem.m_ActionRank = str2rankGroup(vTmp[9]);
		configItem.m_AmbushRank = str2rankGroup(vTmp[10]);

		configItem.m_dlBluffRatio = stod(vTmp[11]);
		configItem.m_dlBluffSize = stod(vTmp[12]);

	}

	return true;
}


