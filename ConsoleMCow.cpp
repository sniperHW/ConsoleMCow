﻿// ConsoleMCow.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
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
#include "CFlopDataFromSolverCalcConfig.h"
#include "CDataFrom.h"
#include "CBoard.h"
#include <regex>
#include "CWdebug.h"
#include "CSpecialProcessingMacroConfig.h"
#include "CPokerHand.h"
#include "CMultiStrategy.h"
#include "CExploiConfig.h"
#include "CSolutions.h"
#include "CNet.h"



using namespace std;

//全局变量
map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置，目前只用于配置special
map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
map<GameType, CActionMatchConfig> g_actionMatchConfigs; //preflop节点匹配表，翻前abbrname和动作序列的对应关系
map<GameType, CFlopDataFromSolverCalcConfig> g_flopDataFromSolverCalcConfigs; //flop策略数据为换街时实时计算匹配表(默认flop为预存数据，实时计算的在配置中指出，目前没有)
map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs; //turn策略数据为预存匹配表
map<GameType, CSpecialProcessingMacroConfig> g_specialProcessingMacroConfig; //预存special processing 宏指令


/*
//以下是目前不用的
//map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;	//范围节点配置
//map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs; //flop策略数据为wizard匹配表
//map<GameType, CStackByStrategyConfig> g_stackByStrategyConfig; //预存solver策略对应的筹码

//和配置文件的对应
map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //StrategyNodeConfig.txt
map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //FlopTreeConfig.txt;		RTTreeConfig.txt
map<GameType, CActionMatchConfig> g_actionMatchConfigs; //preflop_ActionMatchConfig.txt;	toflop_ActionMatchConfig.txt
map<GameType, CFlopDataFromSolverCalcConfig> g_flopDataFromSolverCalcConfigs; //FlopDataFromSolverCalcConfig.txt
map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs; //TurnPresaveSolverConfig.txt
map<GameType, CSpecialProcessingMacroConfig> g_specialProcessingMacroConfig; //SpecialProcessingMacroConfig.txt
*/

CSolver g_solver;
CMultiStrategy g_multiStrategy;
CExploiConfig g_ExploiConfig;
CDataFrom g_DataFrom;

CSolutions g_Solutions;

bool LoadConfigs(GameType gmType)
{
	//替换策略名称目前不使用（用原名，保留替换后的数据文件），仅使用special设置
	CStrategyNodeConfig StrategyNodeConfig;
	if (!StrategyNodeConfig.Init(gmType))
		return false;
	else
		g_strategyNodeConfigs[gmType] = StrategyNodeConfig;

	//目前不使用
	//CRangeNodeConfig RangeNodeConfig;
	//if (!RangeNodeConfig.Init(gmType))
	//	return false;
	//else
	//	g_rangeNodeConfigs[gmType] = RangeNodeConfig;


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

	//目前不用
	//CFlopDataFromWizardConfig FlopDataFromWizardConfig;
	//if (!FlopDataFromWizardConfig.Init(gmType))
	//	return false;
	//else
	//	g_flopDataFromWizardConfigs[gmType] = FlopDataFromWizardConfig;

	CFlopDataFromSolverCalcConfig FlopDataFromSolverCalcConfig;
	if (!FlopDataFromSolverCalcConfig.Init(gmType))
		return false;
	else
		g_flopDataFromSolverCalcConfigs[gmType] = FlopDataFromSolverCalcConfig;


	CTurnPresaveSolverConfig TurnPresaveSolverConfig;
	if (!TurnPresaveSolverConfig.Init(gmType))
		return false;
	else
		g_turnPresaveSolverConfigs[gmType] = TurnPresaveSolverConfig;

	//目前不用
	//CStackByStrategyConfig StackByStrategyConfig;
	//if (!StackByStrategyConfig.Init(gmType))
	//	return false;
	//else
	//	g_stackByStrategyConfig[gmType] = StackByStrategyConfig;

	CSpecialProcessingMacroConfig SpecialProcessingMacroConfig;
	if (!SpecialProcessingMacroConfig.Init(gmType))
		return false;
	else
		g_specialProcessingMacroConfig[gmType] = SpecialProcessingMacroConfig;

	if (!g_DataFrom.Init())
		return false;

	if (!g_solver.Init())
		return false;

	return true;
}

extern bool loadFileAsLine(const string& path,vector<string> &lines);



//commands
//HeroAction:返回: "R/C/A/F/X,fBetSize,fBetSizeByPot"; 参数:sActionLine
//ChangeRound:返回:"s/f"; 参数:sActionLine
//InitGame:返回:"s/f"; 参数:sInitGame
//HeroHands:返回:"s/f"; 参数:sHands
//EndGame返回:"s/f"; 无参数
void onPakcet(SOCKET fd, const std::string& packet)
{
	auto req = split(packet, ':');
	auto sGameID = req[0];
	auto cmd = req[1];
	std::string arg;
	if (req.size() > 2) {
		arg = req[2];
	}

	if (cmd == cmd_InitGame) {
		shared_ptr<CSolution> pSolution = g_Solutions.NewSolution(sGameID);
		if (pSolution == nullptr) {
			CNet::SendPacket(fd, "InitGame failed, solution ID already exist.");
			return;
		}

		bool r = pSolution->InitGame(arg);
		if(r)
			CNet::SendPacket(fd, "InitGame sucess.");
		else
			CNet::SendPacket(fd, "InitGame failed.");

		CWdebug::DeleteDump(pSolution->GetHeroPosition());
	}
	else if (cmd == cmd_HeroAction) { //客户端需要搜索"failed",没有则正常逻辑处理（是否要检查无效action?）
		shared_ptr<CSolution> pSolution = g_Solutions.GetSolutionByGameID(sGameID);
		if (pSolution == nullptr) {
			CNet::SendPacket(fd, "HeroAction failed, solution ID not exist.");
			return;
		}

		Action a = pSolution->HeroAction(arg);
		string sAction = actionType2StringSimple(a.actionType)+","+ double2String(a.fBetSize,1)+","+ double2String(a.fBetSizeByPot,1);

		CNet::SendPacket(fd, sAction);
	}
	else if (cmd == cmd_ChangeRound) {
		shared_ptr<CSolution> pSolution = g_Solutions.GetSolutionByGameID(sGameID);
		if (pSolution == nullptr) {
			CNet::SendPacket(fd, "HeroAction failed, solution ID not exist.");
			return;
		}

		bool r = pSolution->ChangeRound(arg);
		if (r)
			CNet::SendPacket(fd, "ChangeRound sucess.");
		else
			CNet::SendPacket(fd, "ChangeRound failed.");
	}
	else if (cmd == cmd_HeroHands) {
		shared_ptr<CSolution> pSolution = g_Solutions.GetSolutionByGameID(sGameID);
		if (pSolution == nullptr) {
			CNet::SendPacket(fd, "HeroHands failed, solution ID not exist.");
			return;
		}

		bool r = pSolution->HeroHands(arg);
		if (r)
			CNet::SendPacket(fd, "HeroHands sucess.");
		else
			CNet::SendPacket(fd, "HeroHands failed.");
	}
	else if (cmd == cmd_EndGame) {
		shared_ptr<CSolution> pSolution = g_Solutions.GetSolutionByGameID(sGameID);
		if (pSolution == nullptr) {
			CNet::SendPacket(fd, "EndGame failed, solution ID not exist.");
			return;
		}

		g_Solutions.FinishSolution(sGameID);

		CNet::SendPacket(fd, "EndGame sucess.");
	}
	else {
		cout << "error: invalid cmd" << "\t" << cmd << endl;
		CNet::SendPacket(fd, "invalid cmd");
	}
}

//todo:
//*备忘：增加判断两个hand是同一个的函数（2d2c==2c2d）,用private类(确认下已经完成)
//*preflop不同筹码深度下注量有区别是怎么平均的？（按更接近的模板）
//.多人dump的处理（未完成移到solution）
//*turn非标准bet的处理
//整理屏幕输出，日志和dump(全面测试时)
//solver端握手，完成任务，定时检查
int main()
{
/*
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

	//CWdebug::DeleteDump();
*/

	//init net
	int ret = CNet::InitNetSystem();
	if (ret == -1) {
		cout << "error:net init failed." << endl;
		return 0;
	}
	int r = CNet::Listen(8000, onPakcet);
	::getchar();

}






/*
	CBoard testBoard;
	testBoard.SetFlop("7c8c9c");
	testBoard.SetTurn("Tc");
	string s1 = testBoard.GetIsoBoardSymbol();
	string s2 = testBoard.GetIsomorphismSymbol();
	string s3 = testBoard.GetBoardSymbol();
	SuitReplace r= testBoard.GetIsomorphismSuitReplace();
	string s4 = testBoard.GetIsomorphismSymbolTurn();

	cout << s4 << endl;
*/



//CSolution testSolution;
//testSolution.InitGame("GameID=1712739527;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[BB];Hands=<8h7h>;");
//testSolution.HeroAction("[UTG]R2,[HJ]R7.6");
//testSolution.HeroAction("[UTG]F,[HJ]F,[CO]F,[BTN]F,[SB]F");

//testSolution.InitGame("GameID=1712739527;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[HJ];Hands=<AhAd>;");
//testSolution.HeroAction("[UTG]R2.5");
//testSolution.HeroAction("[HJ]C,[CO]R10.0,[BTN]F,[SB]F,[BB]F,-,[UTG]F");
//testSolution.HeroAction("[HJ]R40.0,[CO]R80.0,-");



//testSolution.InitGame("GameID=1666879574;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[UTG];Hands=<QsJs>;");
//testSolution.HeroAction("");
//testSolution.HeroAction("[UTG]R2");
//testSolution.ChangeRound("[UTG]R2,[HJ]C,[CO]F,[BTN]F,[SB]F,[BB]F<2d3h4h>pot=5.5;EStack=[UTG]98,[HJ]98;");
//testSolution.HeroAction("[UTG]R1.8");
//testSolution.ChangeRound("[HJ]C<5h>pot=9.1;EStack=[UTG]96.2,[HJ]96.2;");


//testSolution.HeroAction("[UTG]F,[HJ]F,[CO]F,[BTN]R2,[SB]F");
//testSolution.ChangeRound("[BB]C<8hTdJc>pot=4.5;EStack=[BTN]48,[BB]48;");
//testSolution.HeroAction("");
//testSolution.HeroAction("[BB]X,[BTN]R1.49");

//testSolution.HeroAction("[BTN]R6.3,[HJ]R13");

//testSolution.ChangeRound("[BTN]C,[SB]F,[BB]F<Ks7h4c>pot=5.5;EStack=[HJ]98,[BTN]98;");
//testSolution.ChangeRound("[BTN]R6.3,[HJ]C<Ks>pot=25.5;EStack=[HJ]78,[BTN]78;");
	//testSolution.ChangeRound("[BTN]C<Ks>pot=25.5;EStack=[HJ]78,[BTN]78;");


