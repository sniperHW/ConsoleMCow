﻿// ConsoleMCow.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
#include <regex>
#include "CWdebug.h"


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
	else
		g_strategyNodeConfigs[gmType] = StrategyNodeConfig;

	CRangeNodeConfig RangeNodeConfig;
	if (!RangeNodeConfig.Init(gmType))
		return false;
	else
		g_rangeNodeConfigs[gmType] = RangeNodeConfig;

	CStrategyTreeConfig StrategyTreeConfig;
	if (!StrategyTreeConfig.Init(gmType))
		return false;
	else
		g_strategyTreeConfigs[gmType] = StrategyTreeConfig;


	CActionMatchConfig ActionMatchConfig;
	if (!ActionMatchConfig.Init(gmType))
		return false;
	else
		g_actionMatchConfigs[gmType] = ActionMatchConfig;

	CFlopDataFromWizardConfig FlopDataFromWizardConfig;
	if (!FlopDataFromWizardConfig.Init(gmType))
		return false;
	else
		g_flopDataFromWizardConfigs[gmType] = FlopDataFromWizardConfig;

	CTurnPresaveSolverConfig TurnPresaveSolverConfig;
	if (!TurnPresaveSolverConfig.Init(gmType))
		return false;
	else
		g_turnPresaveSolverConfigs[gmType] = TurnPresaveSolverConfig;

	CStackByStrategyConfig StackByStrategyConfig;
	if (!StackByStrategyConfig.Init(gmType))
		return false;
	else
		g_stackByStrategyConfig[gmType] = StackByStrategyConfig;

	return true;
}

extern bool loadFileAsLine(const string& path,vector<string> &lines);

int main()
{

	if (!LoadConfigs(Max6_NL50_SD100)) {
		cout << "Load config error!" << endl;
		return 0;
	}
	else
		cout << "Load Config finished." << endl;


/*
	cout << "Please input commands, gameover to finish." << endl;
	string sCommand, sRequireHead, sRequireContent;
	char szBuff[1024];
	cin.getline(szBuff,1024);
	sCommand = szBuff;
	cout << endl;
	CSolution* pSolution = new CSolution;
	while (1) {
		if (sCommand == "gameover") {
			delete pSolution;
			pSolution = new CSolution;
			cout << "New game begin, please input input commands." << endl;

			memset(szBuff, 0, sizeof(szBuff));
			cin.getline(szBuff, 1024);
			sCommand = szBuff;
			cout << endl;
		}
		else {
			auto stSpace = sCommand.find(" ");
			sRequireContent = sCommand.substr(stSpace +1);
			sRequireHead = sCommand.substr(0, stSpace);


			if (sRequireHead == "InitGame") {
				pSolution->InitGame(sRequireContent);
				cout << endl;
				cout << "Please input next commands, gameover to finish." << endl;
			}
			else if (sRequireHead == "HeroAction") {
				pSolution->HeroAction(sRequireContent);
				cout << endl;
				cout << "Please input next commands, gameover to finish." << endl;
			}
			else if (sRequireHead == "ChangeRound") {
				pSolution->ChangeRound(sRequireContent);
				cout << endl;
				cout << "Please input next commands, gameover to finish." << endl;
			}

			memset(szBuff, 0, sizeof(szBuff));
			cin.getline(szBuff, 1024);
			sCommand = szBuff;
			cout << endl;
		}
	}
*/
	CWdebug::DeleteDump();


	CSolution testSolution;

	vector<char> chars{ 'A','K','Q','J','T','9','8','7','6','5','4','3','2' };
	vector<char> charsSuit{ 'h','d','c','s' };



	//test bypresent
	//testSolution.InitGame("GameID=1666862283;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[SB]100.0,[BB]100.0;Hero=[HJ];Hands=<AhAd>;");
	//testSolution.HeroAction("[UTG]R2.0");
	//testSolution.ChangeRound("[HJ]C");

//test presave

	testSolution.InitGame("GameID=1666840379;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[BTN];Hands=<AhAd>;");
	testSolution.HeroAction("[UTG]F,[HJ]R2.0,[CO]F");
	testSolution.HeroAction("[BTN]R7.5,[SB]F,[BB]F,-,[HJ]R20.0");
	testSolution.ChangeRound("[BTN]C<AsTh9h>pot=45.5;EStack=[HJ]78.0,[BTN]78.0;");
	testSolution.HeroAction("[HJ]R12");
	testSolution.ChangeRound("[BTN]C<Ks>pot=69.5;EStack=[HJ]66.0,[BTN]66.0;");
//	testSolution.HeroAction("[BTN]R30,[HJ]R53");



 
	//test presave
//	testSolution.InitGame("GameID=1666879573;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]150.0,[HJ]150.0,[CO]150.0,[BTN]150.0,[SB]150.0,[BB]150.0;Hero=[HJ];Hands=<AhAd>;");
//	testSolution.HeroAction("[UTG]R2.5");
//	testSolution.ChangeRound("[HJ]C,[CO]F,[BTN]F,[SB]F,[BB]F<KsKdKc>pot=6.5;EStack=[UTG]147.5,[HJ]147.5;");
//	testSolution.HeroAction("[UTG]R2.1");
//	testSolution.HeroAction("[HJ]C<9s>pot=10.7;EStack=[UTG]145.4,[HJ]145.4;");

//	testSolution.InitGame("GameID=1666422279;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[HJ];Hands=<AhAd>;");
//	testSolution.HeroAction("[UTG]C,[HJ]F,[CO]F,[BTN]F,[SB]C"); 
//	testSolution.HeroAction("[BB]R2.0,-,[UTG]F,[SB]R4.0");
// 
	//testSolution.ChangeRound("[BTN]C,[SB]F,[BB]F<KsKdQc>pot=6.5;EStack=[HJ]97.5,[BTN]97.5;");
	//testSolution.HeroAction("[HJ]R2.1");

	//test utg_vs_hj_3bet
	//testSolution.InitGame("GameID=1666359512;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[UTG];Hands=<AhAd>;");
	//testSolution.HeroAction("");
	//testSolution.HeroAction("[UTG]R3.0,[HJ]R7.0,[CO]F,[BTN]F,[SB]F,[BB]F,-");

/*
	CBoard board;
	string sRowBoard = "KdKcQs";
	cout << "rowBoard:" << sRowBoard << endl;
	board.SetFlop(sRowBoard);
	board.SetTurn("As");

	cout << "Board:" << board.GetBoardSymbol() << endl;
	cout << "ISO_Board:" << board.GetIsoBoardSymbol() << endl;

	if (IsoFlops.find(board.GetIsoBoardSymbol()) == IsoFlops.end())
		cout << "not in ISO set" << endl;
	else
		cout << "in set" << endl;
	
	regex reg("<......>");
	string sReplace = "<" + board.GetIsomorphismSymbol() + ">";
	string sISONodeName = "EP_UTG_vs_2sqz3bet(HJ,BTN)<KdKcQs>X-X<As>";
	sISONodeName = regex_replace(sISONodeName, reg, sReplace);
	cout << sISONodeName;
*/
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

