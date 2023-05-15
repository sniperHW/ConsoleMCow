// ConsoleMCow.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma comment(lib, "ws2_32.lib")
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
#include "CSpecialProcessingMacroConfig.h"
#include "CPokerHand.h"
#include "CMultiStrategy.h"
#include "CExploiConfig.h"
#include "CNet.h"

using namespace std;

//全局变量
map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;	//范围节点配置
map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
map<GameType, CActionMatchConfig> g_actionMatchConfigs; //preflop节点匹配表
map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs; //flop策略数据为wizard匹配表
map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs; //flop策略数据为solver匹配表
map<GameType, CStackByStrategyConfig> g_stackByStrategyConfig; //预存solver策略对应的筹码
map<GameType, CSpecialProcessingMacroConfig> g_specialProcessingMacroConfig; //预存special processing 宏指令

CSolver g_solver;
CMultiStrategy g_multiStrategy;
CExploiConfig g_ExploiConfig;

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

	CSpecialProcessingMacroConfig SpecialProcessingMacroConfig;
	if (!SpecialProcessingMacroConfig.Init(gmType))
		return false;
	else
		g_specialProcessingMacroConfig[gmType] = SpecialProcessingMacroConfig;

	return true;
}

extern bool loadFileAsLine(const string& path,vector<string> &lines);

int main()
{
	//load 所有gmType的config
	for (auto gmType : GameTypes) {
		bool blRet = LoadConfigs(gmType);
		if (!blRet)
			cout << "error: load config failed: " << GameTypeName[gmType] << endl;
	}

	if (!g_multiStrategy.Init()) 
		cout << "error: load config failed: g_multiStrategy" << endl;

	if (!g_ExploiConfig.Init())
		cout << "error: load config failed: g_ExploiConfig" << endl;

	CWdebug::DeleteDump();





	CSolution testSolution;
	testSolution.InitGame("GameID=1666879574;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[BTN];Hands=<KsJs>;");
	testSolution.HeroAction("[UTG]F,[HJ]R2,[CO]F");

	testSolution.ChangeRound("[BTN]C,[SB]F,[BB]F<9s7h6h>pot=5.5;EStack=[HJ]98,[BTN]98;");
	//testSolution.ChangeRound("[BTN]C,[SB]F,[BB]F<Ks7h4c>pot=5.5;EStack=[HJ]98,[BTN]98;");
		
	testSolution.HeroAction("[HJ]R1.9");

	//testSolution.ChangeRound("[BTN]R6.3,[HJ]C<Ks>pot=25.5;EStack=[HJ]78,[BTN]78;");

	testSolution.HeroAction("[BTN]R6.3,[HJ]R13");
	//testSolution.ChangeRound("[BTN]C<Ks>pot=25.5;EStack=[HJ]78,[BTN]78;");


}


