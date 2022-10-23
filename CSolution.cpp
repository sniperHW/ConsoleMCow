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

using namespace std;
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; 
extern CSolver g_solver;
extern map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs;
extern map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs;
extern map<GameType, CStackByStrategyConfig> g_stackByStrategyConfig;

CSolution::CSolution()
{
	m_strategyFrom = from_wizard;
	m_blNotOffline = true;
}

//
Action CSolution::HeroAction(const string& sActionLine)
{
	CStrategy strategy;

	if (!m_actionLine.Parse(sActionLine, m_game)) {
		m_blNotOffline = false;
		return ProcessingOffline();
	}

	switch (m_strategyFrom)
	{
	case from_wizard: {

#ifdef DEBUG_
		cout << "HeroAction Load strategy from_wizard:--------------------------------------------" << endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "ActionSquence:" << m_actionLine.m_sActionSquence << "\t" << "Stacks:" << double2String(GetStacks().dPot, 2) << "," << double2String(GetStacks().dEStack, 2) << "\t" << "oopx:" << CActionLine::getOOPXString(m_game.m_oopx) << "\t" << "IsoBoard:" << m_game.m_board.GetIsomorphismSymbol() << endl << endl;
#endif 

		m_blNotOffline = strategy.Load(m_game.m_gmType, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_oopx, m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());

#ifdef FOR_TEST_DUMP_
		string sComment = "from_wizard-" + m_actionLine.m_sActionSquence;
		strategy.DumpStrategy(sComment, NULL);
#endif

		break;
	}
	case from_solver_presave: {
		if (m_actionLine.m_sActionSquence.find('$') != string::npos) //�������������-Aʱ��Ӹñ�ǣ���solverģʽ����Ҫ����
			m_actionLine.m_sActionSquence.pop_back();

		shared_ptr<Stacks> pStacksByStrategy = g_stackByStrategyConfig[m_game.m_gmType].GetItemByName(m_actionLine.m_sNodeName); //��ȡ���Զ�Ӧ�ĳ���
		Stacks StacksByStrategy;
		if (pStacksByStrategy == nullptr) {
			StacksByStrategy = GetStacks();	//û��ƥ��������ʵ�ʵĳ���
			cout << "error: GetStackByStrategyConfig not matched,NodeName: " << m_actionLine.m_sNodeName << endl << endl;
		}
		else
			StacksByStrategy = *pStacksByStrategy;

#ifdef DEBUG_
		cout << "HeroAction Load strategy from_solver_presave:----------------------------------" << endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "ActionSquence:" << m_actionLine.m_sActionSquence << "\t" << "Stacks:" << double2String(GetStacks().dPot, 2) << "," << double2String(GetStacks().dEStack, 2) << "\t" << "StacksByStrategy:" << double2String(StacksByStrategy.dPot, 2) << "," << double2String(StacksByStrategy.dEStack, 2) << "\t" << "oopx:" << CActionLine::getOOPXString(m_game.m_oopx) << endl << endl;
#endif 

//for test
//m_blNotOffline = true;
		m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());

#ifdef FOR_TEST_DUMP_
		string sComment = "from_solver_presave-" + m_actionLine.m_sActionSquence;
		strategy.DumpStrategy(sComment, NULL);
#endif

		break;
	}
	case from_solver_calc: {

#ifdef DEBUG_
		cout << "HeroAction Load strategy from_solver_calc:--------------------------------------"<< endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "ActionSquence:" << m_actionLine.m_sActionSquence << "\t" << "Stacks:" << double2String(GetStacks().dPot, 2) << "," << double2String(GetStacks().dEStack, 2) << "\t" << "oopx:" << CActionLine::getOOPXString(m_game.m_oopx) << endl << endl;
#endif 

//��鲢�ȴ�������ɣ���Ҫ��Ӵ��룩
//for test
		//m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());	//���Գ��뼴ʵ�ʳ���
	}
	case from_solver_realtime: {

#ifdef DEBUG_
		cout << "HeroAction Load strategy from_solver_realtime:----------------------------------" << endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "ActionSquence:" << m_actionLine.m_sActionSquence << "\t" << "Stacks:" << double2String(GetStacks().dPot, 2) << "," << double2String(GetStacks().dEStack, 2) << "\t" << "oopx:" << CActionLine::getOOPXString(m_game.m_oopx) << endl << endl;
#endif 


//for test
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		m_game.m_board.ClearSuitReplace(); //ʵʱ���㲻��Ҫͬ��ת��
		//m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig, m_solverResult);
		//if (m_blNotOffline)
		//	m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());

#ifdef FOR_TEST_DUMP_
		string sComment = "from_solver_realtime-" + m_actionLine.m_sActionSquence;
		strategy.DumpStrategy(sComment, NULL);
#endif

		break;
	}
	case multi_players: { //flop�����ģʽ,����ʵ��

#ifdef DEBUG_
		cout << "HeroAction multi_players:---------------------------------------------------" << endl << endl;
#endif 

		break;
	}
	}//end of switch

//��������ʵ��
	if (!m_blNotOffline) {
		cout << "error: ProcessingOffline" << endl;

		return ProcessingOffline();
	}

	strategy.AlignmentByBetsize();
	strategy.AlignmentByStackDepth();
	strategy.AlignmentByexploit();

	return CalcHeroAction(strategy);
}

//
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

#ifdef FOR_TEST_DUMP_
	string sComment = "";
	RelativePosition heroRPosition;
	for (auto it : m_game.m_players) {
		if (it.second.m_blIsHero)
			heroRPosition = it.second.m_positionRelative;
	}
#endif

	//����range(round�Ѿ�Ϊ�µ�,m_strategyFromΪ�ɵ�)
	if (m_game.m_round == flop) { //fileģʽ

#ifdef DEBUG_
		cout << "ChangeRound Load range from_file:----------------------------------------------" << endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "NodeName:" << m_actionLine.m_sNodeName << "\t" << "Board:" << m_game.m_board.GetBoardSymbol() << endl << endl;
#endif 

//for test
		//m_blNotOffline = m_range.Load(m_game.m_gmType, m_actionLine.m_sNodeName, m_game.m_board.GetBoardSymbol());

#ifdef FOR_TEST_DUMP_
		sComment = "from_file-" + m_actionLine.m_sNodeName;
#endif

	}
	else if (m_strategyFrom == from_wizard) { //wizardģʽ

#ifdef DEBUG_
		cout << "ChangeRound Load range from_wizard:--------------------------------------------" << endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "NodeName:" << m_actionLine.m_sNodeName << "\t" << "Board:" << m_game.m_board.GetBoardSymbol() << "\t" << "IsoBoard:" << m_game.m_board.GetIsomorphismSymbol() << endl << endl;
#endif 


//for test
		//m_blNotOffline = m_range.Load(m_game.m_gmType, m_actionLine.m_sNodeName, m_game.m_board.GetBoardSymbol(), m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());

#ifdef FOR_TEST_DUMP_
		sComment = "from_wizard-" + m_actionLine.m_sNodeName;
#endif

	}
	else if (m_strategyFrom == from_solver_presave || m_strategyFrom == from_solver_realtime) { //solverģʽ
		Stacks StacksByStrategy = stackPre;
		if (m_strategyFrom == from_solver_presave) {
			shared_ptr<Stacks> pStacksByStrategy = g_stackByStrategyConfig[m_game.m_gmType].GetItemByName(m_actionLine.m_sNodeName); //��ȡ���Զ�Ӧ�ĳ���
			if (pStacksByStrategy == nullptr) 
				cout << "error: GetStackByStrategyConfig not matched,NodeName: " << m_actionLine.m_sNodeName << endl << endl;
			else
				StacksByStrategy = *pStacksByStrategy;
		}

#ifdef DEBUG_
		cout << "ChangeRound Load range from_solver_presave:-------------------------------------" << endl;
		cout << "gmType:" << GameTypeName[m_game.m_gmType] << "\t" << "ActionSquence:" << m_actionLine.m_sActionSquence << "\t" << "Board:" << m_game.m_board.GetBoardSymbol() << "\t" << "Stacks:" << double2String(GetStacks().dPot, 2) << "," << double2String(GetStacks().dEStack, 2) << "\t" << "StacksByStrategy:" << double2String(StacksByStrategy.dPot, 2) << "," << double2String(StacksByStrategy.dEStack, 2) << endl << endl;
#endif 

//for test
		//m_blNotOffline = m_range.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, stackPre, StacksByStrategy, m_game.m_board.GetBoardSymbol(), m_game.m_board.GetIsomorphismSuitReplace());

#ifdef FOR_TEST_DUMP_
		sComment = "from_solver-" + m_actionLine.m_sActionSquence;
#endif

	}
	else if (m_strategyFrom == multi_players) {

#ifdef DEBUG_
		cout << "ChangeRound multi_players:----------------------------------------------------" << endl << endl;
#endif 

		return true;
	}

#ifdef FOR_TEST_DUMP_
	if (sComment.size() > 0) 
		m_range.DumpRange(sComment, heroRPosition);
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
	else if(m_game.m_round == river)
		m_strategyFrom = from_solver_realtime;

#ifdef DEBUG_
	cout << "ChangeRound set data from:" << getDataFromString(m_strategyFrom) << endl << endl;
#endif 

	//����solver presave
	if (m_strategyFrom == from_solver_presave) {	
		//��m_sNodeName��flop��boardתΪͬ��
		regex reg("<......>");
		string sReplace = "<" + m_game.m_board.GetIsomorphismSymbol() + ">";
		string sISONodeName = m_actionLine.m_sNodeName;
		sISONodeName = regex_replace(sISONodeName, reg, sReplace);
		string sFilePath = CDataFrom::GetSolverFilePath(m_game.m_gmType, sISONodeName);

#ifdef DEBUG_
		if (IsoFlops.find(sISONodeName) == IsoFlops.end())
			cout << "error: not in ISO set,sISONodeName:" << sISONodeName << endl;
		cout << "Load solver presave:" << sFilePath << endl << endl;
#endif 

		ifstream ifs;
		ifs.open(sFilePath);
		if (ifs.is_open()) {
			JSONCPP_STRING errs;
			Json::CharReaderBuilder builder;
			m_solverResult.clear();
			if (!parseFromStream(builder, ifs, &m_solverResult, &errs)) {
				ifs.close();
				m_blNotOffline = false;
				return false;
			}
			ifs.close();
		}
		else {
			m_blNotOffline = false;
			return false;
		}
	}
	//����ʱ����
	else if (m_strategyFrom == from_solver_calc) {
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		solverConfig.m_pRTTreeConfigItem = g_strategyTreeConfigs[m_game.m_gmType].GetRTTreeConfig(GetStacks());
		m_game.m_board.ClearSuitReplace(); //ʵʱ���㲻��Ҫͬ��ת��

#ifdef DEBUG_
		cout << "ToSolve"  << endl << endl;
#endif 

		m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig, m_solverResult, calc_async);
	}

	return true;
}

Action CSolution::CalcHeroAction(const CStrategy& strategy)
{
	//������preflop��Ҫ���betsize�ĺϷ���
	return Action{};
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

		regex sep(R"(\s?,\s?)");
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
	//default(5betallin_L)
	return Action{};
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


