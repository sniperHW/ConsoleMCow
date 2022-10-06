//#include "pch.h"
#include "CSolution.h"
#include <regex>
#include <iostream>
#include "CStrategyTreeConfig.h"
#include "CSolver.h"
#include "CFlopDataFromWizardConfig.h"
#include "CTurnPresaveSolverConfig.h"
#include "CDataFrom.h"
#include <fstream>

using namespace std;
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; 
extern CSolver g_solver;
extern map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs;
extern map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs;

CSolution::CSolution()
{
	m_strategyFrom = from_wizard;
	m_blNotOffline = true;
}

//未测试
Action CSolution::HeroAction(const string& sActionLine)
{
	CStrategy strategy;

	if (!m_actionLine.Parse(sActionLine, m_game)) {
		m_blNotOffline = false;
		return ProcessingOffline();
	}
//for test
	printTest();

	switch (m_strategyFrom)
	{
	case from_wizard: {
		m_blNotOffline = strategy.Load(m_game.m_gmType, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_oopx, m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());
		break;
	}
	case from_solver_presave: {
		if (m_actionLine.m_sActionSquence.find('$')) //动作序列最后是-A时会加该标记，但solver模式不需要处理
			m_actionLine.m_sActionSquence.pop_back();

		m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), m_StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
		break;
	}
	case from_solver_calc: {
//检查并等待运算完成（需要添加代码）
		m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), m_StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
	}
	case from_solver_realtime: {
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		//solverConfig.m_strategyTree = g_strategyTreeConfigs[m_game.m_gmType].GetConfigItem(m_actionLine.m_sAbbrName);
		m_StacksByStrategy = solverConfig.m_stacks; //策略对应筹码为实际筹码
		m_game.m_board.ClearSuitReplace(); //实时计算不需要同构转换
		m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig, m_solverResult);
		if (m_blNotOffline)
			m_blNotOffline = strategy.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), m_StacksByStrategy, m_game.m_board.GetIsomorphismSuitReplace());
		break;
	}
	case multi_players: { //flop后多人模式,留待实现

		break;
	}
	}//end of switch

//以下留待实现
	if (!m_blNotOffline)
		return ProcessingOffline();

	strategy.AlignmentByBetsize();
	strategy.AlignmentByStackDepth();
	strategy.AlignmentByexploit();

	return CalcHeroAction(strategy);
}

//未测试
bool CSolution::ChangeRound(const string& sActionLine)
{
	//解析ActionLine
	if (!m_actionLine.Parse(sActionLine, m_game))
		m_blNotOffline = false;


//for test
printTest();


	if (!m_blNotOffline)
		return false;

	if (m_game.m_round == flop) {
		if (m_game.m_players.size() > 2)
			m_strategyFrom = multi_players;
	}

	//加载range(round已经为新的,m_strategyFrom为旧的)
	if (m_game.m_round == flop) { //file模式
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_actionLine.m_sNodeName, m_game.m_board.GetBoardSymbol());
	}
	else if (m_strategyFrom == from_wizard) { //wizard模式
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_actionLine.m_sNodeName, m_game.m_board.GetBoardSymbol(), m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());
	}
	else if (m_strategyFrom == from_solver_presave || m_strategyFrom == from_solver_realtime) { //solver模式
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_solverResult, m_actionLine.m_sActionSquence, GetStacks(), m_StacksByStrategy, m_game.m_board.GetBoardSymbol(), m_game.m_board.GetIsomorphismSuitReplace());
	}
	else if (m_strategyFrom == multi_players)
		return true;

	//设置本街数据来源
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

	//加载solver presave
	if (m_strategyFrom == from_solver_presave) {	
		string sFilePath = CDataFrom::GetSolverFilePath(m_game.m_gmType, m_actionLine.m_sNodeName);
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
	//换街时解算
	else if (m_strategyFrom == from_solver_calc) {
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		solverConfig.m_pRTTreeConfigItem = g_strategyTreeConfigs[m_game.m_gmType].GetRTTreeConfig(GetStacks());
		m_StacksByStrategy = solverConfig.m_stacks; //策略对应筹码为实际筹码
		m_game.m_board.ClearSuitReplace(); //实时计算不需要同构转换
		m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig, m_solverResult, calc_async);
	}

	return true;
}

Action CSolution::CalcHeroAction(const CStrategy& strategy)
{
	//备忘：preflop需要检查betsize的合法性
	return Action{};
}

//格式：GameID=353545;BBSize=0.1;Pot=1.5;Plays=[UTG]98,[HJ]67,[CO]34,[BTN]120,[SB]78,[BB]286;Hero=[UTG];Hands=<KsKd>;//位置上玩家缺席则没有
void CSolution::InitGame(const string& sInitGame)
{
	regex reg;
	smatch m;
	Position psHero;
	string sHands;

	//初始化GameID
	reg = R"(GameID=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_sID = m[1];

	//初始化GameType
	reg = R"(GameType=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_gmType = CGame::GetGameTypeBySymbol(m[1]);

	//初始化BBSize
	reg = R"(BBSize=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_dBBSize = stod(m[1]);

	//初始化pot
	reg = R"(Pot=([^;]*))";
	if (regex_search(sInitGame, m, reg))
		m_game.m_dPot = stod(m[1]);

	//初始化hero
	reg = R"(Hero=\[([^;]*)\])";
	if (regex_search(sInitGame, m, reg))
		psHero = CGame::GetPositionBySymbol(m[1]);

	//初始化hero Hands
	reg = R"(Hands=\<([^;]*)\>)";
	if (regex_search(sInitGame, m, reg))
		sHands = m[1];

	//初始化players
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

		//设置hero和hands
		m_game.m_players[psHero].m_blIsHero = true;
		m_game.m_players[psHero].m_hands.SetHands(sHands);

		//计算m_positionByPresent
		auto posSB = m_game.m_players.find(SB);
		for (auto pos = m_game.m_players.begin(); pos->first != SB; ++pos) {
			auto dif = distance(pos, posSB);
			vector<Position> p{ UTG, HJ, CO, BTN, SB, BB };
			auto pos1 = p.cend() - 2 - dif;
			pos->second.m_positionByPresent = *pos1;
		}
	}
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


void CSolution::printTest()
{
	cout << "ActionSquence:" << m_actionLine.m_sActionSquence << endl;
	cout << "AbbrName:" << m_actionLine.m_sAbbrName << endl;
	cout << "NodeName:" << m_actionLine.m_sNodeName << endl;
	if(m_game.m_round == preflop)
		cout << "PreflopRowActionSquence:" << m_actionLine.GetPreflopRowActionSquence() << endl;
	
	cout << endl;
	cout << endl;
}