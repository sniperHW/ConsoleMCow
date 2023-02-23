//#include "pch.h"
#include "CActionLine.h"
#include "globledef.h"
#include <regex>
#include<utility>
#include<iomanip>
#include<sstream>
#include "CActionMatchConfig.h"
#include <algorithm>
#include "CStrategy.h"
#include "CStrategyTreeConfig.h"
#include <ctype.h>
#include <iostream>
#include "util.h"
#include <iterator>
#include "CWdebug.h"
#include "CPokerHand.h"


using namespace std;

extern map<GameType, CActionMatchConfig> g_actionMatchConfigs; //preflop�ڵ�ƥ���
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //����������

//����[HJ]C,[BTN]C,[SB]F<KsQd7h>pot=15;EStack=[HJ]95.5,[BB]67;
bool CActionLine::Parse(const string& sActionLine, CGame& game)
{
	CWdebug::Log(game.m_sID + " "+ sActionLine);

	regex reg;
	smatch m;
	bool blIsChangeRound = false;
	string sNodeName;
	string sActionLineTmp = sActionLine;
	string sBoard;
	Stacks stacksOri{0,0};
	double dPotNew = 0;

	//��¼ԭʼ��Ϣ,��ʽ [position]Actionsize,..<>... (allin��Ҫ��size,�Ѿ�allin��Ҫ������)
	auto idx = sActionLine.find_first_of('<');
	string sAppend;
	bool blStreet = false;
	if (idx != string::npos) {
		sAppend = sActionLine.substr(0, idx);
		blStreet = true;
	}
	else
		sAppend = sActionLine;
	char lastc;
	if (!m_sOriActionInfo.empty())
		lastc = m_sOriActionInfo[m_sOriActionInfo.size() - 1];
	if (!m_sOriActionInfo.empty() && lastc != '>')
		m_sOriActionInfo += ',';
	m_sOriActionInfo += sAppend;
	if (blStreet)
		m_sOriActionInfo += "<>";

	//���������ֵĳ��루���ڼ���NodeName��
	stacksOri.dPot = game.m_dPot;
	auto it = min_element(game.m_players.begin(), game.m_players.end(), [](auto elem1, auto elem2) {return elem1.second.m_dEStack < elem2.second.m_dEStack; });
	stacksOri.dEStack = it->second.m_dEStack;

	//�ж���hero action����change round(����������������ֻ����һ��<>,������Ϣ�ǻ�����أ��Ƚ�������)
	reg = R"(\<(.*)\>)";
	if (regex_search(sActionLine, m, reg)) { //change round
		//���� board(flopҪ�����䵽1755�������)
		sBoard = m[1];
		if (game.m_round == preflop) {
			game.m_board.SetFlop(sBoard);
			sBoard = game.m_board.GetBoardSymbol();
		}
		else if(game.m_round == flop)
			game.m_board.SetTurn(sBoard);
		else if (game.m_round == turn)
			game.m_board.SetRiver(sBoard);

		blIsChangeRound = true;

		//��ȡ<>ǰ�����ڷ���
		auto idx = sActionLine.find_first_of('<');
		if (idx != string::npos)
			sActionLineTmp = sActionLine.substr(0, idx);	 

		//����
		MyCards privates(ToMyCards(game.GetHero()->m_hands.m_sSymbol));
		MyCards publics(ToMyCards(game.m_board.GetBoardSymbol()));
		CPokerHand pokerHand;
		m_pokerEv = pokerHand.getPokerEvaluate(privates, publics);
	}//end of change round

	//analysis action line��[position]actionsize,","�ŷָpreflop��Ȧ��"-"��
	vector<pair<Position,Action>> posActions;
	regex sep(R"(\s?,\s?)");
	regex regLimp(R"(^(C+))");
	string sRowActionSquence{};	//m_sPreflopRowActionSquenceΪpreflopȫ����sRowActionSquenceΪ������preflop��sRowActionSquenceƴ�ӵ�m_sPreflopRowActionSquence�����Դ˻�ȡAbbrName,flop����ƴ�ӵ�actionSquence
	bool blFirstSec = m_sPreflopRowActionSquence.empty() ? true : false; //����heroǰ��һ�����ж�
	bool blFirstCycle = true; //��һȦ�ж����Ƿ���֡�-��
	if (m_sPreflopRowActionSquence.find('-') != string::npos)
		blFirstCycle = false;
	bool blLimpDoubleR = false; //limp�ߺ�������R


	sregex_token_iterator p(sActionLineTmp.cbegin(), sActionLineTmp.cend(), sep, -1);
	sregex_token_iterator e;
	for (; p != e; ++p) {
		if(!p->str().empty())
			posActions.push_back(ToPosActionPair(p->str()));	//"-"���Ϊnone,nonepos
	}

	//�����һ�ж��Ƿ�Ϊhero
	if (!blIsChangeRound) {
		Position nextPlayerPosition;
		if (!posActions.empty()) {
			if (posActions.back().first == nonepos)
				nextPlayerPosition = game.GetNextPlayerPosition(prev(posActions.end()-1)->first);
			else
				nextPlayerPosition = game.GetNextPlayerPosition(posActions.back().first);
			if (!game.m_players[nextPlayerPosition].m_blIsHero) {
				cout << "error: ActionLine Parse:Next player is not hero" << endl<< endl;
				return false;
			}
		}
	}

	//��¼player���һ���ж�
	Action heroLastAction;
	for (auto pa : posActions) {
		if (pa.first != nonepos) {
			game.m_players[pa.first].m_lastAction = pa.second;
			if(game.m_players[pa.first].m_blIsHero)
				heroLastAction = pa.second;
		}
	}


	//�ж��Ƿ�������ģʽ
	if (blIsChangeRound && game.m_round == preflop) {
		int nExistPlayerCount = 0;
		for (auto player : game.m_players) {
			if (player.second.m_lastAction.actionType != fold)
				nExistPlayerCount++;
		}

		if (nExistPlayerCount > 2) {
			m_blIsMultiPlayer = true;
		}
	}


	//������˶Կ����Ƕ��˶Կ�ʱofflineҲҪ�� MultiCondition m_multiCondition; 
	m_multiCondition.sActionLine = GetBetCountStr(blIsChangeRound); //�Ƕ���offlineʱҲҪ��
	m_multiCondition.sOriActionInfo = m_sOriActionInfo;
	m_multiCondition.heroPosition = game.GetHero()->m_position;
	m_multiCondition.dPot = game.m_dPot;
	m_multiCondition.dHeroEStack = game.GetHero()->m_dEStack;

	//nPlayerCount;
	m_multiCondition.nPlayerCount = 0;
	for (auto player : game.m_players) {
		if (player.second.m_lastAction.actionType != fold)
			m_multiCondition.nPlayerCount++;
	}

	//Multi_Position Position;
	m_multiCondition.MultiPosition = game.GetHeroMultiPosition();

	if (blIsChangeRound) {
		m_multiCondition.Round = getNextRound(game.m_round); //Round Round;
		m_multiCondition.HeroActive = heroLastAction.actionType == raise ? hero_initiative : hero_passive; //Multi_HeroActive HeroActive; //ǰһ����call����raise,������m_players��lastActionʶ��
		//����round
		game.m_round = getNextRound(game.m_round);
		//����pot
		reg = R"(pot=(.*);)";
		if (regex_search(sActionLine, m, reg))
			game.m_dPot = stod(m[1]);
		//����EStack
		double dHeroEStack = 0;
		double dMaxEStack = 0;
		reg = R"(EStack=(.*);)";
		if (regex_search(sActionLine, m, reg)) {
			string sEStacks = m[1];
			regex sep(R"(\s?,\s?)");
			sregex_token_iterator p(sEStacks.cbegin(), sEStacks.cend(), sep, -1);
			sregex_token_iterator e;
			for (; p != e; ++p) {
				regex regpos(R"(\[(.*)\](.*))");
				string s = p->str();
				if (regex_search(s, m, regpos)) {
					game.m_players[CGame::GetPositionBySymbol(m[1])].m_dEStack = stod(m[2]);
					if (game.m_players[CGame::GetPositionBySymbol(m[1])].m_blIsHero)
						dHeroEStack = stod(m[2]);
					else {
						if (stod(m[2]) > dMaxEStack)
							dMaxEStack = stod(m[2]);
					}
				}
			}

			//dlSpr;
			m_multiCondition.dlSpr = min(dHeroEStack, dMaxEStack) / game.m_dPot;
		}
		//ȥ��fold�����
		for (auto it = game.m_players.begin(); it != game.m_players.end();) {
			if (it->second.m_lastAction.actionType == fold) {
				it = game.m_players.erase(it);
			}
			else {
				it++;
			}
		}
	}
	else {
/*
		//preflop Spr (��preflop����ʱ����)
		double dHeroEStack = 0;
		double dMaxEStack = 0;
		if (game.m_round == preflop) {
			for (auto player : game.m_players) {
				if (player.second.m_lastAction.actionType != fold) {
					if (player.second.m_blIsHero)
						dHeroEStack = player.second.m_dEStack;
					else{
						if (player.second.m_dEStack > dMaxEStack)
							dMaxEStack = player.second.m_dEStack;
					}
				}
			}
			m_multiCondition.dlSpr = min(dHeroEStack, dMaxEStack) / game.m_dPot;
		}
*/
		//dlBetSize;
		vector<pair<Position, Action>> posActionsMulti = CStrategy::parseMultiActionSquence(m_sOriActionInfo);
		m_multiCondition.dlBetSize = CStrategy::CalcMultiBetRatio(game.m_dPot, posActionsMulti);

		//dlCurrentSpr
		m_multiCondition.dlSprCurrent = GetCurrentSpr(game, posActionsMulti);
	}

	if (m_blIsMultiPlayer)
		return true;







	//��Limp�ĺϳ�һ��
	if (game.m_round == preflop) {
		if (blFirstSec) { //��һ��
			//�ж�limpģʽ,��¼limp��
			ActionType firstat = fold;
			Position firstpos;
			for (auto p : posActions) { //��¼��һ����Ч����
				if (p.second.actionType != fold && p.second.actionType != none) {
					firstat = p.second.actionType;
					firstpos = p.first;
					break;
				}
			}
			if (firstat == call && firstpos != SB) {	//��һ����Ч����Ϊcall���¼������call����Ϊlimper
				auto pos = posActions.begin();
				while (pos != posActions.end()) {
					if( pos->second.actionType == call)
						m_limps.push_back(pos->first);
					if (pos->second.actionType == raise || pos->second.actionType == allin)
						break;
					pos++;
				}
			}
			if (m_limps.size() > 0)
				m_firstLimperPositionByPresent = game.m_players[m_limps[0]].m_positionByPresent;
		}
		//��posActions��limp�ߺϳ�һ����posActions�ĵ�һ���滻ΪfirstLimp(position�̶�Ϊm_limps�ĵ�һ��)���滻������ɾ��
		pair<Position, Action> removeValue{ nonepos,{none,100,100,100} }; //��posActions�е�limp�߼���������־Ϊɾ��value
		if (m_limps.size() > 0) {
			int nFirst = -1; int nCount = 0; int nRCount = 0; int nACount = 0; int nCCount = 0; 
			auto pos = posActions.begin();
			while (pos != posActions.end()) {
				if (find(m_limps.cbegin(), m_limps.cend(), pos->first) != m_limps.cend()) {
					if (nFirst == -1)
						nFirst = nCount;
					if (pos->second.actionType == allin)
						nACount++;
					else if (pos->second.actionType == call)
						nCCount++;
					else if (pos->second.actionType == raise)
						nRCount++;

					*pos = removeValue;
				}
					
				pos++;
				nCount++;
			}
			
			for (auto it = posActions.begin(); it != posActions.end();) {	//��ɾ��limp��
				if (it->first == nonepos && it->second.fBetSizeByPot > 0) {
					it = posActions.erase(it);
				}
				else {
					it++;
				}
			}

			Action aTmp{none,0,0,0};	//����limp�����
			if (nACount > 0)
				aTmp.actionType = allin;
			else if (nRCount > 0) {
				aTmp.actionType = raise;
				if (nRCount > 1)
					blLimpDoubleR = true;//תΪ�̶�AbbrName
			}
			else if (nCCount > 0) {
				aTmp.actionType = call;
				if (blFirstSec)
					aTmp.actionType = raise;//��һȦתΪR�������ı���call
			}
			else
				aTmp.actionType = fold;

			auto ipos = posActions.begin() + nFirst;
			posActions.insert(ipos, make_pair(m_limps[0], aTmp));
		}
	}//end of limp

	//תΪrowActionSquence(���˵�������,preflop����"-",ȥ��size,flop��ÿ����������"-"�ָ�)
	if (game.m_round == preflop) {
		for (auto pa : posActions) {
			if (pa.second.actionType == none) {
				sRowActionSquence += "-";
				blFirstCycle = false;
			}
			else if(pa.second.actionType == fold) {
				if (!blFirstCycle) {//���ǵ�һ����FҪ������players��ҲҪ��ʱ����
					sRowActionSquence += ToActionSymbol(pa.second, false);	//preflop����Ҫ����size����˵��ò�����false
				}
				else { //��һȦ��Ҫ��players��ȥ��F��player
					game.m_players.erase(pa.first);
				}
			}
			else
				sRowActionSquence += ToActionSymbol(pa.second,false);
		}

		//ƴ��ԭActionSquence��"O"Ҫȥ��,��û����Ϊ��O��
		if (m_sPreflopRowActionSquence.empty() && sRowActionSquence.empty())
			m_sPreflopRowActionSquence = "O";
		else if (m_sPreflopRowActionSquence == "O")
			m_sPreflopRowActionSquence = sRowActionSquence;
		else
			m_sPreflopRowActionSquence += sRowActionSquence;
	}
	else { //flop��
		for (auto pa : posActions) {
			if(sRowActionSquence.empty())
				sRowActionSquence += ToActionSymbol(pa.second);
			else 
				sRowActionSquence = sRowActionSquence + "-" + ToActionSymbol(pa.second);
		}

		//ƴ��ԭActionSquence��"O"Ҫȥ��,��û����Ϊ��O��
		if (m_sActionSquence.back() == '>' && sRowActionSquence.empty())
			m_sActionSquence += "O";
		else if (m_sActionSquence.back() == 'O')	//��Ϊ"ֻ��һ��"
			m_sActionSquence.pop_back();
		else
			m_sActionSquence.back() == '>' ? m_sActionSquence += sRowActionSquence : m_sActionSquence = m_sActionSquence + "-" + sRowActionSquence;
	}

	//����ǰ��ҳ������ȷ����Ϸ����
	if (game.m_round == preflop)
		game.m_gmType = GetCurGameType(game);

	//����preflop��m_sActionSquence��m_sAbbrName
	if (game.m_round == preflop) {
		//ƥ��AbbrName
		if (blLimpDoubleR) //limp��˫RתΪ�̶�����
			m_sAbbrName = sDoubleNLimpRAbbr;
		else {
			ActionMatchMode match_mode = blIsChangeRound ? match_toflop : match_preflop;
			m_sAbbrName = g_actionMatchConfigs[game.m_gmType].GetAbbrName(match_mode, m_sPreflopRowActionSquence);
			if (m_sAbbrName.empty()) {
				cout << "error: ActionLineParse AbbrName not find," << "GameType:" << GameTypeName[game.m_gmType] << "; " << "m_sPreflopRowActionSquence:" << m_sPreflopRowActionSquence << endl << endl;
				return false;
			}
		}

		//��AbbrName��position��д����ActionSquence,limp��Ҫ���⴦��
		sNodeName = m_sAbbrName;
		
		Position heroPosition = nonepos;//HeroActionʱheroΪʵ��hero,changeRoundʱheroΪ���call��
		if (!blIsChangeRound)
			heroPosition = game.GetHero()->m_position;
		else {
			for(auto rp = posActions.rbegin(); rp != posActions.rend(); rp++)
				if (rp->second.actionType == call || rp->second.actionType == check) {
					heroPosition = rp->first;
					break;
				}
		}

		if (heroPosition == nonepos)
			cout << "error: heroPosition = nonepos" << endl;

		//�滻hero
		reg = R"(\[hero\])";
		sNodeName = regex_replace(sNodeName, reg, PositionSymble[game.m_players[heroPosition].m_positionByPresent]);

		//�滻rivals
		reg = R"(\[rivals\])";
		string sRivals;
		if (m_limps.size() > 0) { //�����limp��m_players�л���limp�������m_limps�ĵ�һ��λ��
			for (auto p : game.m_players) {
				if (find(m_limps.begin(), m_limps.end(), p.first) != m_limps.end()){
					sRivals += PositionSymble[m_firstLimperPositionByPresent];
					break;
				}
			}
		}
		if (blFirstSec) { //hero��һ���ж���posActions����Ϊm_players�б�����Ϊ�ж������
			for (auto p : posActions) {
				if (p.second.actionType != fold && p.second.actionType != none) {
					if (p.first != heroPosition) {
						if (find(m_limps.begin(), m_limps.end(), p.first) == m_limps.end()) //�ų�limp��
							sRivals.empty() ? sRivals += PositionSymble[game.m_players[p.first].m_positionByPresent] : sRivals = sRivals + "," + PositionSymble[game.m_players[p.first].m_positionByPresent];
					}
				}
			}
		}
		else {	//�ǵ�һȦ��game.m_players
			for (auto p : game.m_players) {
				if (p.first != heroPosition) {
					if (find(m_limps.begin(), m_limps.end(), p.first) == m_limps.end()) //�ų�limp��
						sRivals.empty() ? sRivals += PositionSymble[p.second.m_positionByPresent] : sRivals = sRivals + "," + PositionSymble[p.second.m_positionByPresent];
				}
			}
		}
		sNodeName = regex_replace(sNodeName, reg, sRivals);

		//�滻oopx(λ��hero>���R��OOPA)
		reg = R"(\[oopx\])";
		string sOOPX;
		Position pTmp;
		for (auto p : game.m_players) { //�ҳ����R��λ��
			double dlMax = 0;
			if (!p.second.m_blIsHero) {
				if (p.second.m_lastAction.actionType == raise && p.second.m_lastAction.fBetSize > dlMax) {
					dlMax = p.second.m_lastAction.fBetSize;
					pTmp = p.second.m_positionByPresent;
				}
			}
		}

		if (find(m_limps.cbegin(), m_limps.cend(), pTmp) != m_limps.cend())	//�����Limper���滻
			pTmp = m_limps[0];
		sOOPX = GetOOPXSymbol(game.GetHero()->m_positionByPresent, pTmp);
		sNodeName = regex_replace(sNodeName, reg, sOOPX);

		//$���:������������allin,��ʵ�ʶ�����û��Aʱ��callҪתΪallin��
		if (sNodeName.find("allin") != string::npos) {
			bool blWithA = false;
			for (auto p : posActions) {
				if (p.second.actionType == allin) {
					blWithA = true;
					break;
				}
			}
			if (!blWithA)
				sNodeName += "$";
		}

		//@���:ʵ�ʶ�����allinʱ�ӣ�allin��raiseҪת��call��
		for (auto p : posActions) {
			if (p.second.actionType == allin) {
				sNodeName += "@";
				break;
			}
		}

		m_sActionSquence = sNodeName;
	}//end of preflop

	if (blIsChangeRound) {
		//����round
		game.m_round = getNextRound(game.m_round);

		//����pot
		reg = R"(pot=(.*);)";
		if (regex_search(sActionLine, m, reg))
			game.m_dPot = stod(m[1]);

		//����EStack
		reg = R"(EStack=(.*);)";
		if (regex_search(sActionLine, m, reg)) {
			string sEStacks = m[1];
			regex sep(R"(\s?,\s?)");
			sregex_token_iterator p(sEStacks.cbegin(), sEStacks.cend(), sep, -1);
			sregex_token_iterator e;
			for (; p != e; ++p) {
				regex regpos(R"(\[(.*)\](.*))");
				string s = p->str();
				if (regex_search(s, m, regpos))
					game.m_players[CGame::GetPositionBySymbol(m[1])].m_dEStack = stod(m[2]);
			}
		}

		//ȥ��fold�����
		for (auto it = game.m_players.begin(); it != game.m_players.end();) {
			if (it->second.m_lastAction.actionType == fold) {
				it = game.m_players.erase(it);
			}
			else {
				it++;
			}
		}

		//�������λ��
		if (game.m_round == flop) {
			auto pFirst = game.m_players.begin();
			for (auto pPlay = game.m_players.begin(); pPlay != game.m_players.end(); pPlay++) {
					pPlay->second.m_positionRelative = pPlay == pFirst ? OOP : IP;
			}
		}

		//��actionSquence����ע�ռ����(flopʱm_sActionSquence��NodeNameһ�£�river����Ҫ)
		if (game.m_round == flop)
			m_sNodeName = m_sActionSquence;	//preflop��m_sNodeName��m_sActionSquenceһ��
		else if (game.m_round == turn) {
			vector<Action> actions;
			string sPrefix, actionStrNotUsed;
			Round rNotUsed;
			CStrategy::parseActionSquence(m_sActionSquence, sPrefix, rNotUsed, actions, actionStrNotUsed);
			m_sNodeName = GetNodeName(game.m_gmType, stacksOri, game.m_oopx, actions, sPrefix);	//ֻ�ڻ���ʱ��д��turn���ڶ�λԤ��solver�⣬ֻ�д���Ԥ���Ĳ��������У����򱣳�ԭ����

			string sSuffix;
			auto pos = m_sNodeName.find('>');
			if (pos != string::npos)
				sSuffix = m_sNodeName.substr(pos+1, m_sNodeName.size());
			m_sAbbrName = m_sAbbrName + "<>" + sSuffix;
		}

		//����oopx
		game.m_oopx = GetOOPX(game);

		//ƴ��<board>
		m_sActionSquence = m_sActionSquence + "<" + sBoard + ">";
		m_sNodeName = m_sNodeName + "<" + sBoard + ">";

	}

#ifdef DEBUG_
	cout << endl;
	cout << "ActionLine:" << sActionLine << endl;
	cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
	if (blIsChangeRound)
		cout << "ActionLineParse ChangeRound://///////////////////////////////////////////////////////////////////" << endl;
	else 
		cout << "ActionLineParse HeroAction ://///////////////////////////////////////////////////////////////////" << endl;
	cout << "gmType:" << GameTypeName[game.m_gmType] << "\t" << "OOPX:" << getOOPXString(game.m_oopx) << "\t" << "Board:" << game.m_board.GetBoardSymbol() << "\t" << "Pot:" << double2String(game.m_dPot, 2) << "\t" << "Round:" << getRoundString(game.m_round) << endl;
	cout << "Players:";
	for (auto it : game.m_players) {
		cout << PositionSymble[it.first];
		cout << "[" << PositionSymble[it.second.m_positionByPresent] << "]";
		if (game.m_round != preflop)
			cout << "[" << getRelativePositionString(it.second.m_positionRelative) << "]";
		if (it.second.m_blIsHero)
			cout << "(hero)";
		cout << double2String(it.second.m_dEStack,2);
		cout << "--";		
	}
	cout << endl;
	cout << "ActionSquence:" << m_sActionSquence << endl;
	cout << "AbbrName:" << m_sAbbrName << endl;
	cout << "NodeName:" << m_sNodeName << endl;
	cout << endl ;
#endif

	return true;
}


//��ȡNodeName(����ע�ռ���룬ֻ�ڻ���ʱ���£�flop��wizard��ȡ����Ҫ�������㣩
string CActionLine::GetNodeName(const GameType gmType, const Stacks& stacks, const OOPX oopx, const vector<Action>& actions, const string& sPrefix)
{
	string sSquence, sPreflopName;
	Json::Value StrategyTreeItem;
	int nCount = 0;	//��ǰ�����actions����ţ�1��ʼ
	double dActuallyRatio = 0;	//ʵ����ע����
	vector<double> candidateRatios;	//������ѡ���ϣ���Ӧ����������

	if (actions.empty()) {	//�޴�����hero��һ���ж�
		sSquence = sPrefix + "O";
		return sSquence;
	}

	auto idx = sPrefix.find('<');
	if (idx != string::npos)
		sPreflopName = sPrefix.substr(0, idx);

	for (auto action : actions) {
		nCount++;

		switch (action.actionType) {	//��ֻ��X,R,A����X��Ӧcheck, A��Ӧallin������C������
		case check: {
			nCount > 1 ? sSquence += "-X" : sSquence += "X";
			break;
		}
		case allin: {
			nCount > 1 ? sSquence += "-A" : sSquence += "A";
			break;
		}
		case raise: {
			candidateRatios.clear();

			//ѡ�������ѡ��Χ
			if (nCount == 1) {	//R��һ����Ҫ�ж�OOPA/OOPD�Ծ�����donk����bet��ע�ռ�
				if (oopx == OOPD) {
					//strategyTreeItem��ȡdonk��ע�ռ�

					g_strategyTreeConfigs[gmType].GetFlopCandidateRatios(sPreflopName, candidateRatios, space_donk);
					//candidateRatios = { 33 }; //for test///////////////////////////////////
				}
				else if (oopx == OOPA) {
					//strategyTreeItem��ȡbet��ע�ռ�
					g_strategyTreeConfigs[gmType].GetFlopCandidateRatios(sPreflopName, candidateRatios, space_bet);
					//candidateRatios = { 33,50,75,125 };  //for test///////////////////////////////////
				}
			}
			else {
				if (nCount == 2 && actions[0].actionType == check) {
					//�����XR��strategyTreeItem��ȡbet��ע�ռ�
					g_strategyTreeConfigs[gmType].GetFlopCandidateRatios(sPreflopName, candidateRatios, space_bet);
					//candidateRatios = { 33,50,75,125 };  //for test///////////////////////////////////
				}
				else {
					//strategyTreeItem��ȡraise��ע�ռ�
					g_strategyTreeConfigs[gmType].GetFlopCandidateRatios(sPreflopName, candidateRatios, space_raise);
					//candidateRatios = { 50,100 };  //for test///////////////////////////////////
				}
			}

			//����dActuallyRatio,��ǰ���ڼ����actions��Ԫ�صı���
			dActuallyRatio = CStrategy::CalcBetRatio(stacks.dPot, actions, nCount - 1);

			if (candidateRatios.empty()) { //�޺�ѡ��Χ�����NodeName���ᱻ����flop wizard��turn solver presave,��˱���ԭ��������
				nCount > 1 ? sSquence += "-R" + to_string(int(dActuallyRatio)) : sSquence += "R" + to_string(int(dActuallyRatio));
			}
			else
			{
				//��ѡ��Χ����allin�ı������൱�����һ��Ԫ���滻Ϊallin�����㣩,��ȷ��allinΪ������
				if (nCount == actions.size()) {
					double dAllinRatio = CStrategy::CalcBetRatio(stacks.dPot, actions, nCount - 1, stacks.dEStack);
					while (candidateRatios.back() > dAllinRatio)
						candidateRatios.pop_back();
					candidateRatios.push_back(int(dAllinRatio));
				}
				int iMatchedIndex = CStrategy::MatchBetRatio(dActuallyRatio, candidateRatios);
				if (nCount == actions.size() && iMatchedIndex == candidateRatios.size() - 1)	//���������ƥ��allin��תΪallin
					nCount > 1 ? sSquence += "-A" : sSquence += "A";
				else
					nCount > 1 ? sSquence += "-R" + to_string(int(candidateRatios[iMatchedIndex])) : sSquence += "R" + to_string(int(candidateRatios[iMatchedIndex]));
			}
		} //end of case raise
		default:
			break;
		} //end of switch
	}// end of for

	if (sSquence == "X-X")	//X-X���һ������C�����Ҫ���⴦��
		sSquence = "X";

	sSquence = sPrefix + sSquence;

	return sSquence;

}

string CActionLine::GetNodeNameWithoutBoard(const string& sNodeName)
{
	regex reg(R"(\<.*\>)");
	string s = regex_replace(sNodeName, reg, "<>");
	return s;
}


//����[UTG]R5
pair<Position, Action> CActionLine::ToPosActionPair(const string& sSymbol)
{
	Action a{none,0,0,0};
	Position p;
	regex regPosition(R"(\[(.*)\])");
	regex regAction(R"(\]([FRCAX])(.*))");
	smatch m;

	if (sSymbol == "-") {
		a = { none,0,0,0 };
		return make_pair(nonepos, a);
	}

	if (regex_search(sSymbol, m, regPosition)) {
		p = CGame::GetPositionBySymbol(m[1]);
	}

	if (regex_search(sSymbol, m, regAction)) {
		if (m[1].str() == "X") {
			a.actionType = check;
		}
		else if (m[1].str() == "R") {
			a.actionType = raise;
			a.fBetSize = stof(m[2].str());
		}
		else if (m[1].str() == "A") {
			a.actionType = allin;
		}
		else if (m[1].str() == "C") {
			a.actionType = call;
		}
		else if (m[1].str() == "F") {
			a.actionType = fold;
		}
	}

	return make_pair(p, a);
}

string CActionLine::ToActionSymbol(const Action& a,bool blRemainSize) {


	if (a.actionType == raise) {
		if (blRemainSize) {
			stringstream ss;
			ss << setiosflags(ios::fixed) << setprecision(1) << "R" << a.fBetSize;
			return ss.str();
		}
		else
			return "R";
	}
	else if (a.actionType == fold)
		return "F";
	else if (a.actionType == check)
		return "X";
	else if (a.actionType == allin)
		return "A";
	else if (a.actionType == call)
		return "C";

	return "E"; //�����־

}


//��������Ҽ���ƥ�����Ϸ����	//δ����
GameType CActionLine::GetCurGameType(const CGame& game)
{
	//������EStack���ֵ��heroȡСֵ
	double dHeroEStack = 0;
	double dRivalMaxEStack = 0;
	double dChose = 0;
	for (auto player : game.m_players) {
		if (player.second.m_blIsHero)
			dHeroEStack = player.second.m_dEStack;
		else {
			if (player.second.m_lastAction.actionType != fold)
				dRivalMaxEStack = player.second.m_dEStack > dRivalMaxEStack ? player.second.m_dEStack : dRivalMaxEStack;
		}
	}

	dChose = dHeroEStack <= dRivalMaxEStack ? dHeroEStack : dRivalMaxEStack;
	return GetGameTypeByStackDepth(MatchStackDepth(dChose));
}

//����Ч����ƥ���ѡ���
int CActionLine::MatchStackDepth(double dEStack)
{
	double dLowbound = 0.4; //ƥ��С��������

	if (dEStack <= CandicateStackDepth[0])
		return CandicateStackDepth[0];
	if (dEStack >= CandicateStackDepth[CandicateStackDepth.size() - 1])
		return CandicateStackDepth[CandicateStackDepth.size() - 1];

	int nCount = 0;
	for (auto pos = CandicateStackDepth.cbegin(); pos + 1 != CandicateStackDepth.cend(); ++pos) {
		auto posNext = pos + 1;
		if (dEStack >= *pos && dEStack < *posNext) {
			double dDvalue = *posNext - *pos;
			double dAllowExceed = dLowbound * dDvalue;

			if (dEStack < *pos + dAllowExceed)
				return *pos;
			else
				return *(pos + 1);
		}
	}

	return 0;
}

//����ѡ������ȵó���Ϸ����
GameType CActionLine::GetGameTypeByStackDepth(int nStackDepth)
{
	//�����Max6_NL50��������ʵ��
	switch (nStackDepth) {
	case 20:
		return Max6_NL50_SD20;
	case 50:
		return Max6_NL50_SD50;
	case 75:
		return Max6_NL50_SD75;
	case 100:
		return Max6_NL50_SD100;
	case 150:
		return Max6_NL50_SD150;
	default:
		return Max6_NL50_SD100;
	}
}

string CActionLine::GetOOPXSymbol(const Position hero, const Position rival)
{
	map<Position, int> m;
	m[UTG] = 3;
	m[HJ] = 4;
	m[CO] = 5;
	m[BTN] = 6;
	m[SB] = 1;
	m[BB] = 2;

	if (m[hero] > m[rival])
		return "OOPA";
	else
		return "OOPD";
}

//�����ж�������������R��OOPA������OOPD
OOPX CActionLine::GetOOPX(const CGame& game)
{
	map<Position, int> m;
	m[UTG] = 3;
	m[HJ] = 4;
	m[CO] = 5;
	m[BTN] = 6;
	m[SB] = 1;
	m[BB] = 2;

	Position posFirst = game.m_players.begin()->first;
	for (auto it : game.m_players) {
		if (m[it.first] < posFirst)
			posFirst = it.first;
	}
	
	OOPX oopx = game.m_players.at(posFirst).m_lastAction.actionType == raise ? OOPA : OOPD;
	return oopx;
}

void CActionLine::EraseBlank(std::string& sSource)
{
	for (auto it = sSource.begin(); it != sSource.end(); it++) {
		if (isblank(*it) != 0)
			sSource.erase(it--);
	}
}

string CActionLine::getOOPXString(const OOPX oopx)
{
	switch (oopx) {
	case OOPA:
		return "OOPA";
	case OOPD:
		return "OOPD";
	default:
		return "";
	}
}

string CActionLine::getRoundString(const Round round)
{
	switch (round) {
	case preflop:
		return "preflop";
	case flop:
		return "flop";
	case turn:
		return "turn";
	case river:
		return "river";
	default:
		return "";
	}
}

string CActionLine::getRelativePositionString(const RelativePosition rp)
{
	switch (rp) {
	case OOP:
		return "OOP";
	case IP:
		return "IP";
	default:
		return "";
	}
}

Round CActionLine::getNextRound(const Round round)
{
	switch (round) {
	case preflop:
		return flop;
	case flop:
		return turn;
	case turn:
		return river;
	default:
		return preflop;
	}
}

string CActionLine::GetBetCountStr(const bool blIsChangeRound)
{
	string sRet;

	//�ֽ�
	regex sep(R"(\<\>)");
	vector<string> vRound;
	sregex_token_iterator p(m_sOriActionInfo.cbegin(), m_sOriActionInfo.cend(), sep, -1);
	sregex_token_iterator e;
	for (; p != e; ++p)
		vRound.push_back(p->str());

	bool blOpen = false;
	if(vRound.empty())
		blOpen = true;
	else {
		if(vRound.back().empty())
			blOpen = true;
	}

	vector<int> counts;

	for (auto one_round : vRound) {
		//�ֶ���
		regex sep(R"(\s?,\s?)");
		sregex_token_iterator p(one_round.cbegin(), one_round.cend(), sep, -1);
		sregex_token_iterator e;
		int nRCount = 0;
		float fLastRaiseSize = 0;
		for (; p != e; ++p) {
			if (!p->str().empty()) {
				regex regAction(R"(\]([FRCAX])(.*))");
				smatch m;
				string sSymbol = p->str();
				if (regex_search(sSymbol, m, regAction)) {
					if (m[1].str() == "R" || m[1].str() == "A") {
						double dSize = stof(m[2].str());
						if (dSize > fLastRaiseSize) {
							fLastRaiseSize = (float)dSize;
							nRCount++;
						}
					}
				}
			}
		}
		counts.push_back(nRCount);
	}

	for (int i = 0; i < counts.size(); i++) {
		if (counts[i] == 0) {
			if(i == counts.size()-1 && blOpen)
				sRet += "O";
			else
				sRet += "X";
		}
		else if (counts[i] == 1)
			sRet += "B";
		else
			sRet = sRet + to_string(counts[i] + 1) + "B";

		if (i != counts.size() - 1)
			sRet += "<>";
	}

	if (!m_sOriActionInfo.empty()) {
		if (m_sOriActionInfo.back() == '>') {
			sRet += "<>";
			if (!blIsChangeRound)
				sRet += "O";
		}
	}

	return sRet;
}

//���ַ�����ʽתΪMyCards
MyCards CActionLine::ToMyCards(const std::string sSymbol)
{
	MyCards myCards;
	for (int i = 0; i < sSymbol.size(); i+=2) {
		string s;
		s.append(1,sSymbol[i]);
		s.append(1, sSymbol[i+1]);
		myCards.push_back(myCard(s));
	}
	return myCards;
}

//���㵱ǰhero��ʣ�����͵�ǰ�׳صı���
double CActionLine::GetCurrentSpr(CGame& game, const vector<pair<Position, Action>>& posActions)
{
	double dCurrentpot = game.m_dPot;
	double dHeroEStack = game.GetHero()->m_dEStack;
	Position heroPosition = game.GetHero()->m_position;

	double dRecentRSize = 0;
	for (auto one : posActions) {
		if (one.second.actionType == raise || one.second.actionType == allin) {
			dRecentRSize = one.second.fBetSize;
			dCurrentpot += one.second.fBetSize;
			if (one.first == heroPosition)
				dHeroEStack -= one.second.fBetSize;
		}
		else if (one.second.actionType == call) {
			dCurrentpot += dRecentRSize;
			if (one.first == heroPosition)
				dHeroEStack -= dRecentRSize;
		}
	}

	return dHeroEStack / dCurrentpot;
}
