// ConsoleMCow.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "globledef.h"
#include "CStrategyNodeConfig.h"
#include "CRangeNodeConfig.h"
#include "CStrategyTreeConfig.h"
#include "CFlopDataFromWizardConfig.h"
#include "CStrategy.h"
#include <fstream>
#include "CActionMatchConfig.h"
#include "util.h"
#include "CStackByStrategyConfig.h"
#include "CTurnPresaveSolverConfig.h"
#include "CSolver.h"
#include "CSolution.h"
#include "CStrategyNodeConfig.h"
#include "CFlopDataFromWizardConfig.h"
#include "CDataFrom.h"
#include "CBoard.h"

using namespace std;

//全局变量
map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;	//范围节点配置
map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
map<GameType, CActionMatchConfig> g_actionMatchConfigs; //preflop节点匹配表
map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs; //flop策略数据为wizard匹配表
map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs; //flop策略数据为solver匹配表
map<GameType, CStackByStrategyConfig> g_stackByStrategyConfig; //预存solver策略对应的筹码
CSolver g_solver;

bool LoadConfigs(GameType gmType)
{

	CStrategyNodeConfig StrategyNodeConfig;
	if (!StrategyNodeConfig.Init(gmType))
		return false;

	CRangeNodeConfig RangeNodeConfig;
	if (!RangeNodeConfig.Init(gmType))
		return false;

	CStrategyTreeConfig StrategyTreeConfig;
	if (!StrategyTreeConfig.Init(gmType))
		return false;

	CActionMatchConfig ActionMatchConfig;
	if (!ActionMatchConfig.Init(gmType))
		return false;


	CFlopDataFromWizardConfig FlopDataFromWizardConfig;
	if (!FlopDataFromWizardConfig.Init(gmType))
		return false;


	CTurnPresaveSolverConfig TurnPresaveSolverConfig;
	if (!TurnPresaveSolverConfig.Init(gmType))
		return false;


	CStackByStrategyConfig StackByStrategyConfig;
	if (!StackByStrategyConfig.Init(gmType))
		return false;

	return true;
}

extern void loadFileAsLine(const string& path,vector<string> &lines);

int main()
{
	/*if (!LoadConfigs(Max6_NL50_SD100)) {
		cout << "Load config error!" << endl;
		return 0;
	}*/

	//CSolution solution;
	//string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]67.8,[CO]34,[BTN]120,[SB]78,[BB]286;Hero=[BTN];Hands=<KsKd>;";
	//string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]100,[CO]94.6,[BTN]120,[SB]99,[BB]286;Hero=[CO];Hands=<KsKd>;";
	//string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]100,[CO]94.6,[BTN]119,[SB]99,[BB]286;Hero=[BB];Hands=<KsKd>;";
	//string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]100,[CO]94.6,[BTN]119,[SB]99,[BB]286;Hero=[BTN];Hands=<KsKd>;";
	//solution.InitGame(sInitGame);


	//例1：hero:CO,BTN缺席,preflop    第一轮：[UTG]F,[HJ]R2  ;   第二轮：[CO]R5,[SB]C,[BB]F,-,[HJ]R21 ;  第三轮： [CO]C,[SB]F
	//例2：hero:CO,flop    第一轮：[UTG]R15 ;   第二轮：[CO]R35,[UTG]R65
	//例3：hero:CO,rivals: UTG,SB,flop    第一轮：[UTG]R15 ;   第二轮：[CO]C,[SB]R30,[UTG]R60
	//例4：limp相关


	//solution.HeroAction("");
	//solution.HeroAction("[UTG]F,[HJ]R2");
	//solution.HeroAction("[CO]R5,[SB]C,[BB]F,-,[HJ]R21");

	//solution.HeroAction("[UTG]C,[HJ]C");

	//CActionMatchConfig actionMatchConfig;
	//actionMatchConfig.Init(Max6_NL50_SD100);
	//g_actionMatchConfigs[Max6_NL50_SD100] = actionMatchConfig;

	//solution.HeroAction("[UTG]C,[HJ]C");
	// 
	//solution.HeroAction("[UTG]R2");
	//solution.HeroAction("[HJ]C,[CO]C");
	//solution.HeroAction("[UTG]R2,[HJ]C,[CO]C");

	//solution.HeroAction("[UTG]F,[HJ]C,[CO]C,[BTN]F,[SB]C");
	//solution.HeroAction("[UTG]F,[HJ]C,[CO]C,[BTN]R2,[SB]F");
	//solution.HeroAction("[BB]C,[HJ]R12.5,[CO]R35,[BTN]F");

//	solution.HeroAction("[UTG]R2,[HJ]C,[CO]F");
//	solution.printTest();
//solution.HeroAction("[BTN]R7.5,[SB]F,[BB]F,-,[UTG]R21,[HJ]F");
//	solution.printTest();

/*
	//test:RCR - [RA]		hj,btn(hero), bb
	string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]100,[CO]94.6,[BTN]119,[SB]99,[BB]286;Hero=[BTN];Hands=<KsKd>;";
	solution.InitGame(sInitGame);
	solution.HeroAction("[UTG]F,[HJ]R2,[CO]F");
	solution.printTest();
	solution.HeroAction("[BTN]C,[SB]F,[BB]R7,-,[HJ]R21");
	solution.printTest();
*/



//	solution.HeroAction("[BTN]R7,[SB]F,[BB]F,-,[HJ]R21");
//	solution.HeroAction("[BTN]R35,-,[HJ]R55");
//	solution.ChangeRound("[BTN]C<KsQd7h>pot=15;EStack=[HJ]95.5,[BTN]67;");

/*
	//test "$":RR-RR-[RA],[hero]_vs[rivals]_6betallin
	string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]100,[CO]94.6,[BTN]119,[SB]99,[BB]286;Hero=[BTN];Hands=<KsKd>;";
	solution.InitGame(sInitGame);
	solution.HeroAction("[UTG]F,[HJ]R2,[CO]F");
	solution.ChangeRound("[BTN]C,[SB]F,[BB]F<KsQd7h>pot=5.5;EStack=[HJ]98,[BTN]98;");
	//solution.HeroAction("[HJ]R1.8");
	//solution.HeroAction("[BTN]R6.5,[HJ]R22.7");
	//solution.ChangeRound("[BTN]C<8h>pot=54.6;EStack=[HJ]73.5,[BTN]73;");
	//solution.HeroAction("[HJ]X");

	solution.HeroAction("[HJ]X");
	solution.ChangeRound("[BTN]X<8h>pot=54.6;EStack=[HJ]73.5,[BTN]73;");
*/
/*
	CStrategyNodeConfig StrategyNodeConfig;
	StrategyNodeConfig.Init(Max6_NL50_SD100);

	string sNodeName = "SB_vsBB_limpbet<KsQs7d>X-R50-R150-R100-R50-A";
	string s1 = StrategyNodeConfig.GetReplace(sNodeName);
	string s2 = StrategyNodeConfig.GetSpecial(sNodeName);
	cout << "result:\t" << s1 << "\t" << s2<<endl;
*/

	//CRangeNodeConfig RangeNodeConfig;
	//RangeNodeConfig.Init(Max6_NL50_SD100);

//	CFlopDataFromWizardConfig FlopDataFromWizardConfig;
//	FlopDataFromWizardConfig.Init(Max6_NL50_SD100);
//	cout << FlopDataFromWizardConfig.IsFrWizard("[hero]_vs[rivals]_srp");

/*
	string sInitGame = "GameID=353545;GameType=Max6_NL50_SD100;BBSize=0.1;Pot=1.5;Plays=[UTG]98.5,[HJ]100,[CO]94.6,[BTN]119,[SB]99,[BB]286;Hero=[BTN];Hands=<KsKd>;";
	solution.InitGame(sInitGame);
	solution.HeroAction("[UTG]F,[HJ]R2,[CO]F");
	solution.ChangeRound("[BTN]C,[SB]F,[BB]F<KsQd7h>pot=5.5;EStack=[HJ]98,[BTN]98;");
	solution.HeroAction("[HJ]X");
	solution.ChangeRound("[BTN]X<8h>pot=54.6;EStack=[HJ]73.5,[BTN]73;");
*/

/*
	CTurnPresaveSolverConfig TurnPresaveSolverConfig;
	TurnPresaveSolverConfig.Init(Max6_NL50_SD100);
	cout << TurnPresaveSolverConfig.IsPresaveSolver("[hero]_vs[rivals]_srp<>X-R33");
*/

/*
	CStackByStrategyConfig StackByStrategyConfig;
	StackByStrategyConfig.Init(Max6_NL50_SD100);
	auto p = StackByStrategyConfig.GetItemByName("BTN_vsHJ_srp<>X-R75");
	if(p!=nullptr)
		cout << p->dPot << "\t" << p->dEStack;
*/

/*
	CStrategyTreeConfig StrategyTreeConfig;
	StrategyTreeConfig.Init(Max6_NL50_SD100);
	vector<double> candidateRatios;
	StrategyTreeConfig.GetFlopCandidateRatios("BTN_vsUTG_3bet<>X", candidateRatios, space_raise);
	Stacks stacks{ 79.180 };
	shared_ptr<CRTTreeConfigItem>p = StrategyTreeConfig.GetRTTreeConfig(stacks);
	cout << endl;
*/

	//string sFilePath = CDataFrom::GetWizardFilePath(Max6_NL50_SD100, "HJ_vsUTG_srp<KsQs7d>R50-R50-R50-A");
	//cout << sFilePath;

	// \Data\SolverFile\Max6_NL50_SD100\flop\HJ_vsUTG_srp<>\HJ_vsUTG_srp<KsQs7d>.json
	// \Data\SolverFile\Max6_NL50_SD100\turn\HJ_vsUTG_srp<>X-R33\HJ_vsUTG_srp<KsQs7d>X-R33<8d>.json
	//string sFilePath = CDataFrom::GetSolverFilePath(Max6_NL50_SD100, "HJ_vsUTG_srp<KsQs7d>X-R33<8d>");
	//cout << sFilePath << endl;

	//string sFilePath = CDataFrom::GetRangesFilePath(Max6_NL50_SD100, "3BET_#_vs_4callcold3bet4bet_EPfold");
	//cout << sFilePath << endl;

	//CBoard b;
	//b.SetFlop("KsQs7d");
	//cout << b.GetBoardSymbol();
	//{
	//	CStrategy strategy;
	//	strategy.LoadMacro("./test/macro.txt");
	//	for(auto it = strategy.m_macro.begin();it != strategy.m_macro.end();it++){
	//		cout << it->first << "," << it->second << endl;
	//			
	//}
	//vector<string> lines;
	//loadFileAsLine("./str1.txt",lines);
	//for(auto l : lines) {
	//	cout << l << endl;
	//}

	{
		CStrategy strategy;
		vector<shared_ptr<CStrategyItem>> strategys;

		/*shared_ptr<CStrategyItem> item1(new CStrategyItem);

		item1->m_action.actionType = raise;
		item1->m_action.fBetSize = 0.1;
		item1->m_action.fBetSizeByCash = 0.2;
		item1->m_action.fBetSizeByPot = 0.3;
		item1->m_strategyData["2d2c"] = 0.1;
		item1->m_strategyData["2h2c"] = 0.2;
		item1->m_strategyData["2h2d"] = 0.3;
		item1->m_evData["2d2c"] = 0.11;
		item1->m_evData["2h2c"] = 0.12;
		item1->m_evData["2h2d"] = 0.13;
		strategys.push_back(item1);

		strategy.DumpStrategy("strategy.txt",strategys);

		strategys.clear();*/

		strategy.ReadStrategy("./strategy.txt",strategys);

		cout << strategys.size() << endl;

		strategy.DumpStrategy("strategy.txt",strategys);


		//cout << strategys[0]->m_action.actionType << "," << strategys[0]->m_action.fBetSize << endl;

		//cout << strategys[0]->m_strategyData["2d2c"] << "," << strategys[0]->m_strategyData["2h2c"] << "," << strategys[0]->m_strategyData["2h2d"] << endl;

		//cout << strategys[0]->m_evData["2d2c"] << "," << strategys[0]->m_evData["2h2c"] << "," << strategys[0]->m_evData["2h2d"] << endl;
	}

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

