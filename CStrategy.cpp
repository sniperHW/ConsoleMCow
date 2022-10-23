//#include "pch.h"
#include "CStrategy.h"
#include "util.h"
#include <fstream>
#include <iostream>
#include <regex>
#include "CStrategyNodeConfig.h"
#include "CStrategyTreeConfig.h"
#include "CDataFrom.h"
#include "CCombo.h"
#include "CActionLine.h"


using namespace std;
extern map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置

bool CStrategy::parseActionSquence(const string& sActionSquence, string& sPrefix, Round &round,vector<Action>& actions,string &actionStr) {
	//查找最后的>
	auto pos = sActionSquence.rfind('>');
	if(pos != string::npos && pos != sActionSquence.size()-1) {
		actionStr = sActionSquence.substr(pos+1,sActionSquence.size());
		cout << "---------------\n" << actionStr << endl;
		auto v = split(actionStr,'-');
		for(auto it = v.begin();it != v.end();it++) {
			auto c = (*it)[0];
			if(c == 'X') {
				actions.push_back(Action{check});
			} else if(c == 'R') {
				actions.push_back(Action{raise,stringToNum<float>((*it).substr(1, (*it).size()))});
			} else if(c == 'A') {
				actions.push_back(Action{allin});
			} else if(c == 'C') {
				actions.push_back(Action{call});
			} else if(c == 'O') {
				break;
			} else {
				return false;
			}
		}
	}

	if(pos == string::npos){
		sPrefix = sActionSquence;
	} else {
		sPrefix = sActionSquence.substr(0,pos+1);
	}

	auto count = 0;

	for(auto character:sActionSquence) {
		if(character=='<'){
			count++;
		}
	}

	switch(count) {
		case 0:
			round = preflop;
			break;
		case 1:
			round = flop;
			break;
		case 2:
			round = turn;
			break;
		case 3:
			round = river;
			break;
		default:
			return false;
			break;								
	}
	return true;
}

//从wizard读取
bool CStrategy::Load(GameType gmType, const string& sActionSquence, const Stacks& stacks, const OOPX oopx, const SuitReplace& suitReplace, const string& sIsoBoard)
{
	Round curRound;
	string sPrefix, actionStr, sNodeName;
	vector<Action> actions;
	string sStrategyFilePath;
	string sActionSquenceTmp = sActionSquence;
	bool blAllin2Call = false;
	bool bl2Allin = false;
	bool blCreateStrategy = false;

	//解析需要allin和call转换的标准
	if (sActionSquenceTmp.find('$')!=string::npos) {
		bl2Allin = true;
		sActionSquenceTmp.pop_back();
	}
	if (sActionSquenceTmp.find('@') != string::npos) {
		blAllin2Call = true;
		sActionSquenceTmp.pop_back();
	}
	
	//获取NodeName(ActionLine只在换轮时计算sNodeName,所以flop从wizard读取需要独立计算)
	parseActionSquence(sActionSquenceTmp, sPrefix, curRound, actions, actionStr);
	if (curRound == preflop)
		sNodeName = sPrefix;
	else if (curRound == flop)
		sNodeName = CActionLine::GetNodeName(gmType, stacks, oopx,actions, sPrefix);
	
	//flop需要处理同构，存在同构替换则替换节点名称中的board部分
	if (curRound == flop) {
		if (suitReplace.blIsNeeded) {
			regex reg("<.*>");
			string sReplace = "<" + sIsoBoard + ">";
			sNodeName = regex_replace(sNodeName, reg, sReplace);
		}
	}

	//从策略配置中获取替换和special设置，存在替换则启用替换的名称
	string sReplace = g_strategyNodeConfigs[gmType].GetReplace(sNodeName);
	if (sReplace == "special")
		blCreateStrategy = true;
	else if (!sReplace.empty())
		sNodeName = sReplace;

	//如果策略不是由special指定，则从策略文件中加载
	if (!blCreateStrategy)
	{
		//获取节点名称对应的文件路径，未找到则返回false,代表offline
		sStrategyFilePath = CDataFrom::GetWizardFilePath(gmType, sNodeName);
		if (sStrategyFilePath.size() == 0) {
			cout << "error:GetWizardFilePath() retrun nothing,gmType:" << gmType << ",sNodeName:" << sNodeName << endl;
			return false;
		}

		//for test
		//sStrategyFilePath = "./test/2h2d2c.json"; //for test/////////////////////////////////////////////////

		//加载数据到m_strategy（code X:check,RAI:allin,R*:raise,F:fold,C:call）(betsize:fBetSize,betsize_by_pot:fBetSizeByPot)
		Json::Value root;
		std::ifstream ifs;
		ifs.open(sStrategyFilePath);

		if (ifs.is_open() == false)
			cout << "error:open wizard file failed,path:" << sStrategyFilePath << endl;

		Json::CharReaderBuilder builder;
		//builder["collectComments"] = true;
		JSONCPP_STRING errs;
		if (!parseFromStream(builder, ifs, &root, &errs)) {
			std::cout << errs << std::endl;
			return false;
		}
		else {
			Json::Value solutions = root["solutions"];
			//记录最大的bet,在最后把类型改为allin
			std::shared_ptr<CStrategyItem> maxRaise = nullptr;
			double maxBetSize = 0;
			for (auto it = solutions.begin(); it != solutions.end(); ++it) {
				std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
				auto action = (*it)["action"];
				strategyItem->m_action.actionType = str2ActionType(action["type"].asString());
				strategyItem->m_action.fBetSize = stringToNum<float>(action["betsize"].asString());
				strategyItem->m_action.fBetSizeByPot = stringToNum<float>(action["betsize_by_pot"].asString());
				if (strategyItem->m_action.actionType == raise && strategyItem->m_action.fBetSize > maxBetSize) {
					maxRaise = strategyItem;
				}
				


				auto strategy = (*it)["strategy"];
				if(strategy.size() == 1326) {
					for (Json::ArrayIndex i = 0; i < strategy.size(); i++) {
						auto name = ComboMapping[i];
						auto value = strategy[i].asDouble();
						strategyItem->m_strategyData[name] = value;
						//cout << name << "," << value << endl;
					}
				} else {
					for (Json::ArrayIndex i = 0; i < strategy.size(); i++) {
						auto names = CCombo::GetCombosByAbbr(AbbrComboMapping[i]);    //ComboMapping[i];
						auto value = strategy[i].asDouble();
						for(auto name : names) {
							strategyItem->m_strategyData[name] = value;
						}
						//cout << name << "," << value << endl;
					}	
				}

				auto evs = (*it)["evs"];
				if(evs.size() == 1326) {
					for (Json::ArrayIndex i = 0; i < evs.size(); i++) {
						auto name = ComboMapping[i];
						auto value = evs[i].asDouble();
						strategyItem->m_evData[name] = value;
						//cout << name << "," << value << endl;
					}
				}else {
					for (Json::ArrayIndex i = 0; i < evs.size(); i++) {
						auto names = CCombo::GetCombosByAbbr(AbbrComboMapping[i]); //ComboMapping[i];
						auto value = evs[i].asDouble();
						for(auto name : names) {
							strategyItem->m_strategyData[name] = value;
						}						
						//cout << name << "," << value << endl;
					}
				}

				m_strategy.push_back(strategyItem);
			}
			if (maxRaise != nullptr) {
				maxRaise->m_action.actionType = allin;
			}
		}
	}

#ifdef FOR_TEST_DUMP_DETAIL_
	string sComment = "from_wizard-row" + sNodeName;
	DumpStrategy(sComment);
#endif

	string sSpecial = g_strategyNodeConfigs[gmType].GetSpecial(sNodeName);
	if (!sSpecial.empty()) {
		SpecialProcessing(sSpecial);	//处理special，策略由special指定时需要构建策略，//pStrategyNodeConfigItem->m_sSpecialProcessing

#ifdef FOR_TEST_DUMP_DETAIL_
		cout << "Load strategy from wizard,SpecialProcessing():" << sSpecial << endl;
		sComment = "from_wizard-after_special" + sNodeName;
		DumpStrategy(sComment);
#endif

	}

	//处理allin和call的转换
	if (bl2Allin) {
		sSpecial = "Replace[whole][allin]";
		SpecialProcessing(sSpecial);
	}

	if (blAllin2Call) {
		sSpecial = "Replace[allin][call];Replace[raise][call]";
		SpecialProcessing(sSpecial);
	}


#ifdef FOR_TEST_DUMP_DETAIL_
	if (bl2Allin || blAllin2Call) {
		if(bl2Allin)
			cout << "Load strategy from wizard,2Allin()" << endl;
		else
			cout << "Load strategy from wizard,Allin2Call()" << endl;
		sComment = "from_wizard-after@&" + sNodeName;
		DumpStrategy(sComment);
	}
#endif

	//同构转换
	ConvertIsomorphism(suitReplace);

#ifdef FOR_TEST_DUMP_DETAIL_
	if(suitReplace.blIsNeeded){
		sComment = "from_wizard-after_iso" + sNodeName;
		DumpStrategy(sComment);
	}
#endif

	return true;
}


const Json::Value *CStrategy::geActionNode(const Json::Value *node,const Action& action,const vector<Action>& actions,vector<Action>& actionsByStrategy, const Stacks& stacks, const Stacks& stacksByStrategy){//double stack){//const StackByStrategy& stack,bool last) {
	const Json::Value *next = nullptr; 
	const Json::Value &nodeChildren = (*node)["childrens"];
	auto members = nodeChildren.getMemberNames();
	if(action.actionType == check) {
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if(*it2 == "CHECK") {
				next = &(nodeChildren[*it2]);
				Action a;
				a.actionType = check;
				actionsByStrategy.push_back(a);
				cout << *it2 << endl;
				break;
			}
		}
	} else if(action.actionType == call) {
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if(*it2 == "CALL") {
				next = &(nodeChildren[*it2]);
				Action a;
				a.actionType = call;
				actionsByStrategy.push_back(a);
				cout << *it2 << endl;
				break;
			}
		}
	} else if (action.actionType == allin) {
		/*float  maxBet = 0.0f;
		string maxName = "";
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
				auto bet = getBetByStr(*it2);
				if(bet>=maxBet){
					maxBet=bet;
					maxName=*it2;
					next=&(nodeChildren[*it2]);
				}
			}				
		}
		if(maxName != "") {
			Action a;
			a.actionType = raise;
			a.fBetSize = maxBet;
			actionsByStrategy.push_back(a);
			cout << maxName << endl;
		}*/
		vector<string> names;
		vector<double>  bets;
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
				names.push_back(*it2);
				/////////CalcBetRatio
				auto v = CalcBetRatio(getBetByStr(*it2),actions,int(actions.size()),stacks.dEStack);	
				bets.push_back(v);
			}
		}

		auto i = MatchBetRatio(action.fBetSize,bets);
		if(i>=0){
			next = &(nodeChildren[names[i]]);
			Action a;
			a.actionType = raise;
			a.fBetSize = getBetByStr(names[i]);
			actionsByStrategy.push_back(a);
			cout << names[i] << endl;
		}		
	} else if (action.actionType == raise) {
		vector<string> names;
		vector<double>  bets;
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
				names.push_back(*it2);
				/////////CalcBetRatio
				auto v = CalcBetRatio(getBetByStr(*it2),actionsByStrategy,int(actionsByStrategy.size()),stacksByStrategy.dPot);	
				bets.push_back(v);
			}
		}

		auto i = MatchBetRatio(action.fBetSize,bets);
		if(i>=0){
			next = &(nodeChildren[names[i]]);
			Action a;
			a.actionType = raise;
			a.fBetSize = getBetByStr(names[i]);
			actionsByStrategy.push_back(a);
			cout << names[i] << endl;
		}
	}

	return next;
}


Action CStrategy::getActionByStr(const string &str) {
	Action a = Action{none};
	auto v = split(str,' ');
	if(v.size() > 0 ) {
		a.actionType = str2ActionType(v[0]);
	}

	if(v.size() > 1) {
		a.fBetSize = stringToNum<float>(v[1]);
	}

	return a;
} 

//从solver读取
bool CStrategy::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy, const SuitReplace& suitReplace)
{
	//解析ActionSquence,取最后一个<>后的序列sCSquence
	vector<Action> actionSquence={};
	string sPrefix=""; 
	Round round;
	string actionStr="";
	if(!parseActionSquence(sActionSquence,sPrefix,round,actionSquence,actionStr)) {
		return false;
	}
	if(actionSquence.size()==0 && (actionStr.size()==0 || actionStr[0] != 'O')) {
		cout << actionStr << "," << actionStr[0] << endl;
		return false;
	}
	cout << "size," << actionSquence.size() << "," << sPrefix  << ",round," << round <<endl; 
		
	//sCSquence为“O”，则目标节点为根节点
	//对每个“-”分割的动作sAction，逐层匹配子节点，如果sAction = X,则选择CHECK子节点为目标节点，如果sAction = A,则选择BET(最大值)为目标节点
	//	如果sAction = R？，则所有子节点为BET* 的，将*依次放入vector<double>& candidates，调用MatchBetSize(?,candidates),返回的序号既选择该子节点
	//目标节点后无子节点则返回false
	//加载数据到m_strategy，（action对应： CHECK:check,BET(最大值):allin,BET:raise,FOLD:fold,CALL:call）(BET*:*对应fBetSize,fBetSizeByPot不填)	
	const Json::Value *node = &root;
	vector<Action> actions={};
	vector<Action> actionsByStrategy={};
	int actionSquenceSize = int(actionSquence.size());  
	for(auto i = 0; i < actionSquenceSize;i++) {
		const Json::Value *next = geActionNode(node,actionSquence[i],actions,actionsByStrategy,stacks,stacksByStrategy);
		//cout << actionSquence[i].actionType << "," << next << endl;
		if(next == nullptr) {
			return false;
		}else {
			actions.push_back(actionSquence[i]);
			node = next;
		} 
	}

	const Json::Value &nodeStrategy = (*node)["strategy"];
	const Json::Value &nodeActions = nodeStrategy["actions"];
	//记录最大的bet,在最后把类型改为allin
	std::shared_ptr<CStrategyItem> maxRaise = nullptr;
	double maxBetSize = 0;
	for(Json::ArrayIndex i = 0;i<nodeActions.size();i++){
		std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
		auto a = getActionByStr(nodeActions[i].asString());
		//cout << nodeActions[i].asString() << "," << a.actionType << "," << a.fBetSize << endl;
		/////////CalcBetRatio
		auto iLastIdx = int(actions.size());
		actions.push_back(a);
		auto v = CalcBetRatio(a.fBetSize,actions,iLastIdx,0);
		actions.pop_back();
		strategyItem->m_action.actionType = a.actionType;
		strategyItem->m_action.fBetSize = (float)v;
		if(strategyItem->m_action.actionType==raise && strategyItem->m_action.fBetSize > maxBetSize) {
			maxRaise = strategyItem;
		}
		auto members = nodeStrategy["strategy"].getMemberNames();
		for(auto it = members.begin();it != members.end();++it){
			strategyItem->m_strategyData[*it] = nodeStrategy["strategy"][*it][i].asFloat();
		}
		m_strategy.push_back(strategyItem);
	}
	if(maxRaise != nullptr) {
		maxRaise->m_action.actionType = allin;
	}

#ifdef FOR_TEST_DUMP_DETAIL_
	string sComment = "from_solver-row" + sActionSquence;
	DumpStrategy(sComment);
#endif

	//同构转换
	ConvertIsomorphism(suitReplace);

#ifdef FOR_TEST_DUMP_DETAIL_
	sComment = "from_wizard-after_iso" + sActionSquence;
	DumpStrategy(sComment);
#endif

	return true;
}

string  CStrategy::ConvertOneHandcard(std::string& sCard, const SuitReplace& suitReplace)
{
	string strConveted(sCard);

	for (int i = 1; i <= 3; i += 2)
	{
		switch (sCard[i])
		{
		case 'h':
		{
			strConveted[i] = CCard::CharacterFromsuit(suitReplace.h2);
			break;
		}
		case 'd':
		{
			strConveted[i] = CCard::CharacterFromsuit(suitReplace.d2);
			break;
		}
		case 'c':
		{
			strConveted[i] = CCard::CharacterFromsuit(suitReplace.c2);
			break;

		}
		case 's':
		{
			strConveted[i] = CCard::CharacterFromsuit(suitReplace.s2);
			break;
		}
		default:
		{
			break;
		}
		}
	}
	return strConveted;
}
void CStrategy::ConvertIsomorphismOneMap(std::unordered_map<std::string, double>* pMap, const SuitReplace& suitReplace)
{
	std::unordered_map<std::string, double>::iterator  it_map;
	std::unordered_map<std::string, double> mapTemp;

	for (it_map = pMap->begin(); it_map != pMap->end(); it_map++)
	{
		string key = it_map->first;
		string strKeyIsomor = ConvertOneHandcard(key, suitReplace);
		mapTemp.insert({ strKeyIsomor, it_map->second });
	}
	pMap->clear();
	pMap->insert(mapTemp.begin(), mapTemp.end());
}

void CStrategy::ConvertIsomorphism(const SuitReplace& suitReplace)
{
	std::shared_ptr<CStrategyItem> pItem = NULL;
	if (false == suitReplace.blIsNeeded)
	{
		return;
	}
	for (int i = 0; i < m_strategy.size(); i++)
	{
		pItem = m_strategy[i];
		ConvertIsomorphismOneMap(&pItem->m_strategyData, suitReplace);
		ConvertIsomorphismOneMap(&pItem->m_evData, suitReplace);
	}
}

//返回匹配的序号(用实际比例和候选比例取匹配，将size转为比例通过CalcBetRatio()来计算，需要知道每轮初始pot和动作序列，Estack在allin时替代计算下注总和，solver模式下，策略中raise的size要对应stackByStrategy的pot才能算出比例来填充候选范围)
int CStrategy::MatchBetRatio(double dActuallyRatio, const vector<double>& candidateRatios)
{
	double dLowbound = 0.33; //匹配小的允许超出1/3
	double dHibound = 3;	//差值超过3倍时，允许小的翻倍

	if (candidateRatios.size() == 0)
		return 0;
	if (candidateRatios.size() == 1)
		return 0;
	if (dActuallyRatio <= candidateRatios[0])
		return 0;
	if (dActuallyRatio >= candidateRatios[candidateRatios.size() - 1])
		return (int)candidateRatios.size() - 1;

	int nCount = 0;
	for (auto pos = candidateRatios.cbegin(); pos + 1 != candidateRatios.cend(); ++pos) {
		auto posNext = pos + 1;
		if (dActuallyRatio >= *pos && dActuallyRatio < *posNext) {
			double dDvalue = *posNext - *pos;
			double dAllowExceed = 0;
			if (*posNext > dHibound * (*pos))
				dAllowExceed = *pos;
			else
				dAllowExceed = dLowbound * dDvalue;

			if (dActuallyRatio < *pos + dAllowExceed)
				return nCount;
			else
				return nCount + 1;
		}
		nCount++;
	}

	return 0;
}

//计算下注比例，actions为完整序列（或到参与计算的最后一个）最后一个一定是rival，iLastIdx为当前需要计算的序号（0开始），dEstack参数只在计算allin时有用，因为allin并不带size信息不能累加下注size，这时用有效筹码即下注总和，只有wizard模式计算最后一个为allin的比例时才用
double CStrategy::CalcBetRatio(const double dPot, const vector<Action>& actions, int iLastIdx, const double dEstack)
{
	double dHeroStackSum = 0; double dRivalStackSum = 0;  //计算双方下注总额，用于计算比例
	double dRatio = 0;

	if (dEstack == 0) {
		for (int i = iLastIdx; i >= 0; i -= 2) {
			if (actions[i].actionType == raise) { dRivalStackSum += actions[i].fBetSize; }
		}
	}
	else
		dRivalStackSum = dEstack;	//代表allin的size

	for (int i = iLastIdx - 1; i >= 0; i -= 2)
		if (actions[i].actionType == raise) { dHeroStackSum += actions[i].fBetSize; }

	dRatio = (dRivalStackSum - dHeroStackSum) / (dPot + 2 * dHeroStackSum) * 100;

	return dRatio;
}

//将special文本命令格式化
//string sPara{"Replace[call,allin][raise,fold](AKo,AKs,AA)<EV+0.1>;Discard[call];Discard[raise]<EV-=0>"}; 
vector<CCommForSpecialProcessing> CStrategy::GetCommands(const string& sCommands)
{
	vector<CCommForSpecialProcessing> commands;
	vector<string> rowCommands;
	regex SepCommands(R"(\s?;\s?)");

	sregex_token_iterator p1(sCommands.cbegin(), sCommands.cend(), SepCommands, -1);
	sregex_token_iterator e;
	for (; p1 != e; ++p1)
		rowCommands.push_back(p1->str());

	regex reg;
	smatch m;

	for (auto s : rowCommands) {
		CCommForSpecialProcessing commObj;

		//匹配命令
		reg = R"(^[^[<(]*)";
		regex_search(s, m, reg);
		commObj.m_sCommand = m[0];

		//匹配动作
		reg = R"(\[([^[<(]*)\])";
		if (regex_search(s, m, reg)) {
			string sAction1, sAction2;
			//填写action1
			sAction1 = m[1];

			regex sep(R"(\s?,\s?)");
			sregex_token_iterator p2(sAction1.cbegin(), sAction1.cend(), sep, -1);
			sregex_token_iterator e2;
			for (; p2 != e; ++p2)
				commObj.m_sActions1.push_back(p2->str());


			//填写Action2
			auto pos = m.suffix().first;
			if (regex_search(m.suffix().first, s.cend(), m, reg)) {
				sAction2 = m[1];

				regex sep(R"(\s?,\s?)");
				sregex_token_iterator p2(sAction2.cbegin(), sAction2.cend(), sep, -1);
				sregex_token_iterator e2;
				for (; p2 != e; ++p2)
					commObj.m_sActions2.push_back(p2->str());
			}

			//匹配range
			reg = R"(\(([^[<(]*)\))";
			string sAbbrCombos;
			if (regex_search(s, m, reg)) {

				sAbbrCombos = m[1];

				if (sAbbrCombos.find('-') != string::npos) {
					sAbbrCombos = sAbbrCombos.substr(1, sAbbrCombos.size());
					commObj.m_blRangeExclude = true;
				}
				else
					commObj.m_blRangeExclude = false;

				regex sep(R"(\s?,\s?)");
				sregex_token_iterator p2(sAbbrCombos.cbegin(), sAbbrCombos.cend(), sep, -1);
				sregex_token_iterator e2;
				for (; p2 != e; ++p2) {
					vector<string> v = CCombo::GetCombosByAbbr(p2->str());
					for (auto s : v)
						commObj.m_range.push_back(s);
				}
			}

			//匹配conditions
			reg = R"(\<([^[<(]*)\>)";
			string sConditions;
			if (regex_search(s, m, reg)) {

				sConditions = m[1];

				regex sep(R"(\s?,\s?)");
				sregex_token_iterator p2(sConditions.cbegin(), sConditions.cend(), sep, -1);
				sregex_token_iterator e2;
				for (; p2 != e; ++p2)
					commObj.m_conditions.push_back(p2->str());
			}
		}
		commands.push_back(commObj);

	} //end of for

	return commands;
}


static unordered_map<ActionType,string> getCommandActions(const string &actionStr) {
	unordered_map<ActionType,string> actions;
	if(actionStr == "call") {
		actions[call] = actionStr; 
	}else if(actionStr == "fold"){
		actions[fold] = actionStr; 
	} else if(actionStr == "check"){
		actions[check] = actionStr; 
	} else if(actionStr.find("raise") != string::npos){
		actions[raise] = actionStr; 
	} else if (actionStr == "allin") {
		actions[allin] = actionStr; 
	} else if (actionStr == "whole") {
		actions[call]  = "call"; 
		actions[check] = "check"; 
		actions[raise] = "raise"; 
		actions[allin] = "allin"; 
	}
	return actions;
} 

static void fetchRaiseParam(const string &actionStr,double &num,bool &isRatio) {
	if(actionStr=="raise") {
		return;
	} else if(actionStr[actionStr.size()-1] == '%') {
		isRatio=true;
		num = stringToNum<double>(actionStr.substr(5,actionStr.size()-5-1));
	} else {
		num = stringToNum<double>(actionStr.substr(5,actionStr.size()-5));
	}
}

static bool matchRaise(const Action& action,const string &actionStr) {
	double num = 0;
	bool   isRatio = false;
	fetchRaiseParam(actionStr,num,isRatio);
	if(num == 0) {
		return true;
	}else if(action.fBetSize == num){
		return true;
	} else {
		return false;
	}	
}

void CStrategy::Discard(const string &action,const unordered_map<std::string, bool> &rangeMap,condition *cond) {
	std::shared_ptr<CStrategyItem> itemFold;
	//找到itemFold
	for(auto s : m_strategy) {
		if(s->m_action.actionType == fold) {
			itemFold = s;
		}
	}

	if(itemFold == nullptr) {
		std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
	    strategyItem->m_action.actionType = fold;
		m_strategy.push_back(strategyItem);
		itemFold = strategyItem;
	}


	auto actions = getCommandActions(action);
	for(auto s : m_strategy) {
		auto aIt = actions.find(s->m_action.actionType);
		bool match = true;
		if(aIt == actions.end()) {
			match = false;
		} else {
			if(s->m_action.actionType == raise){
				match = matchRaise(s->m_action,aIt->second); 	
			}
		}
	
		if(match){
			actions.erase(aIt);
			for(auto it = s->m_strategyData.begin();it != s->m_strategyData.end();) {	
				if(rangeMap.find(it->first) != rangeMap.end()) {
					bool bFold = false;
					//判断ev
					auto itEv = s->m_evData.find(it->first);
					if(itEv != s->m_evData.end()) {
						if(nullptr == cond) {
							bFold = true;
						}else{
							if(cond->m_less && itEv->second < cond->m_ev) {
								bFold = true;
							}else if(!cond->m_less && itEv->second > cond->m_ev) {
								bFold = true;
							}
						
						}
					}

					if(bFold) {
						//加到fold下
						auto it3 = itemFold->m_strategyData.find(it->first);
						if(it3 != itemFold->m_strategyData.end()){
							//找到组合把数值加进去
							it3->second += it->second;
						} else {
							itemFold->m_strategyData[it->first] = it->second;
						}
						it = s->m_strategyData.erase(it);


						auto itEv2 = itemFold->m_evData.find(it->first);
						if(itEv2 != itemFold->m_evData.end()){
							itEv2->second += itEv->second;
						} else {
							itemFold->m_evData[it->first] = itEv->second;
						}
						s->m_evData.erase(itEv);
					} else {
						it++;
					}
				}
			}
		}
	}


}

void CStrategy::Assign(const string &action,const unordered_map<string, bool> &rangeMap){
	std::shared_ptr<CStrategyItem> itemFold;
	//找到itemFold
	for(auto s : m_strategy) {
		if(s->m_action.actionType == fold) {
			itemFold = s;
		}
	}

	
	if(itemFold == nullptr) {
		std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
	    strategyItem->m_action.actionType = fold;
		m_strategy.push_back(strategyItem);
		itemFold = strategyItem;
	}

	//Action下仅且只有range指定的组合，而且这些组合100%分配给该动作（在该action中设为1，其他action下设为0，该action下其他组合转为fold，数值加到fold对应的组合下
	//当action下已经存在数据，则需要判断range指定的组合原数据是否为0，非0则设为1，0则忽略不处理。两种情况不需要判断，一是action是allin，二是action不存在数据。
	auto actions = getCommandActions(action);
	for(auto s : m_strategy) {
		auto aIt = actions.find(s->m_action.actionType);
		bool match = true;
		if(aIt == actions.end()) {
			match = false;
		} else {
			if(s->m_action.actionType == raise){
				match = matchRaise(s->m_action,aIt->second); 	
			}
		}
	
		if(match){
			actions.erase(aIt);
			for(auto it = s->m_strategyData.begin();it != s->m_strategyData.end();) {
				auto it2 = rangeMap.find(it->first);	
				if(it2 != rangeMap.end()) {
					//allin不需要判断
					if(s->m_action.actionType != allin) {
						if(it->second > 0){
							//原来非0设为1
							it->second = 1;
						}else {
							//否则忽略
						}
					}
					it++;
				} else if(itemFold != nullptr){
					//其余添加到fold底下,并从当前策略下删除
					auto it3 = itemFold->m_strategyData.find(it->first);
					if(it3 != itemFold->m_strategyData.end()){
						//找到组合把数值加进去
						it3->second += it->second;
					}
					it = s->m_strategyData.erase(it);

					auto itEv1 = s->m_evData.find(it->first);
					if(itEv1 != s->m_evData.end()) {
						auto itEv2 = itemFold->m_evData.find(it->first);
						if(itEv2 != itemFold->m_evData.end()){
							itEv2->second += itEv1->second;
						}
						s->m_evData.erase(itEv1);
					} 
				}
			}
		} else {
			//其它action下设为0
			for(auto it = rangeMap.begin();it != rangeMap.end();it++) {
				auto it2 = s->m_strategyData.find(it->first);
				if(it2 != s->m_strategyData.end()) {
					it2->second = 0;
				}
			}			
		}
	}
	
	//如果策略中没有该action，则添加该action对应的数据集
	for(auto it = actions.begin();it != actions.end();it++){
		std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
		double num = 0;
		bool   isRatio=false;
		fetchRaiseParam(it->second,num,isRatio);
		strategyItem->m_action.actionType = it->first;
		strategyItem->m_action.fBetSize = num;
		for(auto it2 = rangeMap.begin();it2 != rangeMap.end();it2++) {
			strategyItem->m_strategyData[it2->first] = 1;
			//ev怎么办?????
		}
		m_strategy.push_back(strategyItem);
	}
}

static void parseCondition(const string &condStr,condition &cond) {
	if(condStr[2] == '-') {
		cond.m_less = true;
	}
	if(condStr.size() > 4 && condStr[3] == '=') {
		cond.m_ev = stringToNum<double>(condStr.substr(4, condStr.size()));
	}
}

void CStrategy::Replace(const string &action1,const string &action2,const unordered_map<string, bool> &rangeMap,condition *cond)
{
	auto actions1 = getCommandActions(action1);
	auto actions2 = getCommandActions(action2);
	shared_ptr<CStrategyItem> itemAction2 = nullptr;
	for(auto s : m_strategy) {
		if(s->m_action.actionType == actions2.begin()->first) {
			if(s->m_action.actionType == raise){
				if(matchRaise(s->m_action,actions2.begin()->second)){
					itemAction2 = s;
				} 	
			} else {
				itemAction2 = s;
			}
		}
	}
	if(itemAction2 == nullptr) {
		//找不到跟action2对应的
		return;
	}

	for(auto it = actions1.begin();it != actions1.end();it++) {
		shared_ptr<CStrategyItem> itemAction1 = m_strategy[it->first];
		if(itemAction1==nullptr) {
			continue;
		}

		if(itemAction1->m_action.actionType== raise) {
			//匹配raise值
			if(!matchRaise(itemAction1->m_action,it->second)){
				continue;
			}
		}

		for(auto it2 = rangeMap.begin();it2 != rangeMap.end();it2++) {
			auto combo1It = itemAction1->m_strategyData.find(it2->first);
			auto combo2It = itemAction2->m_strategyData.find(it2->first);
			//combo在两个组合中都有
			if(combo1It==itemAction1->m_strategyData.end() || combo2It == itemAction2->m_strategyData.end()) {
				continue;
			}

			if(itemAction2->m_action.actionType != allin && combo2It->second == 0) {
				shared_ptr<CStrategyItem> itemAllin = m_strategy[allin];
				if(itemAllin != nullptr) {
					auto comboAllinIt = itemAllin->m_strategyData.find(it2->first);
					if(comboAllinIt != itemAllin->m_strategyData.end()) {
						if(comboAllinIt->second > 100) {//allin的值大于预设值
							comboAllinIt->second += combo1It->second; //action1的值加到allin上
							combo1It->second = 0;                     //action1值清0
						}
					}
				}
			} else {
				//判断condition,符合条件则转换
				auto replace = false;
				if(cond == nullptr) {
					replace = true;
				} else {
					auto ev =  itemAction2->m_evData[it2->first];
					if(cond->m_less && ev < cond->m_ev) {
						replace = true;
					}else if(!cond->m_less && ev > cond->m_ev) {
						replace = true;
					}
				}

				if(replace) {
					combo2It->second += combo1It->second;
					combo1It->second = 0; 
				}
			}
		}
	}
}

void CStrategy::SpecialProcessing(const std::string& sCommand)
{
	auto commands = GetCommands(sCommand);
	for(auto c : commands) {
		unordered_map<string, bool> rangeMap;
		if (!c.m_blRangeExclude) {
			//将ranges中所有的combo添加到_ranges中
			for(auto r : c.m_range) {
				auto range = CCombo::GetCombosByAbbr(r);
				for(auto v : range) {
					rangeMap[v] = true;
				}
			}
		} else {
			unordered_map<string, bool> RangeExcludeMap;
			for(auto r : c.m_range) {
				auto range = CCombo::GetCombosByAbbr(r);
				for(auto v : range) {
					RangeExcludeMap[v] = true;
				}
			}

			//遍历所有范围，只有不在RangeExcludeMap中的才添加进rangeMap
			for(auto v : ComboMapping) {
				if(RangeExcludeMap.find(v) == RangeExcludeMap.end()) {
					rangeMap[v] = true;
				}
			}
		}

		if(c.m_sCommand == "Assign") {
			for(auto a : c.m_sActions1) {
				Assign(a,rangeMap);
			}
		} else if (c.m_sCommand == "Replace") {

		} else if (c.m_sCommand == "Discard"){
			if(c.m_sCommand.size() == 0) {
				//无条件
				for(auto a : c.m_sActions1) {
					Discard(a,rangeMap,nullptr);
				}
			} else if(c.m_sActions1.size() == c.m_conditions.size()) {
				//每个action有对应的条件
				for(auto i=0;i<int(c.m_sActions1.size());i++) {
					condition cond;
					parseCondition(c.m_conditions[i],cond);
					Discard(c.m_sActions1[i],rangeMap,&cond);
				}
			}
		}
	}
}

extern void loadFileAsLine(const string& path,vector<string> &lines);

void CStrategy::LoadMacro(std::string path) 
{
	vector<string> lines;
	loadFileAsLine(path,lines);
	for(auto l:lines) {
		auto v = split(l,'\t');	
		if(v.size() == 2) {
			m_macro[v[0]] = v[1];
		}
	}
}

void CStrategy::DoMacro(std::string macro) {
	auto it = m_macro.find(macro);
	if(it != m_macro.end()) {
		SpecialProcessing(it->second);
	}
}

void CStrategy::AlignmentByBetsize()
{

}

void CStrategy::AlignmentByStackDepth()
{

}

void CStrategy::AlignmentByexploit()
{

}

void CStrategy::DumpStrategy(const std::string& sComment,  const std::vector<std::shared_ptr<CStrategyItem>>* pStrategy) {
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\dump\\";
	string sCommandsPath = sConfigFolder + "commands.txt";
	time_t t = time(nullptr);
	string sDataPath = sConfigFolder + to_string(t) + ".txt";

	ofstream ofCommands;
	ofCommands.open(sCommandsPath, ofstream::out | ios_base::app);
	if (ofCommands.is_open()) {
		//格式：87969;	rRng; changeRange1;	D:\VCCode\test\ConsoleTestDataManage\ConsoleTestDataManage\data\Rng1.txt
		string sLine = to_string(t) + "; " + "rStr; " + sComment + "; " + sDataPath;
		ofCommands << sLine << endl;
	}
	ofCommands.close();
	
	if (pStrategy == NULL)
		pStrategy = &m_strategy;

	std::ofstream ofs;
	ofs.open(sDataPath,std::ofstream::out);
	if(ofs.is_open()) {
		for(auto s : *pStrategy) {
			ofs << actionType2String(s->m_action.actionType) << "," << s->m_action.fBetSize << "," << s->m_action.fBetSizeByCash << "," << s->m_action.fBetSizeByPot;
			ofs << ":";
			for(auto it = s->m_strategyData.begin();it != s->m_strategyData.end();it++){
				if(it != s->m_strategyData.begin()){
					ofs << ";";
				} 
				ofs<<it->first <<"," << it->second;
			}
			ofs << ":";
			for(auto it = s->m_evData.begin();it != s->m_evData.end();it++){				
				if(it != s->m_evData.begin()){
					ofs << ";";
				} 
				ofs<<it->first <<"," << it->second;
			}
			ofs << "\n";
		}
	}
	ofs.close();
}

//本程序不用，测试程序中用	
void CStrategy::ReadStrategy(const std::string& sPath,  std::vector<std::shared_ptr<CStrategyItem>>& strategy) {
	vector<string> lines;
	loadFileAsLine(sPath,lines);
	for(auto l:lines) {
		auto blocks = split(l,':');
		auto action = split(blocks[0],',');
		shared_ptr<CStrategyItem> item(new CStrategyItem);
		item->m_action.actionType = str2ActionType(action[0]);
		item->m_action.fBetSize = stringToNum<float>(action[1]);
		item->m_action.fBetSizeByCash = stringToNum<float>(action[2]);
		item->m_action.fBetSizeByPot = stringToNum<float>(action[3]);
		auto strategyDatas = split(blocks[1],';');
		for(auto s : strategyDatas) {
			auto strategyData = split(s,',');
			item->m_strategyData[strategyData[0]] = stringToNum<double>(strategyData[1]);
		}

		if(blocks[2]!="") {
			auto evDatas = split(blocks[2],';');
			for(auto ev : evDatas) {
				auto evData = split(ev,',');
				item->m_evData[evData[0]] = stringToNum<double>(evData[1]);
			}
		}

		strategy.push_back(item);
	}
}