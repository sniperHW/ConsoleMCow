//#include "pch.h"
#include "CSolution.h"
#include <regex>
#include <iostream>
#include "CStrategyTreeConfig.h"
#include "CSolver.h"
#include "CFlopDataFromSolverCalcConfig.h"
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
extern map<GameType, CFlopDataFromSolverCalcConfig> g_flopDataFromSolverCalcConfigs;
extern map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs;
extern CMultiStrategy g_multiStrategy;
extern CDataFrom g_DataFrom;

CSolution::CSolution()
{
	m_strategyFrom = from_strategy_file;
	m_blNotOffline = true;
	//time(&m_tmBegin);

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

	string sHeroAction = DumpSelAction(m_game.GetHero()->m_position, sActionLine);	//添加 SelA
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
		if (m_game.m_gmTypeBound.first != GAMETYPE_none) 
			m_blNotOffline = m_blNotOffline && strategyLow.Load(m_game.m_gmTypeBound.first, m_actionLine.m_sActionSquence);
		else
			cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;

		if (m_game.m_gmTypeBound.second != GAMETYPE_none) 
			m_blNotOffline = m_blNotOffline && strategyUp.Load(m_game.m_gmTypeBound.second, m_actionLine.m_sActionSquence);

		if (m_blNotOffline) {
			if (m_game.m_gmTypeBound.second == GAMETYPE_none)
				strategy = strategyLow;
			else {
				int nTemplate = m_game.m_gmType == m_game.m_gmTypeBound.first ? 0 : 1;
				//preflop betsize可能不同，按更接近的作为模板，以模板的betsize为准
				strategy.m_strategy = CStrategy::getAverageStrategyByStackDepth(strategyLow.m_strategy, strategyUp.m_strategy, m_game.m_dSegmentRatio, nTemplate,true);
			}
		}

		break;
	}
	case from_wizard: { //不会进入
		cout << "not support from_wizard" << endl;
		m_blNotOffline = strategy.Load(m_game.m_gmType, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_oopx, m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());
		break;
	}
	case from_solver_presave: { 
		//如果turn属于常用行动线，则转为实时计算，from_solver_presave保持不变，后续行动线不匹配重新计算则会沿用已经计算好的结果
		recalc_actionline recaleMode = recalc_none;
		double dBetBeforeBigRaise = 0;
		double dRaiseSize = 0;
		if(m_game.m_round == turn)
			recalc_actionline recaleMode = CSolver::IsRecalcActionline(m_game.m_dPot,m_actionLine.m_sActionSquence, dBetBeforeBigRaise, dRaiseSize);

		if (recaleMode != recalc_none) { //需要实时计算(未测试)
			CSolverConfig solverConfig;
			solverConfig.m_stacks = GetStacks();
			solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
			solverConfig.m_pRange = &m_range;
			solverConfig.m_pRTTreeConfigItem = g_strategyTreeConfigs[m_game.m_gmType].GetRTTreeConfig(recalc_mode_spr.at(recaleMode));

			g_solver.AdjustTree(solverConfig, recaleMode, (int)dBetBeforeBigRaise, (int)dRaiseSize); //只有本presave的转实时计算需要简化树
			m_game.m_board.ClearSuitReplace(); //实时计算不需要同构转换

			m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig);

			if (m_blNotOffline)
				m_blNotOffline = g_solver.GetSolverResult(m_game.m_sID, m_solverResultLow);

			if (m_blNotOffline)
				m_blNotOffline = strategy.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());
		}
		else {

			if (m_actionLine.m_sActionSquence.find('$') != string::npos) //动作序列最后是-A时会加该标记，但solver模式不需要处理
				m_actionLine.m_sActionSquence.pop_back();

			if (m_game.m_gmTypeBound.first != GAMETYPE_none) {
				m_blNotOffline = m_blNotOffline && strategyLow.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());
			}
			else
				cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;

			if (m_game.m_gmTypeBound.second != GAMETYPE_none) {
				m_blNotOffline = m_blNotOffline && strategyUp.Load(m_solverResultUp, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());
			}

			if (m_blNotOffline) {
				if (m_game.m_gmTypeBound.second == GAMETYPE_none)
					strategy = strategyLow;
				else {
					int nTemplate = m_game.m_gmType == m_game.m_gmTypeBound.first ? 0 : 1;
					strategy.m_strategy = CStrategy::getAverageStrategyByStackDepth(strategyLow.m_strategy, strategyUp.m_strategy, m_game.m_dSegmentRatio, nTemplate);
				}
			}
		}

		break;
	}
	case from_solver_calc: {
		m_blNotOffline = g_solver.GetSolverResult(m_game.m_sID, m_solverResultLow);
		if(m_blNotOffline)
			m_blNotOffline = strategy.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());	//策略筹码即实际筹码
	}
	case from_solver_realtime: {
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		m_game.m_board.ClearSuitReplace(); //实时计算不需要同构转换

		m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig);
		if (m_blNotOffline)
			m_blNotOffline = g_solver.GetSolverResult(m_game.m_sID, m_solverResultLow);

		if (m_blNotOffline)
			m_blNotOffline = strategy.Load(m_solverResultLow, m_actionLine.m_sActionSquence, GetStacks(), m_game.m_board.GetIsomorphismSuitReplace());
		break;
	}
	case multi_players: {
		Action a = g_multiStrategy.GetHeroAction(m_actionLine.m_multiCondition, m_actionLine.m_pokerEv); //这里计算后立即返回
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
	Stacks stackPre = GetStacks();	//Parse后会更新stack,而计算范围要按前一条街的，所以先保留

	//解析ActionLine
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

	//加载range(round已经为新的,m_strategyFrom为旧的)
	if (m_game.m_round == flop && m_strategyFrom != multi_players) { //file模式
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
	else if (m_strategyFrom == from_wizard) { //wizard模式,不会进入
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_actionLine.m_sNodeName, m_game.m_board.GetIsomorphismSuitReplace(), m_game.m_board.GetIsomorphismSymbol());
	}
	else if (m_strategyFrom == from_solver_presave) { //solver模式
		CRange rangeLow = m_range;
		CRange rangeUp = m_range;

		bool blNotOffline = true;

		if (m_game.m_gmTypeBound.first != GAMETYPE_none) { 
			blNotOffline = blNotOffline && rangeLow.Load(m_game.m_gmTypeBound.first, m_solverResultLow, m_actionLine.m_sActionSquence, stackPre, m_game.m_board.GetIsomorphismSuitReplace());
		}
		else
			cout << "error: m_gmTypeBound.first can't be GAMETYPE_none" << endl;

		if (m_game.m_gmTypeBound.second != GAMETYPE_none) {
			blNotOffline = blNotOffline && rangeUp.Load(m_game.m_gmTypeBound.second, m_solverResultUp, m_actionLine.m_sActionSquence, stackPre, m_game.m_board.GetIsomorphismSuitReplace());
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
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_solverResultLow, m_actionLine.m_sActionSquence, stackPre, m_game.m_board.GetIsomorphismSuitReplace());
	}
	else if (m_strategyFrom == from_solver_calc) { //?
		m_blNotOffline = m_range.Load(m_game.m_gmType, m_solverResultLow, m_actionLine.m_sActionSquence, stackPre, m_game.m_board.GetIsomorphismSuitReplace());
	}
	else if (m_strategyFrom == multi_players) { //这里不需要做如何事

		return true;
	}


	#ifdef FOR_TEST_DUMP_ 
	DumpActionBeforeChangeRound(m_game.GetHero()->m_position); //hero
	string sComment = "herorange" + m_actionLine.m_sActionSquence;
	RelativePosition rpHero = m_game.GetHero()->m_positionRelative;

	m_range.DumpRange(m_game.GetHero()->m_position, sComment, rpHero);
	#endif


	//设置本街数据来源
	if (m_strategyFrom != multi_players) { //如果多人或因意外变为多人则保持多人策略
		if (m_game.m_round == flop) {
			if (g_flopDataFromSolverCalcConfigs[m_game.m_gmType].IsFrSolverCalc(m_actionLine.m_sAbbrName))
				m_strategyFrom = from_solver_calc;
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
	}


	//加载solver presave
	if (m_strategyFrom == from_solver_presave) {
		//将m_sNodeName的flop的board转为同构
		string sISONodeName = m_actionLine.m_sNodeName;

		regex regFlop("<......>");
		string sReplaceFlop = "<" + m_game.m_board.GetIsomorphismSymbol() + ">";
		sISONodeName = regex_replace(sISONodeName, regFlop, sReplaceFlop);

		if(m_game.m_round == turn){ //turn时也要把board的turn发牌转花色
			regex regTurn("<..>");
			string sReplaceTurn = "<" + m_game.m_board.GetIsomorphismSymbolTurn() + ">";
			sISONodeName = regex_replace(sISONodeName, regTurn, sReplaceTurn);
		}

		bool isNeedDecompress = m_game.m_round == turn ? true : false;

		if (m_game.m_gmTypeBound.first != GAMETYPE_none) {
			string sFilePath = g_DataFrom.GetSolverFilePath(m_game.m_gmTypeBound.first, sISONodeName);
			if (!CSolver::LoadSolverFile(sFilePath, m_solverResultLow, isNeedDecompress)) {
				m_blNotOffline = false;
				return false;
			}
		}

		if (m_game.m_gmTypeBound.second != GAMETYPE_none) {
			string sFilePath = g_DataFrom.GetSolverFilePath(m_game.m_gmTypeBound.second, sISONodeName);
			if (!CSolver::LoadSolverFile(sFilePath, m_solverResultUp, isNeedDecompress)) {
				m_blNotOffline = false;
				return false;
			}
		}
	}
	else if (m_strategyFrom == from_solver_calc) { //换街时解算
		CSolverConfig solverConfig;
		solverConfig.m_stacks = GetStacks();
		solverConfig.m_sBoard = m_game.m_board.GetBoardSymbol();
		solverConfig.m_pRange = &m_range;
		solverConfig.m_pRTTreeConfigItem = g_strategyTreeConfigs[m_game.m_gmType].GetRTTreeConfig(GetStacks());
		m_game.m_board.ClearSuitReplace(); //实时计算不需要同构转换

		m_blNotOffline = g_solver.ToSolve(m_game.m_sID, solverConfig);
	}

	m_heroCurRange = m_game.GetHero()->m_positionRelative == OOP ? m_range.m_OOPRange : m_range.m_IPRange;

	return true;
}

//如果返回fBetSize非0，fBetSizeByPot==0,则直接启用fBetSize
//客户端需要做的判断和处理：
//比例和size都要填写，平台支持比例和默认比例按钮存在则用比例，否则用size，比例转为size调用strategy::CalcBetSize()
//当size>hero当前剩余筹码时转为allin（由客户端判断）
//todo:策略里没有手牌时的处理
Action CSolution::CalcHeroAction(const CStrategy& strategy)
{
	Action retAction{ fold,0,0,0 };
	string sHand = m_game.GetHero()->m_hands.m_sSymbol; //已经align了
	
	//todo:sHand排序问题

	vector<pair<Action, double>> handStrategy;	//action,ratio
	vector<double> randBounds;	//随机数范围

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



	//未测试-------------------
	//hero的手牌无对应策略
	if (!blHandFound) {
		cout << "warning: CalcHeroAction hand not found in strategy" << endl;
		set<string> allinCombos{"AA","KK","AKs"}; //脱线时这些combo allin
		if (m_game.m_round == preflop) {
			string sHandAbbrName = CCombo::GetAbbrSymble(sHand);

			if (allinCombos.find(sHandAbbrName) != allinCombos.end()) { //to allin
				Action retActionAllin{ allin,0,0,0 };
				return retActionAllin;
			}
			else { //to fold
				return retAction;
			}
		}
		else { //翻后
			retAction = g_multiStrategy.GetHeroAction(m_actionLine.m_multiCondition, m_actionLine.m_pokerEv);
			return retAction;
		}
	}

	//检查有效性，产生随机范围
	double dRatioSum = 0;
	for (auto it : handStrategy) {
		randBounds.push_back(dRatioSum);
		dRatioSum += it.second;
	}
	if (fabs(dRatioSum - 1) > 0.01)
		cout << "error: CalcHeroAction ratio sum error > 0.01" << endl;

	//随机命中
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

	//检查betsize的合法性(最少为对手下注的两倍，preflop由于存在对手下注特别大的情况下会发生)
	if(retAction.actionType == raise && m_game.m_round == preflop){
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
			retAction.fBetSize = (float)(dRivalMaxRaiseSize * 2); //(多0.1为留一定余地)
			retAction.fBetSizeByPot = 0; //只启用fBetSize
		}
	}
	
	m_heroLastStrategy = strategy.GetStrategy(); //用于exo;oi


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

//格式：GameID=353545;BBSize=0.1;Pot=1.5;Plays=[UTG]98,[HJ]67,[CO]34,[BTN]120,[SB]78,[BB]286;Hero=[UTG];Hands=<KsKd>;//位置上玩家缺席则没有
//不满6人，则SB，BB固定，其他按UTG..顺序以此，运行位置少人
bool CSolution::InitGame(const string& sInitGame)
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
		sHands = CCombo::Align(m[1]);

	//初始化players
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
	return true;
}

bool CSolution::HeroHands(const string& sHands)
{
	m_game.GetHero()->m_hands.SetHands(CCombo::Align(sHands));
	return true;
}

Stacks CSolution::GetStacks()
{
	Stacks stacks;
	stacks.dPot = m_game.m_dPot;

	auto it = min_element(m_game.m_players.begin(), m_game.m_players.end(), [](auto elem1, auto elem2) {return elem1.second.m_dEStack < elem2.second.m_dEStack; });
	stacks.dEStack = it->second.m_dEStack;

	return stacks;
}

//preflop实际逻辑时没有匹配预存行动线则会返回no_match_default，触发对应策略，而Offline是匹配了策略但因为文件缺失等原因
Action CSolution::ProcessingOffline()
{
	Action a;

	if (m_game.m_round == preflop) {
		CStrategy strategy;
		strategy.SpecialProcessing(m_game.m_gmType, "default(5bet)");
		a = CalcHeroAction(strategy);
	}
	else {
		cout << "Offline:  " << m_actionLine.m_sActionSquence << endl;
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

//是增量，所以hero只会有一个
string CSolution::DumpSelAction(const Position position, const std::string& sActionLine)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\dump\\";

	#ifdef FOR_TESTCLIENT_DUMP__ 
	sConfigFolder = sConfigFolder + PositionSymble[position] + "\\";
	#endif

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
				posActions.push_back(CActionLine::ToPosActionPair(p->str()));	//"-"标记为none,nonepos
		}

		for (auto it : posActions) {
			auto p = m_game.m_players.find(it.first);
			if (p != m_game.m_players.end()) {
				if (p->second.m_blIsHero) {
					sAction = CActionLine::ToActionSymbol(it.second, true);

					ofstream ofCommands;
					ofCommands.open(sCommandsPath, ofstream::out | ios_base::app);
					if (ofCommands.is_open()) {
						//格式：87969;	SelA; comment;	R12
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

void CSolution::DumpActionBeforeChangeRound(const Position position, const bool blDumpHero)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\dump\\";

	#ifdef FOR_TESTCLIENT_DUMP__ 
	sConfigFolder = sConfigFolder + PositionSymble[position] + "\\";
	#endif

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
					//格式：87969;	SelA_BeforeCR; comment;	R12
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

Position CSolution::GetHeroPosition()
{
	return m_game.GetHero()->m_position;
}




