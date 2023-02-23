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

	if (!g_multiStrategy.Init())
		return false;

	return true;
}

extern bool loadFileAsLine(const string& path,vector<string> &lines);

int main()
{

	{
		std::string request = std::string("set_pot 50\n");
		request.append("set_effective_stack 200\n");
		request.append("set_board Qs,Jh,2h\n");
		request.append("set_range_ip AA,KK,QQ,JJ,TT,99:0.75,88:0.75,77:0.5,66:0.25,55:0.25,AK,AQs,AQo:0.75,AJs,AJo:0.5,ATs:0.75,A6s:0.25,A5s:0.75,A4s:0.75,A3s:0.5,A2s:0.5,KQs,KQo:0.5,KJs,KTs:0.75,K5s:0.25,K4s:0.25,QJs:0.75,QTs:0.75,Q9s:0.5,JTs:0.75,J9s:0.75,J8s:0.75,T9s:0.75,T8s:0.75,T7s:0.75,98s:0.75,97s:0.75,96s:0.5,87s:0.75,86s:0.5,85s:0.5,76s:0.75,75s:0.5,65s:0.75,64s:0.5,54s:0.75,53s:0.5,43s:0.5\n");
		request.append("set_range_oop QQ:0.5,JJ:0.75,TT,99,88,77,66,55,44,33,22,AKo:0.25,AQs,AQo:0.75,AJs,AJo:0.75,ATs,ATo:0.75,A9s,A8s,A7s,A6s,A5s,A4s,A3s,A2s,KQ,KJ,KTs,KTo:0.5,K9s,K8s,K7s,K6s,K5s,K4s:0.5,K3s:0.5,K2s:0.5,QJ,QTs,Q9s,Q8s,Q7s,JTs,JTo:0.5,J9s,J8s,T9s,T8s,T7s,98s,97s,96s,87s,86s,76s,75s,65s,64s,54s,53s,43s\n");
		request.append("set_bet_sizes oop,flop,bet,50\n");
		request.append("set_bet_sizes oop,flop,raise,60\n");
		request.append("set_bet_sizes oop,flop,allin\n");
		request.append("set_bet_sizes ip,flop,bet,50\n");
		request.append("set_bet_sizes ip,flop,raise,60\n");
		request.append("set_bet_sizes ip,flop,allin\n");
		request.append("set_bet_sizes oop,turn,bet,50\n");
		request.append("set_bet_sizes oop,turn,raise,60\n");
		request.append("set_bet_sizes oop,turn,allin\n");
		request.append("set_bet_sizes ip,turn,bet,50\n");
		request.append("set_bet_sizes ip,turn,raise,60\n");
		request.append("set_bet_sizes ip,turn,allin\n");
		request.append("set_bet_sizes oop,river,bet,50\n");
		request.append("set_bet_sizes oop,river,donk,50\n");
		request.append("set_bet_sizes oop,river,raise,60,100\n");
		request.append("set_bet_sizes oop,river,allin\n");
		request.append("set_bet_sizes ip,river,bet,50\n");
		request.append("set_bet_sizes ip,river,raise,60,100\n");
		request.append("set_bet_sizes ip,river,allin\n");
		request.append("set_allin_threshold 0.67\n");
		request.append("build_tree\n");
		request.append("set_thread_num 8\n");
		request.append("set_accuracy 0.5\n");
		request.append("set_max_iteration 200\n");
		request.append("set_print_interval 10\n");
		request.append("set_use_isomorphism 1\n");
		request.append("start_solve\n");
		request.append("set_dump_rounds 2\n");
		request.append("dump_result output_result.json\n");

		CNet::InitNetSystem();

		auto resp = CNet::Request(std::string("127.0.0.1"), 8081, request);

		auto jsonStr = resp->GetData();

		Json::Value root;
		Json::CharReaderBuilder builder;
		JSONCPP_STRING errs;
		std::istringstream istr(jsonStr);
		if (!parseFromStream(builder, istr, &root, &errs)) {
			std::cout << errs << std::endl;
		}
		else {
			std::cout << "parse json ok" << std::endl;
		}

	}


	if (!LoadConfigs(Max6_NL50_SD100)) {
		cout << "Load config error!" << endl;
		return 0;
	}
	else
		cout << "Load Config finished." << endl;

	//CWdebug::DeleteDump();
	CSolution testSolution;

	testSolution.InitGame("GameID=1666879574;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[CO];Hands=<AhAd>;");
	testSolution.HeroAction("[UTG]R2.5,[HJ]C");
	testSolution.ChangeRound("[CO]C,[BTN]F,[SB]F,[BB]F<KsKdKc>pot=9;EStack=[UTG]97.5,[HJ]97.5,[CO]97.5;");
	//testSolution.HeroAction("[UTG]R3,[HJ]C");
	//testSolution.HeroAction("[CO]R8,[UTG]R19,[HJ]C");


	//testSolution.HeroAction("[UTG]R2.1");
	//testSolution.HeroAction("[HJ]C<9s>pot=10.7;EStack=[UTG]145.4,[HJ]145.4;");

	//testSolution.ChangeRound("[HJ]C,[CO]C,[BTN]F,[SB]F,[BB]F<KsKdKc>pot=6.5;EStack=[UTG]97.5,[HJ]97.5,[CO]97.5;");















}

/*测试牌型
MyCards privates;
MyCards publics;

privates.push_back(myCard("Qc"));
privates.push_back(myCard("Ad"));

publics.push_back(myCard("Ks"));
publics.push_back(myCard("3h"));
publics.push_back(myCard("Jd"));
//publics.push_back(myCard("3s"));
//publics.push_back(myCard("3d"));

CPokerHand pokerHand;
PokerEvaluate pe = pokerHand.getPokerEvaluate(privates, publics);
//PokerEvaluate pe = testPairEvaluate(privates, publics);
cout << "rankGroup:  " << pe.rankGroup.nMainGroup << "," << pe.rankGroup.nSubGroup << endl;
*/

/* 测试命令
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

/* test special
	CStrategy testStrategy;

	vector<string> combosAK = CCombo::GetCombosByAbbr("AK");
	vector<string> combosAKs = CCombo::GetCombosByAbbr("AKs");

	shared_ptr<CStrategyItem> strategyItemAllin(new CStrategyItem);
	strategyItemAllin->m_action.actionType = allin;
	strategyItemAllin->m_action.fBetSize = 0;
	for (auto it: ComboMapping)
		strategyItemAllin->m_strategyData[it] = 0;
	for (auto it : combosAK)
		strategyItemAllin->m_strategyData[it] = 0.1;
	//strategyItemAllin->m_strategyData["AcKc"] = 0.009;

	strategyItemAllin->m_evData["AhKh"] = 0.1;
	strategyItemAllin->m_evData["AcKc"] = 0.2;
	strategyItemAllin->m_evData["AdKd"] = 0;
	strategyItemAllin->m_evData["AsKs"] = 0.09;


	shared_ptr<CStrategyItem> strategyItemRaise(new CStrategyItem);
	strategyItemRaise->m_action.actionType = raise;
	strategyItemRaise->m_action.fBetSize = 2;
	for (auto it : ComboMapping)
		strategyItemRaise->m_strategyData[it] = 0;
	for (auto it : combosAK)
		strategyItemRaise->m_strategyData[it] = 0.4;
	strategyItemRaise->m_strategyData["AcKc"] = 0;

	strategyItemRaise->m_evData["AhKh"] = 0.1;
	strategyItemRaise->m_evData["AcKc"] = 0.2;
	strategyItemRaise->m_evData["AdKd"] = 0;
	strategyItemRaise->m_evData["AsKs"] = 0.09;

	shared_ptr<CStrategyItem> strategyItemCall(new CStrategyItem);
	strategyItemCall->m_action.actionType = call;
	for (auto it : ComboMapping)
		strategyItemCall->m_strategyData[it] = 0;
	for (auto it : combosAK)
		strategyItemCall->m_strategyData[it] = 0.2;


	shared_ptr<CStrategyItem> strategyItemFold(new CStrategyItem);
	strategyItemFold->m_action.actionType = fold;
	for (auto it : ComboMapping) {
		strategyItemFold->m_strategyData[it] = 1;
	}
	for (auto it : combosAK)
	{
		strategyItemFold->m_strategyData[it] = 0.3;
	}
	strategyItemFold->m_strategyData["AcKc"] = 0.7;


	testStrategy.m_strategy.push_back(strategyItemAllin);
	testStrategy.m_strategy.push_back(strategyItemRaise);
	testStrategy.m_strategy.push_back(strategyItemCall);
	testStrategy.m_strategy.push_back(strategyItemFold);



	//for test
	for (auto it : testStrategy.m_strategy)
	{
		int nCount = 0;
		cout << "actiontype:" << CActionLine::ToActionSymbol(it->m_action) << endl;
		for (auto e : it->m_strategyData) {
			if (CCombo::GetAbbrSymble(e.first) == "AKs" || CCombo::GetAbbrSymble(e.first) == "AKo") {
				cout << e.first << ":" << e.second << ", ";
				nCount++;
			}
		}
		cout << endl << "Count:" << nCount << endl << endl;
	}





	//string sCommand = "Assign[raise2](AKs)";
	//string sCommand = "Discard[allin](AKs)<EV+=0.1>";
	//string sCommand = "Replace[whole][raise](AKs)<EV+=0.1>";
	string sCommand = "Replace[call][raise](AKs)";
	testStrategy.SpecialProcessing(Max6_NL50_SD100,sCommand);



	//for test
	for (auto it : testStrategy.m_strategy)
	{
		int nCount = 0;
		cout << "actiontype:" << CActionLine::ToActionSymbol(it->m_action) << endl;
		for (auto e : it->m_strategyData) {
			if (CCombo::GetAbbrSymble(e.first) == "AKs" || CCombo::GetAbbrSymble(e.first) == "AKo") {
				cout << e.first << ":" << e.second << ", ";
				nCount++;
			}
		}
		cout << endl << "Count:" << nCount << endl << endl;
	}

*/



//test limp
/*
	testSolution.InitGame("GameID=1667099118;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[BB];Hands=<AhAd>;");
	testSolution.HeroAction("[UTG]F,[HJ]F,[CO]F,[BTN]F,[SB]C");
	testSolution.ChangeRound("[BB]X<As9hTh>pot=2.0;EStack=[SB]99.0,[BB]99.0;");
	testSolution.HeroAction("[SB]R3.0");
	testSolution.ChangeRound("[BB]C<Ks>pot=8.0;EStack=[SB]96.0,[BB]96.0;");
*/





//test presave
//testSolution.InitGame("GameID=1666840379;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[BTN];Hands=<AhAd>;");
//testSolution.HeroAction("[UTG]F,[HJ]R2.0,[CO]F");
//testSolution.HeroAction("[BTN]R7.5,[SB]F,[BB]F,-,[HJ]R20.0");
//testSolution.ChangeRound("[BTN]C<AsTh9h>pot=45.5;EStack=[HJ]78.0,[BTN]78.0;");	//pot和estack模拟客户端程序计算有误差，不影响测试
//testSolution.HeroAction("[HJ]R12");
//	testSolution.ChangeRound("[BTN]C<Ks>pot=69.5;EStack=[HJ]66.0,[BTN]66.0;");





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



