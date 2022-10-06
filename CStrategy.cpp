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
	bool blAllin2Call, bl2Allin = false;
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
	
	//获取NodeName
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
		if (sStrategyFilePath.size() == 0)
			return false;

		sStrategyFilePath = "./test/2h2d2c.json"; //for test/////////////////////////////////////////////////

		//加载数据到m_strategy（code X:check,RAI:allin,R*:raise,F:fold,C:call）(betsize:fBetSize,betsize_by_pot:fBetSizeByPot)
		Json::Value root;
		std::ifstream ifs;
		ifs.open(sStrategyFilePath);

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
				for (Json::ArrayIndex i = 0; i < strategy.size(); i++) {
					auto name = ComboMapping[i];
					auto value = strategy[i].asDouble();
					strategyItem->m_strategyData[name] = value;
					cout << name << "," << value << endl;
				}

				auto evs = (*it)["evs"];
				for (Json::ArrayIndex i = 0; i < evs.size(); i++) {
					auto name = ComboMapping[i];
					auto value = evs[i].asDouble();
					strategyItem->m_evData[name] = value;
					cout << name << "," << value << endl;
				}
				m_strategy.push_back(strategyItem);
			}
			if (maxRaise != nullptr) {
				maxRaise->m_action.actionType = allin;
			}
		}
	}

	string sSpecial = g_strategyNodeConfigs[gmType].GetSpecial(sNodeName);
	if (!sSpecial.empty()) {
		//处理special，策略由special指定时需要构建策略，//pStrategyNodeConfigItem->m_sSpecialProcessing

	}

	//处理allin和call的转换

	//同构转换

	return true;
}


const Json::Value *CStrategy::geActionNode(const Json::Value *node,const Action& action,const vector<Action>& actions,double stack){//const StackByStrategy& stack,bool last) {
	const Json::Value *next = nullptr; 
	const Json::Value &nodeChildren = (*node)["childrens"];
	auto members = nodeChildren.getMemberNames();
	if(action.actionType == check) {
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if(*it2 == "CHECK") {
				next = &(nodeChildren[*it2]);
				cout << *it2 << endl;
				break;
			}
		}
	} else if(action.actionType == call) {
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if(*it2 == "CALL") {
				next = &(nodeChildren[*it2]);
				cout << *it2 << endl;
				break;
			}
		}
	} else if (action.actionType == allin) {
		float  maxBet = 0.0f;
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
			cout << maxName << endl;
		}
	} else if (action.actionType == raise) {
		vector<string> names;
		vector<double>  bets;
		for(auto it2 = members.begin();it2 != members.end();++it2){
			if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
				names.push_back(*it2);
				/////////CalcBetRatio
				auto v = CalcBetRatio(getBetByStr(*it2),actions,int(actions.size()),stack);	
				bets.push_back(v);
			}
		}

		auto i = MatchBetRatio(action.fBetSize,bets);
		if(i>=0){
			next = &(nodeChildren[names[i]]);
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
	int actionSquenceSize = int(actionSquence.size());  
	for(auto i = 0; i < actionSquenceSize;i++) {
		double sstack = 0;
		if(i==actionSquenceSize-1) {
			sstack = stacks.dEStack;
		}
		const Json::Value *next = geActionNode(node,actionSquence[i],actions,sstack);
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
	//同构转换

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

void CStrategy::SpecialProcessing(const std::string& sCommand)
{
	
}

//返回匹配的序号
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

//计算下注比例，actions为完整序列（或到参与计算的最后一个），iLastIdx为当前需要计算的序号（0开始），dEstack只有wizard模式计算最后一个为allin的比例时才用
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
		dRivalStackSum = dEstack;

	for (int i = iLastIdx - 1; i >= 0; i -= 2)
		if (actions[i].actionType == raise) { dHeroStackSum += actions[i].fBetSize; }

	dRatio = (dRivalStackSum - dHeroStackSum) / (dPot + 2 * dHeroStackSum) * 100;

	return dRatio;
}

//将special文本命令格式化
//for test :string sPara{"Replace[call,allin][raise,fold](AKo,AKs,AA)<EV+0.1>;Discard[call];Discard[raise]<EV-=0>"}; 
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

void CStrategy::AlignmentByBetsize()
{

}

void CStrategy::AlignmentByStackDepth()
{

}

void CStrategy::AlignmentByexploit()
{

}