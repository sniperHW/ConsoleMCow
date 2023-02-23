#include "CMultiStrategy.h"
#include <vector>
#include <regex>
#include <fstream>
#include "CActionLine.h"
#include "CStrategy.h"


using namespace std;

//���صĶ������ͣ�raise��call��check��raise��size�ͱ���ͬʱ���أ���ƽ̨��raise�����Ƿ������ȷ�������ȷ������Ĭ�ϰ�ť�򰴱��������򰴾���size��(�ͻ��˿�����Ҫ��allinתΪcall allin)
Action CMultiStrategy::GetHeroAction(const MultiCondition& multiCondition, const PokerEvaluate& pokerEv)
{
	Action retAction;
	retAction.actionType = fold; retAction.fBetSize = 0; retAction.fBetSizeByCash = 0; retAction.fBetSizeByPot = 0;
	
	RankGroup rankGroup = pokerEv.rankGroup;
	RankGroup rgMin; //F+
	rgMin.nMainGroup = ev_small;
	rgMin.nMainGroup = evsub_small;
	vector<Action> possibleActions;
	bool blBluff = false;

	for (auto item : m_MultiStrategyConfigItems) {

		if (item.m_sActionLineCondition == ".*<>3B<>B")
			cout << "test";


		if (multiCondition.Round != item.m_Round)
			continue;
		if(!compareBySymbol(multiCondition.nPlayerCount, item.m_PlayerCountComparer, item.m_nPlayerCount))
			continue;
		if (!compareBySymbol(multiCondition.dlBetSize, item.m_BetSizeComparer, item.m_dlBetSize))
			continue;
		if (!compareBySymbol(multiCondition.dlSpr, item.m_SprComparer, item.m_dlSpr))
			continue;
		if(multiCondition.MultiPosition != item.m_Position && item.m_Position != multi_none)
			continue;
		if(multiCondition.HeroActive != item.m_HeroActive && item.m_HeroActive != hero_none)
			continue;
		if (!ComparePublicCondition(pokerEv.publicStruct, item.m_PublicConditions))
			continue;

		regex reg;
		reg = item.m_sActionLineCondition;
		if (!regex_match(multiCondition.sActionLine, reg))
			continue;

		if (!(rankGroup >= item.m_ActionRank)) { //���Ͳ����ϴ�СҪ���ж��Ƿ����bluff(ֻ��river)
			if (item.m_dlBluffRatio > 0) {
				double dlRand = (double)(rand() % (100 + 1)) / (double)100;
				if (dlRand < item.m_dlBluffRatio) { //��bluff�ĸ���
					Action actionTmp;
					actionTmp.actionType = raise;
					actionTmp.fBetSizeByPot = (float)item.m_dlBluffSize;
					possibleActions.push_back(actionTmp);
					blBluff = true;
				}
			}
		}
		else { //���ͷ��ϴ�СҪ���ж��Ƿ�>=�������
			Action actionTmp;
			actionTmp.actionType = item.m_ActionType;
			actionTmp.fBetSizeByPot = (float)item.m_dlHeroBetSize;
			if (rankGroup >= item.m_AmbushRank && item.m_AmbushRank.nMainGroup != -1)
				actionTmp.actionType = call;
			possibleActions.push_back(actionTmp);
		}
	}

	//��������ģ��������������жԸ���������ø����size��
	for (auto one_action : possibleActions) {
		if (one_action > retAction)
			retAction = one_action;
	}

	if (retAction.actionType == raise) {
		//��raise�ı����������raise��size
		vector <pair < Position, Action>> posActions = CStrategy::parseMultiActionSquence(multiCondition.sOriActionInfo);
		retAction.fBetSize = (float)CStrategy::CalcBetSize(multiCondition.dPot, posActions, multiCondition.heroPosition, retAction.fBetSizeByPot);
		double dHeroRamainStack = GetRemainStack(multiCondition.dHeroEStack, posActions, multiCondition.heroPosition);
		if ((float)dHeroRamainStack <= retAction.fBetSize) { //����עsize>=ʣ�����ʱ��תΪallin
			if (!blBluff) {
				retAction.actionType = allin;
				retAction.fBetSize = (float)dHeroRamainStack; //Ӧ�ò���Ҫʹ��

				//���oriActionLine�������������allin����allinתΪcall���ͻ���ʵ�֣�
			}
			else //���ʣ����벻��bluff��size�����bluff
				retAction.actionType = fold;
		}

		//�����Ҫraise allin threshold ����������㣬Ŀǰ����Ҫ
	}

	//�����fold,��X��Oʱ��תΪcheck
	if (retAction.actionType == fold) {
		string sLastActionLine;
		auto p = multiCondition.sActionLine.rfind('>');
		if (p != string::npos) {
			sLastActionLine = multiCondition.sActionLine.substr(p + 1, multiCondition.sActionLine.size());
			if (sLastActionLine == "O" || sLastActionLine == "X")
				retAction.actionType = check;
		}

		//����call allin��ֵʱ
		if (retAction.actionType != check && multiCondition.dlSprCurrent < ALLIN_THRESHOLD_CALL) {
			if(rankGroup >= rgMin)
				retAction.actionType = call;
		}
	}

	//���������ʷ��¼��ʶ��ģʽ�����޸Ĳ��ԣ���ʱ��Ӳ����ʵ,��ģʽʶ��
	retAction = AdjustActionByMode(retAction, multiCondition,pokerEv);

	CActionHistoryItem actionHistoryItem;
	actionHistoryItem.m_multiCondition = multiCondition;
	actionHistoryItem.m_pokerEv = pokerEv;
	actionHistoryItem.m_action = retAction;
	m_ActionHistorys.push_back(actionHistoryItem);

	return retAction;
}

//flop;	.*-X;	<=3;<=0.75;	;front	;initiative;	raise;	0.5;	ev_nuts,evsub_large;	ev_nuts;	0;
//round;�ж���ƥ������;����;size;spr;hero���λ��;hero����/����;����;size;����;�������;bluff����
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

		configItem.m_PublicConditions = str2PublicConditions(vTmp[7]);

		configItem.m_ActionType = str2ActionType(vTmp[8]);
		if (regex_search(vTmp[9], m, sepDigit))
			configItem.m_dlHeroBetSize = stod(m[0]);

		configItem.m_ActionRank = str2rankGroup(vTmp[10]);
		configItem.m_AmbushRank = str2rankGroup(vTmp[11]);

		if (regex_search(vTmp[12], m, sepDigit))
			configItem.m_dlBluffRatio = stod(m[0]);
		if (regex_search(vTmp[13], m, sepDigit))
			configItem.m_dlBluffSize = stod(m[0]);

		m_MultiStrategyConfigItems.push_back(configItem);

	}

	return true;
}

double CMultiStrategy::GetRemainStack(double dEstack, const std::vector<std::pair<Position, Action>>& posActions, const Position heroPosition)
{
	double dRecentRSize = 0;
	double dHeroRemain = dEstack;
	for (int i = 0; i < posActions.size(); i++) {
		if (posActions[i].second.actionType == raise || posActions[i].second.actionType == allin) {
			dRecentRSize = posActions[i].second.fBetSize;
			if (posActions[i].first == heroPosition)
				dHeroRemain -= posActions[i].second.fBetSize;
		}
		else if (posActions[i].second.actionType == call) {
			if (posActions[i].first == heroPosition)
				dHeroRemain -= dRecentRSize;
		}
	}
	
	return dHeroRemain;
}

bool CMultiStrategy::ComparePublicCondition(const PublicStruct& publicStruct, vector<PublicCondition>& PublicConditions)
{
	if (PublicConditions.empty())
		return true;

	bool ret = true;
	for (auto config : PublicConditions) {
		bool blItem = false;
		if (config.pcToCheck == pc_flush) {
			if (compareBySymbol(publicStruct.nNeedtoFlush, config.rankComparer, config.nCount))
				blItem = true;
		}
		else if (config.pcToCheck == pc_straight) {
			if (compareBySymbol(publicStruct.nNeedtoStraight, config.rankComparer, config.nCount))
				blItem = true;
		}
		else if (config.pcToCheck == pc_pair) {
			if (compareBySymbol(publicStruct.nPair, config.rankComparer, config.nCount))
				blItem = true;
		}
		else if (config.pcToCheck == pc_high) {
			if (compareBySymbol(publicStruct.nMaxRank, config.rankComparer, config.nCount))
				blItem = true;
		}

		if (config.lcComparer == lc_and)
			ret = ret && blItem;
		else if(config.lcComparer == lc_or)
			ret = ret || blItem;
	}

	return ret;
}

vector<PublicCondition> CMultiStrategy::str2PublicConditions(const string& sSymbol)
{
	vector<PublicCondition> ret;

	regex sep(R"(\s*,\s*)");
	vector<string> vTmp;

	sregex_token_iterator p(sSymbol.cbegin(), sSymbol.cend(), sep, -1);
	sregex_token_iterator e;
	vTmp.clear();
	for (; p != e; ++p) {
		string s = p->str();
		vTmp.push_back(s);
	}

	smatch m;
	regex reg(R"(([&|]+)([a-zA-Z]+)([<=>]+)([0-9.]+))");

	for (auto it : vTmp) {
		PublicCondition pcTmp;

		 if (regex_search(it, m, reg)) {
			 pcTmp.lcComparer = str2LogicComparer(m[1]);
			 pcTmp.pcToCheck = str2pcToCheck(m[2]);
			 pcTmp.rankComparer = str2compareSymbol(m[3]);
			 pcTmp.nCount = stoi(m[4]);
			 ret.push_back(pcTmp);

		}
	}

	return ret;
}

//ʶ��ģʽ����������(Ӳ����ʵ�ֲ���)
Action CMultiStrategy::AdjustActionByMode(const Action& oldAction, const MultiCondition& multiCondition, const PokerEvaluate& pokerEv)
{
	Action newAction = oldAction;
	int nHistorCount = (int)m_ActionHistorys.size();
	if(nHistorCount == 0)
		return newAction;

	//1.flop 2�� ��˳�棬3�ˣ�hero Сע�����ָ�ע��turn����2�������޳�˳�ƣ�hero ���Զ�������1.25%������ע
	//flopֻ��һ����¼��trun�޼�¼��flop����2�� ��˳��,B��hero Сע����ǰ��������2�������޳�˳���޶ԣ���ǰ���3�ˣ� hero����������������ע��hero��������D
	if (nHistorCount == 1 && multiCondition.Round == turn) { //ֻ��flopһ����¼,ĿǰΪturn
		MultiCondition& hMultiCondition = m_ActionHistorys.back().m_multiCondition;
		PokerEvaluate& hPokerEvaluate = m_ActionHistorys.back().m_pokerEv;
		Action& hAction = m_ActionHistorys.back().m_action;
		regex reg(R"(B<>B<>(X|O))");
		RankGroup rgD{ ev_sec_large, evsub_small, 0 };

		if (hPokerEvaluate.publicStruct.nNeedtoFlush == 3 &&
			hAction.fBetSizeByPot < 0.6 &&
			pokerEv.publicStruct.nNeedtoFlush == 3 &&
			pokerEv.publicStruct.nNeedtoStraight != 1 && pokerEv.publicStruct.nNeedtoStraight != 2 &&
			pokerEv.publicStruct.nPair == 0 &&
			pokerEv.rankGroup >= rgD &&
			multiCondition.nPlayerCount <= 3 &&
			multiCondition.HeroActive == hero_initiative &&
			regex_match(multiCondition.sActionLine, reg)){

			newAction.fBetSizeByPot = 1.25;
			vector <pair < Position, Action>> posActions = CStrategy::parseMultiActionSquence(multiCondition.sOriActionInfo);
			newAction.fBetSize = (float)CStrategy::CalcBetSize(multiCondition.dPot, posActions, multiCondition.heroPosition, newAction.fBetSizeByPot);
			double dHeroRamainStack = GetRemainStack(multiCondition.dHeroEStack, posActions, multiCondition.heroPosition);
			if ((float)dHeroRamainStack <= newAction.fBetSize) { //����עsize>=ʣ�����ʱ��תΪallin
				newAction.actionType = allin;
				newAction.fBetSize = (float)dHeroRamainStack;
			}
		}
	}

	//bluff
	//ץզ
	
	return newAction;
}


