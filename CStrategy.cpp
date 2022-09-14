//#include "pch.h"
#include "CStrategy.h"
#include "util.h"
#include <fstream>
#include <iostream>
#include <regex>
#include "CStrategyNodeConfig.h"
#include "CStrategyNodeConfigItem.h"
#include "CStrategyTreeConfig.h"
#include "CDataFrom.h"


using namespace std;
extern map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
extern CDataFrom g_dataFrom;

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
		sPrefix = sActionSquence.substr(1,pos);
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

//生成wizard用的NodeName （未测试）
string CStrategy::getNodeName(const GameType gmType, const StackByStrategy& stack, const vector<Action>& actions, const string& sPrefix)
{
	string sSquence, sPreflopName;
	OOPX oopx = OOPA;
	Json::Value StrategyTreeItem;
	int nCount = 0;	//当前处理的actions的序号，1开始
	double dActuallyRatio = 0;	//实际下注比例
	double dHeroStackSum, dRivalStackSum = 0;	//hero和对手总筹码投入
	double dEstack = 0;	//有效筹码，为实际有效筹码，非策略对应筹码
	vector<double> candidateRatios;	//比例候选集合，对应策略数设置

	if (actions.empty()) {	//无代表本街hero第一个行动
		sSquence = sPrefix + "O";
		return sSquence;
	}

	auto idx = sPrefix.find('<');
	if (idx != string::npos)
		sPreflopName = sPrefix.substr(0, idx);

	//get oopx
	auto pStrategyNodeConfigItem = g_strategyNodeConfigs[gmType].GetItemByName(sPreflopName);
	if (pStrategyNodeConfigItem != nullptr) {
		oopx = pStrategyNodeConfigItem->m_oopx;
	}

	//get strategy tree config
	Json::Value strategyTreeItem = g_strategyTreeConfigs[gmType].GetConfigItem(sPreflopName);

	for (auto action : actions) {
		nCount++;

		switch (action.actionType) {	//（只有X,R,A），X对应check, A对应allin
			case check:
				nCount > 1 ? sSquence += "-X" : sSquence += "X";
				break;
			case allin:
				nCount > 1 ? sSquence += "-A" : sSquence += "A";
				break;
			case raise:{
		
				candidateRatios.clear();
				dEstack = 0;
				if (nCount == int(actions.size())) {	//R最后一个需要判断是否会转为allin，将size转为比例，MatchBetRatio中EStack非0则会判断是否转allin
					dEstack = stack.fEStack;
				}

				//选择比例候选范围
				if (nCount == 1) {	//R第一个需要判断OOPA/OOPD以决定用donk还是bet下注空间
					if (oopx == OOPD) {
						//strategyTreeItem里取donk下注空间，测试donk下注空间用{33} 
						candidateRatios = { 33 }; //for test///////////////////////////////////
					}
					else if (oopx == OOPA) {
						//strategyTreeItem里取bet下注空间，测试donk下注空间用{33,50,75,125}
						candidateRatios = { 33,50,75,125 };  //for test///////////////////////////////////
					}
				}
				else {
					if (nCount == 2 && actions[0].actionType == check) {
						//如果是XR，strategyTreeItem里取bet下注空间, 测试下注空间用{33,50,75,125}
						candidateRatios = { 33,50,75,125 };  //for test///////////////////////////////////
					}
					else {
						//strategyTreeItem里取raise下注空间, 测试下注空间用{50,100}
						candidateRatios = { 50,100 };  //for test///////////////////////////////////
					}
				}

				//计算dActuallyRatio
				dHeroStackSum = 0; dRivalStackSum = 0; //计算双方下注总额，用于计算比例
				for (int i = nCount - 1; i >= 0; i -= 2 ) {
					if (actions[i].actionType == raise) { dRivalStackSum += actions[i].fBetSize; }
				}
				for (int i = nCount - 2; i >= 0; i -= 2) {
					if (actions[i].actionType == raise) { dHeroStackSum += actions[i].fBetSize; }
				}
				dActuallyRatio = (dRivalStackSum - dHeroStackSum) / (stack.fPot + 2 * dHeroStackSum) * 100;

				int iMatchedIndex = MatchBetRatio(dActuallyRatio, candidateRatios, dEstack);

				if(iMatchedIndex == -1)	//转为allin
					nCount > 1 ? sSquence = "-A" : sSquence = "A";
				else
					nCount > 1 ? sSquence = "-R" + to_string(candidateRatios[iMatchedIndex]) : sSquence = "R" + to_string(candidateRatios[iMatchedIndex]);

			} //end of case raise
			default:
			break;
		} //end of switch
	}// end of for

	sSquence = sPrefix + sSquence;
	return sSquence;
}

//从wizard读取
bool CStrategy::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const SuitReplace& suitReplace, const string& sIsoBoard)
{
	Round curRound;
	string sPrefix, actionStr, sNodeName;
	vector<Action> actions;
	string sStrategyFilePath;
	
	//获取NodeName
	parseActionSquence(sActionSquence, sPrefix, curRound, actions, actionStr);
	if (curRound == preflop)
		sNodeName = sPrefix;
	else if (curRound == flop)
		sNodeName = getNodeName(gmType, stack, actions, sPrefix);	
	
	//flop需要处理同构，存在同构替换则替换节点名称中的board部分
	if (curRound == flop) {
		if (suitReplace.blIsNeeded) {
			regex reg("<.*>");
			string sReplace = "<" + sIsoBoard + ">";
			sNodeName = regex_replace(sNodeName, reg, sReplace);
		}
	}

	//从策略配置中获取替换和special设置，存在替换则启用替换的名称，(flop用通配匹配法配置)
	auto StrategyNodeConfig = g_strategyNodeConfigs[gmType];
	auto pStrategyNodeConfigItem = StrategyNodeConfig.GetItemByName(sNodeName);
	if (pStrategyNodeConfigItem != nullptr) {
		if (!pStrategyNodeConfigItem->m_sReplaceNodeName.empty())
			sNodeName = pStrategyNodeConfigItem->m_sReplaceNodeName;
	}

	//获取节点名称对应的文件路径，未找到则返回false,代表offline
	sStrategyFilePath = g_dataFrom.GetWizardFilePath(gmType,  sNodeName, curRound);
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
  	} else {
  		Json::Value solutions = root["solutions"];
		//记录最大的bet,在最后把类型改为allin
		std::shared_ptr<CStrategyItem> maxRaise = nullptr;
		double maxBetSize = 0; 
		for(auto it = solutions.begin();it != solutions.end();++it){
			std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
			auto action = (*it)["action"];
			strategyItem->m_action.actionType = str2ActionType(action["type"].asString());
			strategyItem->m_action.fBetSize = stringToNum<float>(action["betsize"].asString());
			strategyItem->m_action.fBetSizeByPot = stringToNum<float>(action["betsize_by_pot"].asString());
			if(strategyItem->m_action.actionType==raise && strategyItem->m_action.fBetSize > maxBetSize) {
				maxRaise = strategyItem;
			}
			auto strategy = (*it)["strategy"];
			for(Json::ArrayIndex i = 0;i<strategy.size();i++){
				auto name = ComboMapping[i];
				auto value = strategy[i].asDouble();
				strategyItem->m_strategyData[name] = value;
				cout << name << "," << value << endl;
			}

			auto evs = (*it)["evs"];
			for(Json::ArrayIndex i = 0;i<evs.size();i++){
				auto name = ComboMapping[i];
				auto value = evs[i].asDouble();
				strategyItem->m_evData[name] = value;
				cout << name << "," << value << endl;
			}
			m_strategy.push_back(strategyItem);
		}
		if(maxRaise != nullptr) {
			maxRaise->m_action.actionType = allin;
		}
  	} 
	//处理special

	//同构转换

	return true;
}


const Json::Value *CStrategy::geActionNode(const Json::Value *node,const Action& action,double stack){//const StackByStrategy& stack,bool last) {
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
				bets.push_back(getBetByStr(*it2));
			}
		}

		auto i = MatchBetSize(action.fBetSize,bets,stack);
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
bool CStrategy::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const SuitReplace& suitReplace)
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
	int actionSquenceSize = int(actionSquence.size());  
	for(auto i = 0; i < actionSquenceSize;i++) {
		double sstack = 0;
		if(i==actionSquenceSize-1) {
			sstack = stack.fEStack;
		}

		const Json::Value *next = geActionNode(node,actionSquence[i],sstack);

		//cout << actionSquence[i].actionType << "," << next << endl;

		if(next == nullptr) {
			return false;
		}else {
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
		strategyItem->m_action.actionType = a.actionType;
		strategyItem->m_action.fBetSize = a.fBetSize;
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

void CStrategy::ConvertIsomorphism(const SuitReplace& suitReplace)
{

}

void CStrategy::SpecialProcessing()
{
	
}

//返回匹配的序号，当R为最后一个动作时，dEStatck非0则需要匹配allin，返回正常序号,最后一个代表allin
int CStrategy::MatchBetSize(double dActuallySize, const vector<double>& candidateSizes, const double dEStatck)
{
	for(auto i = 0;i<int(candidateSizes.size());i++) {
		if(abs(dActuallySize-candidateSizes[i]) < 0.1){
			return i;
		}
	}
	return -1;
}

//返回匹配的序号，当R为最后一个动作时，dEStatck非0则需要匹配allin, allin则返回-1,
int CStrategy::MatchBetRatio(double dActuallyRatio, const vector<double>& candidateRatios, const double dEStatck)
{
	return 0;
}

void CStrategy::AlignmentByBetsize(float fBase, float fActually)
{

}

void CStrategy::AlignmentByStackDepth(float fBase, float fActually)
{

}

void CStrategy::AlignmentByexploit()
{

}