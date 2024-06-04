#include "CExploiConfig.h"

#include "util.h"
#include <fstream>
#include <iostream>
#include <regex>
#include "CActionLine.h"
#include <algorithm>
#include "CPokerHand.h"

using namespace std;

//sPublicCondition格式：[paramType][m,n)|m ; ... 其中pokerClass表示为(main,sub)只能用|（无区间），blOESDpossible只能0或1（无区间）
//paramType: pokerClass, NeedtoFlush， NeedtoStraight，Pair，MaxRank，HighCardCount，OESDpossible
//[:>=	(:>	]:<=	):<	 无:== ，可以只用一半比如: >0.5 (0.5  ; <=0.5 0.5]
bool CExploiConfig::MatchPublicExploi(const PublicStruct& publicStruct, const string& sPublicCondition)
{
	bool blRet = true;

	regex sep(R"(\s?;\s?)");
	sregex_token_iterator p(sPublicCondition.cbegin(), sPublicCondition.cend(), sep, -1);
	sregex_token_iterator e;
	vector<string> params;
	for (; p != e; ++p) {
		if (!p->str().empty())
			params.push_back(p->str());
	}

	for (auto one_param : params) {
		regex regParamType;
		smatch m;
		regParamType = R"(^\[([^[\]]*)\])";
		string sParamType, sCondition;
		if (regex_search(one_param, m, regParamType)) {
			sParamType = m[1];
			sCondition = m.suffix();
		}

		if (sParamType == "pokerClass")
			blRet = blRet && matchPublicExploi_OneParam(publicStruct.pokerClass, sCondition);
		else if (sParamType == "NeedtoFlush")
			blRet = blRet && matchPublicExploi_OneParam(publicStruct.nNeedtoFlush, sCondition);
		else if (sParamType == "NeedtoStraight")
			blRet = blRet && matchPublicExploi_OneParam(publicStruct.nNeedtoStraight, sCondition);
		else if (sParamType == "Pair")
			blRet = blRet && matchPublicExploi_OneParam(publicStruct.nPair, sCondition);
		else if (sParamType == "MaxRank")
			blRet = blRet && matchPublicExploi_OneParam(publicStruct.nMaxRank, sCondition);
		else if (sParamType == "HighCardCount")
			blRet = blRet && matchPublicExploi_OneParam(publicStruct.nHighCardCount, sCondition);
		else if (sParamType == "OESDpossible")
			blRet = blRet && matchPublicExploi_OneParam((int)publicStruct.blOESDpossible, sCondition);

		if (blRet == false)
			break;
	}

	return blRet;
}


bool CExploiConfig::matchPublicExploi_OneParam(const int& iInput, const string& sCondition)
{
	bool blRet = false;

	//按|分解sCondition
	regex sep(R"(\s?\|\s?)");
	sregex_token_iterator p(sCondition.cbegin(), sCondition.cend(), sep, -1);
	sregex_token_iterator e;
	vector<string> Conditions;
	for (; p != e; ++p) {
		if (!p->str().empty())
			Conditions.push_back(p->str());
	}

	for (auto one_condition : Conditions) {
		//[:>=	(:>	]:<=	):<		无:==
		bool blMatchOne = true;

		regex sepTmp(R"(\s?,\s?)");
		sregex_token_iterator p1(one_condition.cbegin(), one_condition.cend(), sepTmp, -1);
		sregex_token_iterator e1;
		vector<string> conditionAparts;
		for (; p1 != e1; ++p1) {
			if (!p1->str().empty()) {
				conditionAparts.push_back(p1->str());
			}
		}

		//[m,n)
		for (auto one_part : conditionAparts) {
			string sCompare;
			int nValue;
			regex regValue(R"([0-9.]+)");
			regex regCompare(R"([[\]()]+)");
			smatch m;

			if (regex_search(one_part, m, regValue))
				nValue = stoi(m[0]);
			else {
				cout << "error: matchPublicExploi_OneParam no value" << endl;
				break;
			}

			if (regex_search(one_part, m, regCompare))
				sCompare = m[0];

			if (sCompare == "[")
				blMatchOne = blMatchOne && iInput >= nValue;
			else if (sCompare == "(")
				blMatchOne = blMatchOne && iInput > nValue;
			else if (sCompare == "]")
				blMatchOne = blMatchOne && iInput <= nValue;
			else if (sCompare == ")")
				blMatchOne = blMatchOne && iInput < nValue;
			else
				blMatchOne = blMatchOne && iInput == nValue;
		}

		blRet = blRet || blMatchOne;
		if (blRet == true) //|只要有一个满足即为true
			break;
	}

	return blRet;
}

bool CExploiConfig::matchPublicExploi_OneParam(const PokerClass& pokerClass, const std::string& sCondition)
{
	bool blRet = false;

	//按|分解sCondition
	regex sep(R"(\s?\|\s?)");
	sregex_token_iterator p(sCondition.cbegin(), sCondition.cend(), sep, -1);
	sregex_token_iterator e;
	vector<string> Conditions;
	for (; p != e; ++p) {
		if (!p->str().empty())
			Conditions.push_back(p->str());
	}

	for (auto one_condition : Conditions) {
		bool blMatchOne = pokerClass == str2PokerClass(one_condition) ? true : false;
		blRet = blRet || blMatchOne;
		if (blRet == true) //|只要有一个满足即为true
			break;

	}

	return blRet;
}

//sActionSquenceCondition格式：abbrName|abbrName<>ActionStr-ActionStr...<>...
//ActionStr:其中R有几种格式：R	代表匹配所有size; R[m,n]  代表size范围;  R[m  代表size>=;  Rn]代表size<=; flop后第一个用"O"
//目前不支持preflop size
bool CExploiConfig::MatchActionLineExploi(const string& sActionSquenceRatio, const string& sActionSquenceCondition)
{
	//分解到每条街，判断街数是否相同
	regex sep;
	sep = (R"(\s?\<.*\>\s?)");

	vector<string> toMatchRounds;
	vector<string> conditionRounds;

	sregex_token_iterator p(sActionSquenceRatio.cbegin(), sActionSquenceRatio.cend(), sep, -1);
	sregex_token_iterator e;
	for (; p != e; ++p) {
		if (!p->str().empty())
			toMatchRounds.push_back(p->str());
	}
	sregex_token_iterator p1(sActionSquenceCondition.cbegin(), sActionSquenceCondition.cend(), sep, -1);
	sregex_token_iterator e1;
	for (; p1 != e1; ++p1) {
		if (!p1->str().empty())
			conditionRounds.push_back(p1->str());
	}

	if (toMatchRounds.size() != conditionRounds.size() || toMatchRounds.empty())
		return false;

	for (int i = 0; i < conditionRounds.size(); i++) {
		string sCondition_round = conditionRounds[i];
		string sToMatch_round = toMatchRounds[i];

		if (i == 0) { //preflop
			vector<string> sAbbrNames;
			sep = (R"(\s?\|\s?)");
			sregex_token_iterator p2(sCondition_round.cbegin(), sCondition_round.cend(), sep, -1);
			sregex_token_iterator e2;
			for (; p2 != e2; ++p2) {
				if (!p2->str().empty())
					sAbbrNames.push_back(p2->str());
			}

			bool blFlopMatched = false;
			for (auto it : sAbbrNames) {
				if (it == sToMatch_round) {
					blFlopMatched = true;
					break;
				}
			}
			if (!blFlopMatched)
				return false;
		}
		else { // post flop
			//"[hero]_vs[rivals]_srp<6h7h9s>R34.5%-R47.3%-R47.5%";
			//"[hero]_vs[rivals]_srp|testAbbr<>R[30,40]-R[45-R60]";
			vector<string> toMatchSecs;
			vector<string> conditionSecs;
			sep = (R"(\s?-\s?)");

			sregex_token_iterator p3(sCondition_round.cbegin(), sCondition_round.cend(), sep, -1);
			sregex_token_iterator e3;
			for (; p3 != e3; ++p3) {
				if (!p3->str().empty())
					conditionSecs.push_back(p3->str());
			}
			sregex_token_iterator p4(sToMatch_round.cbegin(), sToMatch_round.cend(), sep, -1);
			sregex_token_iterator e4;
			for (; p4 != e4; ++p4) {
				if (!p4->str().empty())
					toMatchSecs.push_back(p4->str());
			}

			if (toMatchSecs.size() != conditionSecs.size())
				return false;

			for (int j = 0; j < toMatchSecs.size(); j++) {
				if (!matchActionSecExploi(toMatchSecs[j], conditionSecs[j]))
					return false;
			}
		}
	}

	return true;
}

bool CExploiConfig::matchActionSecExploi(const string& sToMatch, const string& sCondition)
{
	string sActionType_toMatch, sActionType_condition;
	double dSize_toMatch;
	double dLow_condition = -1;
	double dUp_condition = -1;

	regex regToMatch;
	smatch m;
	regToMatch = R"(([OACRX])([0-9.]*))";
	if (regex_search(sToMatch, m, regToMatch)) {
		sActionType_toMatch = m[1];
		dSize_toMatch = stod(m[2]);
	}
	else {
		cout << "error:matchActionSecExploi sToMatch format error" << endl;
		return false;
	}

	regToMatch = R"([OACRX])";
	if (regex_search(sCondition, m, regToMatch)) {
		sActionType_condition = m[0];
	}
	else {
		cout << "error:matchActionSecExploi sCondition format error" << endl;
		return false;
	}

	regToMatch = R"(\[([0-9.]*))";
	if (regex_search(sCondition, m, regToMatch))
		dLow_condition = stod(m[1]);

	regToMatch = R"(([0-9.]*)\])";
	if (regex_search(sCondition, m, regToMatch))
		dUp_condition = stod(m[1]);

	if (sActionType_toMatch != sActionType_condition)
		return false;
	else if (sActionType_toMatch == sActionType_condition && sActionType_toMatch != "R")
		return true;
	else {
		if (dLow_condition == -1 && dUp_condition == -1)
			return true;
		if (dLow_condition != -1 && dSize_toMatch < dLow_condition)
			return false;
		if (dUp_condition != -1 && dSize_toMatch > dUp_condition)
			return false;
	}

	return true;
}

//sPositionCondition格式：[hero]BTN|CO,[rival]UTG|HJ   其中位置为空代表全匹配
//ToMatch为positionByPresent
//preflop多人时不能指定rival
bool CExploiConfig::MatchPositionExploi(const Position posHeroToMatch, const Position posRivalToMatch, const std::string& sPositionCondition)
{
	vector<Position> heroPositionConditions, rivalPositionConditions;

	regex sep;
	sep = (R"(\s?,\s?)");

	vector<string> conditionStrs;
	sregex_token_iterator p(sPositionCondition.cbegin(), sPositionCondition.cend(), sep, -1);
	sregex_token_iterator e;
	for (; p != e; ++p) {
		if (!p->str().empty())
			conditionStrs.push_back(p->str());
	}

	for (auto conditionStr : conditionStrs) {
		//[hero]BTN|CO
		regex reg;
		smatch m;
		reg = R"(\[(.*)\](.*))";
		string sPlayer, sPositions;
		if (regex_search(conditionStr, m, reg)) {
			sPlayer = m[1];
			sPositions = m[2];
		}

		vector<Position>* pPlayerPositionConditions = nullptr;
		if (sPlayer == "hero")
			pPlayerPositionConditions = &heroPositionConditions;
		else if (sPlayer == "rival")
			pPlayerPositionConditions = &rivalPositionConditions;
		else
			cout << "error:MatchPositionExploi player error" << endl;

		if (pPlayerPositionConditions != nullptr) {
			regex sepPosition(R"(\s?\|\s?)");
			sregex_token_iterator p1(sPositions.cbegin(), sPositions.cend(), sepPosition, -1);
			sregex_token_iterator e1;
			for (; p1 != e1; ++p1) {
				if (!p1->str().empty())
					pPlayerPositionConditions->push_back(str2Position(p1->str()));
			}
		}
	}

	bool blHeroMatched = false;
	bool blRivalMatched = false;
	for (auto positionCondition : heroPositionConditions) {
		if (positionCondition == posHeroToMatch) {
			blHeroMatched = true;
			break;
		}
	}
	for (auto positionCondition : rivalPositionConditions) {
		if (positionCondition == posRivalToMatch) {
			blRivalMatched = true;
			break;
		}
	}
	if (heroPositionConditions.empty())
		blHeroMatched = true;
	if (rivalPositionConditions.empty())
		blRivalMatched = true;

	return blHeroMatched && blRivalMatched;
}

//sGmTypeCondition格式：Max6_NL50_SD150|Max6_NL50_SD100  空为全匹配
bool CExploiConfig::MatchStackDepthExploi(const GameType gmTypeToMatch, const std::string& sGmTypeCondition)
{
	vector<GameType> gmTypeConditions;
	regex sep(R"(\s?\|\s?)");
	sregex_token_iterator p(sGmTypeCondition.cbegin(), sGmTypeCondition.cend(), sep, -1);
	sregex_token_iterator e;
	for (; p != e; ++p) {
		if (!p->str().empty())
			gmTypeConditions.push_back(str2gmType(p->str()));
	}

	if (gmTypeConditions.empty())
		return true;

	for (auto gmTypeCondition : gmTypeConditions) {
		if (gmTypeToMatch == gmTypeCondition)
			return true;
	}

	return false;
}

bool CExploiConfig::matchActionExploi(const Action& srcAction, const Action& desAction)
{
	if (srcAction.actionType == desAction.actionType) {
		if (srcAction.actionType == check || srcAction.actionType == call || srcAction.actionType == allin || srcAction.actionType == fold)
			return true;
		else { //raise

			if (fabs(srcAction.fBetSizeByPot - desAction.fBetSizeByPot) <= EXPLOI_MATCH_DIS)
				return true;
			else
				return false;
		}
	}
	else
		return false;
}

//返回空代表不匹配, 匹配第一个满足的
string CExploiConfig::GetCommand(const string& sBoard, const string& sActionSquenceRatio, const Position posHero, const Position posRival, const GameType gmType)
{
	MyCards publics(CActionLine::ToMyCards(sBoard));
	CPokerHand pokerHand;
	PublicStruct publicStruct = pokerHand.getPublicStruct(publics);

	for (auto item : m_ExploiItems) {
		if (!MatchPublicExploi(publicStruct, item.m_sPublicCondition))
			break;
		if (!MatchActionLineExploi(sActionSquenceRatio, item.m_sActionSquenceCondition))
			break;
		if (!MatchPositionExploi(posHero, posRival, item.m_sPositionCondition))
			break;
		if(!MatchStackDepthExploi(gmType, item.m_sGmTypeCondition))
			break;

		return item.m_sCommand;
	}

	return "";
}

//格式：sPublicCondition ;; sActionSquenceCondition ;; sPositionCondition ;; sGmTypeCondition ;; sCommand
bool CExploiConfig::Init()
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\";
	regex reg_blank(R"(^\s*$)");

	string sLine, sFilePath;

	sFilePath = sConfigFolder + "ExploiConfig.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;


	while (getline(fin, sLine)) {
		if (regex_match(sLine, reg_blank))
			continue;

		regex sep(R"(\s?\;;\s?)");
		sregex_token_iterator p(sLine.cbegin(), sLine.cend(), sep, -1);
		sregex_token_iterator e;
		vector<string> params;
		for (; p != e; ++p) {
			if (!p->str().empty())
				params.push_back(p->str());
		}

		if (params.size() != 5) {
			cout << "error: Exploi format not correct" << endl;
			return false;
		}

		CExploiConfigItem exploiConfigItem;
		exploiConfigItem.m_sPublicCondition = params[0];
		exploiConfigItem.m_sActionSquenceCondition = params[1];
		exploiConfigItem.m_sPositionCondition = params[2];
		exploiConfigItem.m_sGmTypeCondition = params[3];
		exploiConfigItem.m_sCommand = params[4];

		m_ExploiItems.push_back(exploiConfigItem);
	}
	fin.close();

	return true;
}





