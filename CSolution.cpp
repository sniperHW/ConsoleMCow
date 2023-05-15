//#include "pch.h"
#include "CSolution.h"
#include <regex>
#include <iostream>
#include "CStrategyTreeConfig.h"
#include "CSolver.h"
#include "CFlopDataFromWizardConfig.h"
#include "CTurnPresaveSolverConfig.h"
#include "CStackByStrategyConfig.h"
#include "CDataFrom.h"
#include <fstream>
#include "util.h"
#include "CActionLine.h"
#include "CMultiStrategy.h"
#include "CWdebug.h"

using namespace std;
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; 
extern CSolver g_solver;
extern map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs;
extern map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs;
extern map<GameType, CStackByStrategyConfig> g_stackByStrategyConfig;
extern CMultiStrategy g_multiStrategy;

CSolution::CSolution()
{
	m_strategyFrom = from_strategy_file;
	m_blNotOffline = true;

	for (auto combo : ComboMapping) 
		m_heroCurRange[combo] = 1;
}


Action CSolution::HeroAction(const string& sActionLine)
{
	CStrategy strategy;
	CStrategy strategyLow,strategyUp;

	if (!m_actionLine.Parse(sActionLine, m_game)) {
		m_blNotOffline = false;
		cout << "error: Parse failed,sActionLine = " << sActionLine << endl;
		return ProcessingOffline();
	}

	if (!m_blNotOffline) 
		return ProcessingOffline();

	string sHeroAction = DumpSelAction(sActionLine);	//��� SelA
	if(!sHeroAction.empty())
		updataHeroCurRange(sHeroAction);


	#ifdef FOR_FLOW_
	cout << "HeroAction: " << m_game.m_sID << " " << m_actionLine.m_sActionSquence;
	cout << " " << "[" << GameTypeName[m_game.m_gmTypeBound.first] << "," << GameTypeName[m_game.m_gmTypeBound.second] << "]";
	cout << " " << strategyFrom2str(m_strategyFrom);
	cout << endl;
	#endif
	

	switch (m_strategyFrom)
	{
	case from_strategy_file: {
		//for test
		m_blNotOffline = true; 

/*
		//��ʽ���룬����ʱע�͵�

		bool blNotOffline = true;
		if (m_game.m_gmTypeBound.first != GAMETYPE_none) 
			blNotOffline = blNotOffline && strategyLow.Load(m_game.m_gmTypeBound.first, m_actionLine.m_sActionSquence);
		else
			cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;

		if (m_game.m_gmTypeBound.second != GAMETYPE_none) 
			blNotOffline = blNotOffline && strategyUp.Load(m_game.m_gmTypeBound.second, m_actionLine.m_sActionSquence);

		if (blNotOffline) {
			if (m_game.m_gmTypeBound.second == GAMETYPE_none)
				strategy = strategyLow;
			else {
				int nTemplate = m_game.m_gmType == m_game.m_gmTypeBound.first ? 0 : 1;
				strategy.m_strategy = CStrategy::getAverageStrategyByStackDepth(strategyLow.m_strategy, strategyUp.m_strategy, m_game.m_dSegmentRatio, nTemplate,true);
			}
		}
*/
		break;
	}
	case from_wizard: { //�������
		cout << "not support from_wizard" << endl;
		m_blNotOffline = strategy.Load(m_game.m_gmType, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_oopx, m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());
		break;
	}
	case from_solver_presave: {
		if (m_actionLine.m_sActionSquence.find('$') != string::npos) //�������������-Aʱ��Ӹñ�ǣ���solverģʽ����Ҫ����
			m_actionLine.m_sActionSquence.pop_back();

		bool blNotOffline = true;
		if (m_game.m_gmTypeBound.first != GAMETYPE_none) {
			shared_ptr<Stacks> pStacksByStrategy = g_stackByStrategyConfig[m_game.m_gmTypeBound.first].GetItemByName(m_actionLine.m_sNodeName); //��ȡ���Զ�Ӧ�ĳ���
			Stacks StacksByStrategy;
			if (pStacksByStrategy == nullptr) {
				StacksByStrategy = GetStacks();	//û��ƥ��������ʵ�ʵĳ���
				cout << "error: GetStackByStrategyConfig not matched,NodeName: " << m_actionLine.m_sNodeName << endl << endl;
			}
			else
				StacksByStrategy = *pStacksByStrategy;
			blNotOffline = blNotOffline && strategyLow.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
		}
		else
			cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;
	
		if (m_game.m_gmTypeBound.second != GAMETYPE_none) {
			shared_ptr<Stacks> pStacksByStrategy = g_stackByStrategyConfig[m_game.m_gmTypeBound.second].GetItemByName(m_actionLine.m_sNodeName); //��ȡ���Զ�Ӧ�ĳ���
			Stacks StacksByStrategy;
			if (pStacksByStrategy == nullptr) {
				StacksByStrategy = GetStacks();	//û��ƥ��������ʵ�ʵĳ���
				cout << "error: GetStackByStrategyConfig not matched,NodeName: " << m_actionLine.m_sNodeName << endl << endl;
			}
			else
				StacksByStrategy = *pStacksByStrategy;
			blNotOffline = blNotOffline && strategyUp.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
		}
		
		if (blNotOffline) {
			if (m_game.m_gmTypeBound.second == GAMETYPE_none)
				strategy = strategyLow;
			else {
				int nTemplate = m_game.m_gmType == m_game.m_gmTypeBound.first ? 0 : 1;
				strategy.m_strategy = CStrategy::getAverageStrategyByStackDepth(strategyLow.m_strategy, strategyUp.m_strategy, m_game.m_dSegmentRatio, nTemplate);
			}
		}

		if (!blNotOffline) { //���������ļ������¼���,����޷����¼�����allin����call allinתallin��
			vector<double> abnormalSizes; //1:opp bet size  2:ip raise size
			bool blSupportRecalc = strategy.GetAbnormalSizes(GetStacks().dPot, m_actionLine.m_sActionSquence, abnormalSizes);

			if (blSupportRecalc) {
				#ifdef FOR_FLOW_
				cout << "abnormalSize recalc: " <<  m_actionLine.m_sActionSquence << endl;
				#endif

				CSolverConfig solverConfig;
				solverConfig.m_stacks = GetStacks();
				solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
				solverConfig.m_pRange = &m_range;
				solverConfig.m_pRTTreeConfigItem = nullptr;
				m_game.m_board.ClearSuitReplace(); //ʵʱ���㲻��Ҫͬ��ת��

				m_blNotOffline = g_solver.ToSolveSimple(abnormalSizes, m_game.m_round, m_game.m_sID, solverConfig, m_solverResultLow);
				if (m_blNotOffline) {
					m_game.m_board.ClearSuitReplace(); //ʵʱ���㲻��Ҫͬ��ת��
					m_blNotOffline = strategy.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());
				}

				m_strategyFrom = from_solver_calc;
			}
			else {
				#ifdef FOR_FLOW_
				cout << "abnormalSize failed to recalc, convert to allin: " << m_actionLine.m_sActionSquence << endl;
				#endif

				//����m_actionLine.m_sActionSquence����allin����
				m_blNotOffline = strategy.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), GetStacks(), m_game.m_board.GetIsomorphismSuitReplace(),true);
			}
		}
		break;
	}
	case from_solver_calc: {
//��鲢�ȴ�������ɣ���Ҫ��Ӵ��룩
//for test
		//m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());	//���Գ��뼴ʵ�ʳ���
	}
	case from_solver_realtime: {

		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		m_game.m_board.ClearSuitReplace(); //ʵʱ���㲻��Ҫͬ��ת��

//for test
		//m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig, m_solverResultLow);
		//if (m_blNotOffline)
		//	m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());
		break;
	}
	case multi_players: {
		Action a = g_multiStrategy.GetHeroAction(m_actionLine.m_multiCondition, m_actionLine.m_pokerEv); //����������������
		return a;
	}
	}//end of switch

	if (!m_blNotOffline) {
		cout << "error: ProcessingOffline" << endl;
		return ProcessingOffline();
	}

	strategy.AlignmentByexploit(m_heroCurRange, m_game.m_board.GetBoardSymbol(), m_actionLine.m_sActionSquenceRatio, m_game.GetHero()->m_positionByPresent, m_game.GetRival()->m_positionByPresent, m_game.m_gmType);

	return CalcHeroAction(strategy);
}

bool CSolution::ChangeRound(const string& sActionLine)
{
	Stacks stackPre = GetStacks();	//Parse������stack,�����㷶ΧҪ��ǰһ���ֵģ������ȱ���

	//����ActionLine
	if (!m_actionLine.Parse(sActionLine, m_game))
		m_blNotOffline = false;

	if (!m_blNotOffline)
		return false;

	if (m_game.m_round == flop) {
		if (m_game.m_players.size() > 2)
			m_strategyFrom = multi_players;
	}


	#ifdef FOR_FLOW_
	cout << "ChangeRound: " << m_game.m_sID << " " << m_actionLine.m_sActionSquence << " " << m_actionLine.m_sNodeName;
	cout << " " << "[" << GameTypeName[m_game.m_gmTypeBound.first] << "," << GameTypeName[m_game.m_gmTypeBound.second] << "]";
	cout << " " << strategyFrom2str(m_strategyFrom);
	cout << endl;
	#endif

	//����range(round�Ѿ�Ϊ�µ�,m_strategyFromΪ�ɵ�)
	if (m_game.m_round == flop && m_strategyFrom != multi_players) { //fileģʽ
		CRange rangeLow, rangeUp;
		bool blNotOffline = true;

		if (m_game.m_gmTypeBound.first != GAMETYPE_none)
			blNotOffline = blNotOffline && rangeLow.Load(m_game.m_gmTypeBound.first, m_actionLine.m_sNodeName);
		else
			cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;

		if (m_game.m_gmTypeBound.second != GAMETYPE_none)
			blNotOffline = blNotOffline && rangeUp.Load(m_game.m_gmTypeBound.second, m_actionLine.m_sNodeName);

		if (blNotOffline) {
			if (m_game.m_gmTypeBound.second == GAMETYPE_none)
				m_range = rangeLow;
			else {
				m_range.m_IPRange = CRange::getAverageRange(rangeLow.m_IPRange, rangeUp.m_IPRange, m_game.m_dSegmentRatio);
				m_range.m_OOPRange = CRange::getAverageRange(rangeLow.m_OOPRange, rangeUp.m_OOPRange, m_game.m_dSegmentRatio);
			}
		}
	}
	else if (m_strategyFrom == from_wizard) { //wizardģʽ,�������
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_actionLine.m_sNodeName, m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());
	}
	else if (m_strategyFrom == from_solver_presave) { //solverģʽ
		CRange rangeLow, rangeUp;
		bool blNotOffline = true;

		if (m_game.m_gmTypeBound.first != GAMETYPE_none) { 
			shared_ptr<Stacks> pStacksByStrategy = g_stackByStrategyConfig[m_game.m_gmTypeBound.first].GetItemByName(m_actionLine.m_sNodeName); //��ȡ���Զ�Ӧ�ĳ���
			Stacks StacksByStrategy;
			if (pStacksByStrategy == nullptr) {
				StacksByStrategy = GetStacks();	//û��ƥ��������ʵ�ʵĳ���
				cout << "error: GetStackByStrategyConfig not matched,NodeName: " << m_actionLine.m_sNodeName << endl << endl;
			}
			else
				StacksByStrategy = *pStacksByStrategy;

			blNotOffline = blNotOffline && rangeLow.Load(m_game.m_gmTypeBound.first, m_solverResultLow, m_actionLine.m_sActionSquence, stackPre, StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
		}
		else
			cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;

		if (m_game.m_gmTypeBound.second != GAMETYPE_none) {
			shared_ptr<Stacks> pStacksByStrategy = g_stackByStrategyConfig[m_game.m_gmTypeBound.second].GetItemByName(m_actionLine.m_sNodeName); //��ȡ���Զ�Ӧ�ĳ���
			Stacks StacksByStrategy;
			if (pStacksByStrategy == nullptr) {
				StacksByStrategy = GetStacks();	//û��ƥ��������ʵ�ʵĳ���
				cout << "error: GetStackByStrategyConfig not matched,NodeName: " << m_actionLine.m_sNodeName << endl << endl;
			}
			else
				StacksByStrategy = *pStacksByStrategy;

			blNotOffline = blNotOffline && rangeUp.Load(m_game.m_gmTypeBound.second, m_solverResultUp, m_actionLine.m_sActionSquence, stackPre, StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
		}

		if (blNotOffline) {
			if (m_game.m_gmTypeBound.second == GAMETYPE_none)
				m_range = rangeLow;
			else {
				m_range.m_IPRange = CRange::getAverageRange(rangeLow.m_IPRange, rangeUp.m_IPRange, m_game.m_dSegmentRatio);
				m_range.m_OOPRange = CRange::getAverageRange(rangeLow.m_OOPRange, rangeUp.m_OOPRange, m_game.m_dSegmentRatio);
			}
		}
	}
	else if (m_strategyFrom == from_solver_realtime) {
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_solverResultLow, m_actionLine.m_sActionSquence, stackPre, stackPre, m_game.m_board.GetIsomorphismSuitReplace());
	}
	else if (m_strategyFrom == from_solver_calc) { //?
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_solverResultLow, m_actionLine.m_sActionSquence, stackPre, stackPre, m_game.m_board.GetIsomorphismSuitReplace());
	}
	else if (m_strategyFrom == multi_players) { //���ﲻ��Ҫ�������

		return true;
	}


	#ifdef FOR_TEST_DUMP_ 
	DumpActionBeforeChangeRound(); //hero
	string sComment = "herorange" + m_actionLine.m_sActionSquence;
	RelativePosition rpHero = m_game.GetHero()->m_positionRelative;
	m_range.DumpRange(sComment, rpHero);
	#endif


	//���ñ���������Դ
	if (m_game.m_round == flop) {
		if (g_flopDataFromWizardConfigs[m_game.m_gmType].IsFrWizard(m_actionLine.m_sAbbrName))
			m_strategyFrom = from_wizard;
		else
			m_strategyFrom = from_solver_presave;
	}
	else if (m_game.m_round == turn) {
		if (g_turnPresaveSolverConfigs[m_game.m_gmType].IsPresaveSolver(m_actionLine.m_sAbbrName))
			m_strategyFrom = from_solver_presave;
		else
			m_strategyFrom = from_solver_calc;
	}
	else if (m_game.m_round == river)
		m_strategyFrom = from_solver_realtime;

	//����solver presave
	if (m_strategyFrom == from_solver_presave) {
		//��m_sNodeName��flop��boardתΪͬ��
		regex reg("<......>");
		string sReplace = "<" + m_game.m_board.GetIsomorphismSymbol() + ">";
		string sISONodeName = m_actionLine.m_sNodeName;
		sISONodeName = regex_replace(sISONodeName, reg, sReplace);

		if (m_game.m_gmTypeBound.first != GAMETYPE_none) {
			string sFilePath = CDataFrom::GetSolverFilePath(m_game.m_gmTypeBound.first, sISONodeName);
			if (!g_solver.LoadSolverFile(sFilePath, m_solverResultLow)) {
				m_blNotOffline = false;
				return false;
			}
		}

		if (m_game.m_gmTypeBound.second != GAMETYPE_none) {
			string sFilePath = CDataFrom::GetSolverFilePath(m_game.m_gmTypeBound.second, sISONodeName);
			if (!g_solver.LoadSolverFile(sFilePath, m_solverResultUp)) {
				m_blNotOffline = false;
				return false;
			}
		}
	}
	else if (m_strategyFrom == from_solver_calc) { //����ʱ����
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		solverConfig.m_pRTTreeConfigItem = g_strategyTreeConfigs[m_game.m_gmType].GetRTTreeConfig(GetStacks());
		m_game.m_board.ClearSuitReplace(); //ʵʱ���㲻��Ҫͬ��ת��

		//for test
		//m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig, m_solverResultLow, calc_async);
	}

	m_heroCurRange = m_game.GetHero()->m_positionRelative == OOP ? m_range.m_OOPRange : m_range.m_IPRange;

	return true;
}

//�������fBetSize��0��fBetSizeByPot==0,��ֱ������fBetSize
//�ͻ�����Ҫ�����жϺʹ���
//������size��Ҫ��д��ƽ̨֧�ֱ�����Ĭ�ϱ�����ť�������ñ�����������size������תΪsize����strategy::CalcBetSize()
//��size>hero��ǰʣ�����ʱתΪallin���ɿͻ����жϣ�
Action CSolution::CalcHeroAction(const CStrategy& strategy)
{
	Action retAction{ fold,0,0,0 };
	string sHand = m_game.GetHero()->m_hands.m_sSymbol;

	vector<pair<Action, double>> handStrategy;	//action,ratio
	vector<double> randBounds;	//�������Χ

	bool blHandFound = true;
	for (int i = 0; i < strategy.m_strategy.size(); i++) {
		Action a = strategy.m_strategy[i]->m_action;
		double dRatio = 0;

		auto p = strategy.m_strategy[i]->m_strategyData.find(sHand);
		if (p != strategy.m_strategy[i]->m_strategyData.end()) {
			dRatio = p->second;
			handStrategy.push_back(make_pair(a, dRatio));
		}
		else
			blHandFound = false;
	}

	//hero�������޶�Ӧ���ԣ�ֱ�ӵ�fold����
	if (!blHandFound) {
		cout << "error: CalcHeroAction hand not found in strategy" << endl;
		return retAction;
	}

	//�����Ч�ԣ����������Χ
	double dRatioSum = 0;
	for (auto it : handStrategy) {
		randBounds.push_back(dRatioSum);
		dRatioSum += it.second;
	}
	if (fabs(dRatioSum - 1) > 0.01)
		cout << "error: CalcHeroAction ratio sum error > 0.01" << endl;

	//�������
	
	double dRand = getRandonNum(0, 1000) / 1000.0;//0-1
	int lowBound = 0;
	for (int i = 0; i < randBounds.size(); i++) {
		if (dRand >= randBounds[i])
			lowBound = i;
		else
			break;
	}
	if(!handStrategy.empty())
		retAction = handStrategy[lowBound].first;

	//���betsize�ĺϷ���(����Ϊ������ע��������preflop���ڴ��ڶ�����ע�ر�������»ᷢ��)
	if(retAction.actionType == raise){
		double dRivalMaxRaiseSize = 0;
		for (auto player : m_game.m_players) {
			if (!player.second.m_blIsHero) {
				if (player.second.m_lastAction.actionType == raise)
					if (player.second.m_lastAction.fBetSize > dRivalMaxRaiseSize)
						dRivalMaxRaiseSize = player.second.m_lastAction.fBetSize;
			}
		}

		if (retAction.fBetSize < dRivalMaxRaiseSize * 2) {
			cout << "warning: betsize < 2 * rival betsize" << endl;
			retAction.fBetSize = (float)(dRivalMaxRaiseSize * 2.1); //(��0.1Ϊ��һ�����)
			retAction.fBetSizeByPot = 0; //ֻ����fBetSize
		}
	}
	
	m_heroLastStrategy = strategy.GetStrategy(); //����exo;oi


	#ifdef FOR_FLOW_
	string sCandicate;
	for (auto it : handStrategy) {
		if (!sCandicate.empty())
			sCandicate += ",";
		sCandicate = sCandicate + action2symbol(it.first) + ":" + "<" + double2String(it.second * 100, 1) + "%>";
	}
	cout << "CalcHeroAction:" << "[" << sCandicate << "]" << " " << " rand:" << double2String(dRand, 2) << " select:" << action2symbol(retAction) << endl;
	#endif

	return retAction;
}

//��ʽ��GameID=353545;BBSize=0.1;Pot=1.5;Plays=[UTG]98,[HJ]67,[CO]34,[BTN]120,[SB]78,[BB]286;Hero=[UTG];Hands=<KsKd>;//λ�������ȱϯ��û��
void CSolution::InitGame(const string& sInitGame)
{
	regex reg;
	smatch m;
	Position psHero;
	string sHands;

	//��ʼ��GameID
	reg = R"(GameID=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_sID = m[1];

	//��ʼ��GameType
	reg = R"(GameType=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_gmType = CGame::GetGameTypeBySymbol(m[1]);

	//��ʼ��BBSize
	reg = R"(BBSize=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_dBBSize = stod(m[1]);

	//��ʼ��pot
	reg = R"(Pot=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_dPot = stod(m[1]);

	//��ʼ��hero
	reg = R"(Hero=\[([^;]*)\])";
	if (regex_search(sInitGame, m, reg))
		psHero = CGame::GetPositionBySymbol(m[1]);

	//��ʼ��hero Hands
	reg = R"(Hands=\<([^;]*)\>)";
	if (regex_search(sInitGame, m, reg))
		sHands = m[1];

	//��ʼ��players
	string sPlayers;
	reg = R"(Plays=([^;]*))";
	if (regex_search(sInitGame, m, reg)) {
		sPlayers = m[1];

		regex sep(R"(\s*,\s*)");
		sregex_token_iterator p(sPlayers.cbegin(), sPlayers.cend(), sep, -1);
		sregex_token_iterator e;
		regex regPosition(R"(\[(.*)\])");
		regex regEstack(R"(\](.*))");
		for (; p != e; ++p) {
			string s = p->str();
			if (regex_search(s, m, regPosition)) {
				Position ps = CGame::GetPositionBySymbol(m[1]);
				regex_search(s, m, regEstack);
				double dStack = stod(m[1]);
				CPlayer player(ps, dStack);
				m_game.m_players[ps] = player;
			}
		}

		//����hero��hands
		m_game.m_players[psHero].m_blIsHero = true;
		m_game.m_players[psHero].m_hands.SetHands(sHands);

		//����m_positionByPresent
		auto posSB = m_game.m_players.find(SB);
		for (auto pos = m_game.m_players.begin(); pos->first != SB; ++pos) {
			auto dif = distance(pos, posSB);
			vector<Position> p{ UTG, HJ, CO, BTN, SB, BB };
			auto pos1 = p.cend() - 2 - dif;
			pos->second.m_positionByPresent = *pos1;
		}
	}


	#ifdef FOR_FLOW_
	cout << "flow InitGame: " << sInitGame << endl;
	#endif


#ifdef DEBUG_
	cout << "InitGame:--------------------------------------------------" << endl;
	cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "Pot:" << double2String(m_game.m_dPot, 2) << "\t" << endl;
	cout << "Players:";
	for (auto it : m_game.m_players) {
		cout << PositionSymble[it.first];
		cout << "[" << PositionSymble[it.second.m_positionByPresent] << "]";
		if (it.second.m_blIsHero)
			cout << "(hero)";
		cout << double2String(it.second.m_dEStack, 2);
		cout << "--";
	}
	cout << endl << endl;
#endif
}

void CSolution::HeroHands(const string& sHands)
{
	m_game.GetHero()->m_hands.SetHands(sHands);
}

Stacks CSolution::GetStacks()
{
	Stacks stacks;
	stacks.dPot = m_game.m_dPot;

	auto it = min_element(m_game.m_players.begin(), m_game.m_players.end(), [](auto elem1, auto elem2) {return elem1.second.m_dEStack < elem2.second.m_dEStack; });
	stacks.dEStack = it->second.m_dEStack;

	return stacks;
}

Action CSolution::ProcessingOffline()
{
	Action a;

	if (m_game.m_round == preflop) {
		CStrategy strategy;
		strategy.SpecialProcessing(m_game.m_gmType, "default(5bet)");
		a = CalcHeroAction(strategy);
	}
	else {
		a = g_multiStrategy.GetHeroAction(m_actionLine.m_multiCondition, m_actionLine.m_pokerEv);
	}

	return a;
}

bool CSolution::IsMultiPlayers()
{
	return m_strategyFrom == multi_players ? true : false;
}

string CSolution::getDataFromString(const StrategyFrom fr)
{
	//typedef enum { from_wizard, from_solver_presave, from_solver_realtime, from_solver_calc, multi_players }StrategyFrom;
	switch (fr) {
	case from_wizard:
		return "from_wizard";
	case from_solver_presave:
		return "from_solver_presave";
	case from_solver_realtime:
		return "from_solver_realtime";
	case from_solver_calc:
		return "from_solver_calc";
	case multi_players:
		return "multi_players";
	default:
		return "";
	}
}

//������������heroֻ����һ��
string CSolution::DumpSelAction(const std::string& sActionLine)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\dump\\";
	string sCommandsPath = sConfigFolder + "commands.txt";
	time_t t = time(nullptr);
	t += rand();

	string sAction;

	smatch m;
	regex reg(R"(\<(.*)\>)");
	if (!regex_search(sActionLine, m, reg)) {	
		regex sep(R"(\s?,\s?)");
		vector<pair<Position, Action>> posActions;
		sregex_token_iterator p(sActionLine.cbegin(), sActionLine.cend(), sep, -1);
		sregex_token_iterator e;
		for (; p != e; ++p) {
			if (!p->str().empty())
				posActions.push_back(CActionLine::ToPosActionPair(p->str()));	//"-"���Ϊnone,nonepos
		}

		for (auto it : posActions) {
			auto p = m_game.m_players.find(it.first);
			if (p != m_game.m_players.end()) {
				if (p->second.m_blIsHero) {
					sAction = CActionLine::ToActionSymbol(it.second, true);

					ofstream ofCommands;
					ofCommands.open(sCommandsPath, ofstream::out | ios_base::app);
					if (ofCommands.is_open()) {
						//��ʽ��87969;	SelA; comment;	R12
						string sLine = to_string(t) + "; " + "SelA; " + sAction + "; " + sAction;
						ofCommands << sLine << endl;
					}
					ofCommands.close();

				}
			}
		}
	}

	return sAction;

}

void CSolution::DumpActionBeforeChangeRound(const bool blDumpHero)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\dump\\";
	string sCommandsPath = sConfigFolder + "commands.txt";
	time_t t = time(nullptr);
	t += rand();

	for (auto play : m_game.m_players) {
		if (play.second.m_blIsHero) {
			Action heroLastAction = play.second.m_lastAction;
			if (heroLastAction.actionType != none) {
				string sAction = blDumpHero == true ? CActionLine::ToActionSymbol(play.second.m_lastAction, true) : "";

				ofstream ofCommands;
				ofCommands.open(sCommandsPath, ofstream::out | ios_base::app);
				if (ofCommands.is_open()) {
					//��ʽ��87969;	SelA_BeforeCR; comment;	R12
					string sLine = to_string(t) + "; " + "SelA_BeforeCR; " + sAction + "; " + sAction + ";";
					ofCommands << sLine << endl;
				}
				ofCommands.close();
			}
			break;
		}
	}
}

void CSolution::updataHeroCurRange(const string& sSelA)
{
	Action a = CActionLine::strToAction(sSelA);

	int nIdx = -1;
	double dlMin = 10000;
	for (int i = 0; i < m_heroLastStrategy.size(); i++) {
		Action& b = m_heroLastStrategy[i]->m_action;
		if (a.actionType == b.actionType && a.actionType != raise) {
			nIdx = i;
			break;
		}

		if (a.actionType == raise && b.actionType == raise) {
			if (fabs(a.fBetSize - b.fBetSize) < dlMin) {
				dlMin = fabs(a.fBetSize - b.fBetSize);
				nIdx = i;
			}
		}
	}

	if (nIdx == -1) {
		cout << "error: updataHeroCurRange nIdx == -1" << endl;
		return;
	}

	StrategyData& strategyData = m_heroLastStrategy[nIdx]->m_strategyData;

	for (auto p = m_heroCurRange.begin(); p != m_heroCurRange.end(); p++) {
		auto pStrategy = strategyData.find(p->first);
		if (pStrategy != strategyData.end())
			p->second *= pStrategy->second;
		else
			p->second = 0;
	}
}




