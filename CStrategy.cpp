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
#include<algorithm>
#include<unordered_set>
#include "CSpecialProcessingMacroConfig.h"

using namespace std;
extern map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
extern map<GameType, CSpecialProcessingMacroConfig> g_specialProcessingMacroConfig;


bool CStrategy::parseActionSquence(const string& sActionSquence, string& sPrefix, Round &round,vector<Action>& actions,string &actionStr) {
	//查找最后的>
	auto pos = sActionSquence.rfind('>');
	if(pos != string::npos && pos != sActionSquence.size()-1) {
		actionStr = sActionSquence.substr(pos+1,sActionSquence.size());
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

//从自定义策略文件读取(preflop全部用该模式)
bool CStrategy::Load(GameType gmType, const std::string& sActionSquence)
{
	bool blAllin2Call = false;
	bool bl2Allin = false;
	bool blCreateStrategy = false;
	string sNodeName = sActionSquence;

	//解析需要allin和call转换的标准
	if (sNodeName.find('$') != string::npos) {
		bl2Allin = true;
		sNodeName.pop_back();
	}
	if (sNodeName.find('@') != string::npos) {
		blAllin2Call = true;
		sNodeName.pop_back();
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
		string sStrategyFilePath = CDataFrom::GetStrategyFilePath(gmType, sNodeName);
		if (sStrategyFilePath.size() == 0) {
			cout << "error:GetStrategyFilePath() retrun nothing,gmType:" << gmType << ",sNodeName:" << sNodeName << endl;
			return false;
		}

		//加载数据到m_strategy
		if (ReadStrategy(sStrategyFilePath, m_strategy) == false){
			cout << "error:open strategy file failed,path:" << sStrategyFilePath << endl;
			return false;
		}
	}

#ifdef FOR_TEST_DUMP_DETAIL_
	string sComment = "from_strategy-row" + sNodeName;
	DumpStrategy(sComment);
#endif

	string sSpecial = g_strategyNodeConfigs[gmType].GetSpecial(sNodeName);
	if (!sSpecial.empty()) {
		SpecialProcessing(gmType,sSpecial);	//处理special，策略由special指定时需要构建策略，//pStrategyNodeConfigItem->m_sSpecialProcessing

#ifdef FOR_TEST_DUMP_DETAIL_
		cout << "Load strategy from strategyFile,SpecialProcessing():" << sSpecial << endl;
		sComment = "from_strategy-after_special" + sNodeName;
		DumpStrategy(sComment);
#endif

	}

	//处理allin和call的转换
	if (bl2Allin) {
		sSpecial = "$";
		SpecialProcessing(gmType,sSpecial);
	}

	if (blAllin2Call) {
		sSpecial = "@";
		SpecialProcessing(gmType,sSpecial);
	}


#ifdef FOR_TEST_DUMP_DETAIL_
	if (bl2Allin || blAllin2Call) {
		if (bl2Allin)
			cout << "Load strategy from strategyFile,2Allin()" << endl;
		else
			cout << "Load strategy from strategyFile,Allin2Call()" << endl;
		sComment = "from_strategyFile-after@&" + sNodeName;
		DumpStrategy(sComment);
	}
#endif

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
			regex reg("<......>");
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

#ifdef DEBUG
	cout << "Replace:" << sReplace << endl;
#endif // DEBUG


	//如果策略不是由special指定，则从策略文件中加载
	if (!blCreateStrategy)
	{
		//获取节点名称对应的文件路径，未找到则返回false,代表offline
		sStrategyFilePath = CDataFrom::GetWizardFilePath(gmType, sNodeName);
		if (sStrategyFilePath.size() == 0) {
			cout << "error:GetWizardFilePath() retrun nothing,gmType:" << gmType << ",sNodeName:" << sNodeName << endl;
			return false;
		}


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

			for (auto it = solutions.begin(); it != solutions.end(); ++it) {
				std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
				auto action = (*it)["action"];
				strategyItem->m_action.actionType = str2ActionType(action["type"].asString(), action["code"].asString());
				strategyItem->m_action.fBetSize = stringToNum<float>(action["betsize"].asString());
				strategyItem->m_action.fBetSizeByPot = stringToNum<float>(action["betsize_by_pot"].asString());
				
				auto strategy = (*it)["strategy"];
				if(strategy.size() == 1326) {
					for (Json::ArrayIndex i = 0; i < strategy.size(); i++) {
						auto name = ComboMapping[i];
						auto value = strategy[i].asDouble();
						strategyItem->m_strategyData[name] = value;
						//cout << name << "," << value << endl;
					}
				} 
				else if (strategy.size() == 169) {	//169(与编号对应不上，目前不用)
					for (Json::ArrayIndex i = 0; i < strategy.size(); i++) {
						auto names = CCombo::GetCombosByAbbr(AbbrComboMapping[i]);    //ComboMapping[i];
						auto value = strategy[i].asDouble();
						for(auto name : names) 
							strategyItem->m_strategyData[name] = value;
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
							strategyItem->m_evData[name] = value;
						}
					}
				}
				m_strategy.push_back(strategyItem);
			}
		}
	}


	string sComment;
//#ifdef FOR_TEST_DUMP_DETAIL_
//	string sComment = "from_wizard-row" + sNodeName;
//	DumpStrategy(sComment);
//#endif

	string sSpecial = g_strategyNodeConfigs[gmType].GetSpecial(sNodeName);
	if (!sSpecial.empty()) {
		SpecialProcessing(gmType,sSpecial);	//处理special，策略由special指定时需要构建策略，//pStrategyNodeConfigItem->m_sSpecialProcessing

//#ifdef FOR_TEST_DUMP_DETAIL_
//		cout << "Load strategy from wizard,SpecialProcessing():" << sSpecial << endl;
//		sComment = "from_wizard-after_special" + sNodeName;
//		DumpStrategy(sComment);
//#endif

	}

	//处理allin和call的转换
	if (bl2Allin) {
		sSpecial = "$";
		SpecialProcessing(gmType,sSpecial);
	}

	if (blAllin2Call) {
		sSpecial = "@";
		SpecialProcessing(gmType,sSpecial);
	}

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

//如果实际行动中对方动作是allin那一定是选择BET最大值（代表allin）,这情况下是不用比例去匹配的，策略中也要把ActionType设为allin
//既然已经allin，也就无需跟踪下一步了
//原来讲的allin要计算比例仅指对候选来说，allin也要作为一种raise来供选择，并不是对实际动作序列里allin的处理，是因为wizard解allin（对手raise可能很大接近allin而当对手allin处理）
//，是不知道size的，所以要有效筹码替代，但solver中allin也是用bet表示，有size,所以不需要特别处理
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
					next=&(nodeChildren[*it2]);		//？这之后策略中的是否会把ActionType设为allin
				}
			}				
		}
	} else if (action.actionType == raise) {
		vector<string> names;
		vector<double> bets;
		vector<Action> actionsByStrategyTmp = actionsByStrategy;	//因为要拿候选bet节点逐个替换进去计算
		for(auto it2 = members.begin();it2 != members.end();++it2){	
			if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
				//加入当前要计算的子节点
				Action actionCur;	
				actionCur.actionType = raise;
				actionCur.fBetSize = getBetByStr(*it2);
				actionsByStrategyTmp.push_back(actionCur);

				names.push_back(*it2);
				/////////CalcBetRatio计算该节点的比例
				auto v = CalcBetRatio(stacksByStrategy.dPot, actionsByStrategyTmp,int(actionsByStrategyTmp.size()-1));
				actionsByStrategyTmp.pop_back();
				bets.push_back(v);	//候选比例
			}
		}

		//计算实际比例
		double dActuallyRatio = CalcBetRatio(stacks.dPot, actions, int(actions.size()-1)); //只计算到当前一个
		//匹配比例
		auto i = MatchBetRatio(dActuallyRatio,bets);

		if(i>=0){
			next = &(nodeChildren[names[i]]);	
			Action a;
			a.actionType = raise;
			a.fBetSize = getBetByStr(names[i]);
			actionsByStrategy.push_back(a);
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
		
	//sCSquence为“O”，则目标节点为根节点
	//对每个“-”分割的动作sAction，逐层匹配子节点，如果sAction = X,则选择CHECK子节点为目标节点，如果sAction = A,则选择BET(最大值)为目标节点
	//	如果sAction = R？，则所有子节点为BET* 的，将*依次放入vector<double>& candidates，调用MatchBetSize(?,candidates),返回的序号既选择该子节点
	//目标节点后无子节点则返回false
	//加载数据到m_strategy，（action对应： CHECK:check,BET(最大值):allin,BET:raise,FOLD:fold,CALL:call）(BET*:*对应fBetSize,fBetSizeByPot不填)	
	const Json::Value *node = &root;

	vector<Action> actions={};
	vector<Action> actionsByStrategy={};
	int actionSquenceSize = int(actionSquence.size());  
	//定位到目标节点
	for(auto i = 0; i < actionSquenceSize;i++) {	
		actions.push_back(actionSquence[i]);
		const Json::Value *next = geActionNode(node,actionSquence[i],actions,actionsByStrategy,stacks,stacksByStrategy);
		if(next == nullptr) 
			return false;
		else
			node = next;
	}

	//获取目标节点下的策略数据
	const Json::Value &nodeStrategy = (*node)["strategy"];
	const Json::Value &nodeActions = nodeStrategy["actions"];
	//记录最大的bet,在最后把类型改为allin
	std::shared_ptr<CStrategyItem> maxRaise = nullptr;
	double maxBetSize = 0;
	for(Json::ArrayIndex i = 0;i<nodeActions.size();i++){
		std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
		auto a = getActionByStr(nodeActions[i].asString());

		strategyItem->m_action.actionType = a.actionType;
		if (strategyItem->m_action.actionType == raise) {
			//计算bet节点比例
			actionsByStrategy.push_back(a);
			auto v = CalcBetRatio(stacksByStrategy.dPot, actionsByStrategy, (int)actionsByStrategy.size()-1)/100;
			actionsByStrategy.pop_back();	//计算完需要用下个节点的替代
			strategyItem->m_action.fBetSize = a.fBetSize;
			strategyItem->m_action.fBetSizeByPot = (float)v;
			//记忆最大bet节点
			if (strategyItem->m_action.actionType == raise && strategyItem->m_action.fBetSize > maxBetSize) {
				maxRaise = strategyItem;
				maxBetSize = strategyItem->m_action.fBetSize;
			}
		}
		auto members = nodeStrategy["strategy"].getMemberNames();
		for (auto it = members.begin(); it != members.end(); ++it) 
			strategyItem->m_strategyData[CCombo::Align(* it)] = nodeStrategy["strategy"][*it][i].asFloat();

		m_strategy.push_back(strategyItem);
	}
	//将最大bet节点改为allin
	if(maxRaise != nullptr) {
		maxRaise->m_action.actionType = allin;
	}

	//同构转换
	ConvertIsomorphism(suitReplace);



#ifdef FOR_TEST_DUMP_DETAIL_
	string sComment = "from_solver-after_iso" + sActionSquence;
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
	return CCombo::Align(strConveted);
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

//解析多人格式，和2人不同（allin如果小于R则当C），而且是解析整条当前街（不是增量）(Open时O没有)
vector<pair<Position, Action>> CStrategy::parseMultiActionSquence(const string& sOriActionInfo)
{
	vector<pair<Position, Action>> posActions;
	string sOriActionInfoTmp = sOriActionInfo;
	auto pos = sOriActionInfo.rfind('>');
	if (pos != string::npos) 
		sOriActionInfoTmp = sOriActionInfo.substr(pos + 1, sOriActionInfo.size());

	if (sOriActionInfoTmp.empty())
		return posActions;

	regex sep(R"(\s?,\s?)");
	sregex_token_iterator p(sOriActionInfoTmp.cbegin(), sOriActionInfoTmp.cend(), sep, -1);
	sregex_token_iterator e;
	float fLastRaiseSize = 0;
	for (; p != e; ++p) {
		if (!p->str().empty()) {
			string sSymbol = p->str();
			Action a{ none,0,0,0 };
			regex regAction(R"(\]([FRCAX])(.*))");
			Position p;
			regex regPosition(R"(\[(.*)\])");
			smatch m;

			if (regex_search(sSymbol, m, regPosition)) 
				p = CGame::GetPositionBySymbol(m[1]);

			if (regex_search(sSymbol, m, regAction)) {
				if (m[1].str() == "X") {
					a.actionType = check;
				}
				else if (m[1].str() == "R") {
					a.actionType = raise;
					a.fBetSize = stof(m[2].str());
					if (a.fBetSize > fLastRaiseSize)
						fLastRaiseSize = a.fBetSize;
				}
				else if (m[1].str() == "A") {
					a.actionType = allin;
					a.fBetSize = stof(m[2].str());
					if (a.fBetSize > fLastRaiseSize)
						fLastRaiseSize = a.fBetSize;
					if(a.fBetSize < fLastRaiseSize) //allin如果小于R则当C
						a.actionType = call;
				}
				else if (m[1].str() == "C") {
					a.actionType = call;
				}
				else if (m[1].str() == "F") {
					a.actionType = fold;
				}
			}

			posActions.push_back(make_pair(p, a));
		}
	}

	return posActions;
}

//计算多人下注比例（为最后一个R的比例），公式为：下注额-与最近R的size的差值/下注前底池总和+与最近R的size的差值
//注：当计算hero下注比例时最后call的不能省略
double CStrategy::CalcMultiBetRatio(const double dPot, const vector<pair<Position, Action>>& posActions)
{
	double ret = 0;
	vector<pair<Position, Action>> posActionsTmp = posActions;

	for (int i = (int)posActionsTmp.size() - 1; i >= 0; i--) { //去掉末尾call的
		if (posActionsTmp[i].second.actionType != raise && posActionsTmp[i].second.actionType != allin) {
			posActionsTmp.pop_back();
		}
		else
			break;
	}

	if (posActionsTmp.empty())
		return 0;

	//记录位置对应的累计下注额
	map<Position, double> posSizes;
	double dRecentRSize = 0;
	double dPotBeforeBet = dPot; //除最后一次下注外的底池总和
	for (int i = 0; i < posActionsTmp.size(); i++) {
		if (posActionsTmp[i].second.actionType == raise || posActionsTmp[i].second.actionType == allin) {
			dRecentRSize = posActionsTmp[i].second.fBetSize;
			posSizes[posActionsTmp[i].first] += posActionsTmp[i].second.fBetSize;
			if(i != posActionsTmp.size() - 1)
				dPotBeforeBet += posActionsTmp[i].second.fBetSize;
		}
		else if (posActionsTmp[i].second.actionType == call) {
			posSizes[posActionsTmp[i].first] += dRecentRSize;
			if (i != posActionsTmp.size() - 1)
				dPotBeforeBet += dRecentRSize;
		}
	}

	//最后R的位置
	Position posLastR = posActionsTmp.back().first;

	//下注额（最后R）
	double dBetSize = posActionsTmp[posActionsTmp.size() - 1].second.fBetSize;

	//与最近R的size的差值(最后一次不计)
	double dLastRSizeDif = 0;
	Position posBeforeLastR;
	for (auto p = posActionsTmp.rbegin() + 1; p != posActionsTmp.rend(); p++) {
		if (p->second.actionType == raise || p->second.actionType == allin) {
			posBeforeLastR = p->first;
			break;
		}
	}
	dLastRSizeDif = posSizes[posBeforeLastR] - posSizes[posLastR] + dBetSize;

	ret = (dBetSize - dLastRSizeDif) / (dPotBeforeBet + dLastRSizeDif);

	if (ret == 0)
		cout << "error: CalcMultiBetRatio can't be 0." << endl;

	return ret;
}

//按下注比例计算具体size,2人，多人调用同一个,最后一个为对手行动,公式为：size = (pot+差额)*比例+差额 (size在客户端精确到int)
double CStrategy::CalcBetSize(const double dPot, const std::vector<std::pair<Position, Action>>& posActions, const Position heroPosition, const double dSizeByPot)
{
	//记录位置对应的累计下注额
	map<Position, double> posSizes;
	double dRecentRSize = 0;
	double dPotTotal = dPot; 
	for (int i = 0; i < posActions.size(); i++) {
		if (posActions[i].second.actionType == raise || posActions[i].second.actionType == allin) {
			dRecentRSize = posActions[i].second.fBetSize;
			posSizes[posActions[i].first] += posActions[i].second.fBetSize;
			dPotTotal += posActions[i].second.fBetSize;
		}
		else if (posActions[i].second.actionType == call) {
			posSizes[posActions[i].first] += dRecentRSize;
			dPotTotal += dRecentRSize;
		}
	}

	//最后R的位置
	Position posLastR;
	auto p = posActions.rbegin();
	while (p != posActions.rend()) {
		if (p->second.actionType == raise || p->second.actionType == allin) {
			posLastR = p->first;
			break;
		}
		else
			p++;
	}

	//差额
	double dDif = posSizes[posLastR] - posSizes[heroPosition];
	
	//size = (pot+差额)*比例+差额
	double dRet = (dPotTotal + dDif) * dSizeByPot + dDif;

	return dRet;
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
		num = -1;	//用于填写betSizeByPot,代表通配
		return;
	} else if(actionStr[actionStr.size()-1] == '%') {
		isRatio=true;
		num = stringToNum<double>(actionStr.substr(5,actionStr.size()-5-1))/(double)100;
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
	}else if(fabs(action.fBetSize - num) < 0.0001){
		return true;
	} else {
		return false;
	}	
}

//格式：对象 +-= 数值
static Condition parseCondition(const string& condStr) {
	Condition condition{ "",false,false,0 };
	if (!condStr.empty()) {
		regex reg(R"(([^+\-=]+)([+\-=]+)([.\d]*))"); //-加转义，否则表示范围
		smatch m;
		if (regex_search(condStr, m, reg)) {
			condition.sObj = m[1];
			string sComp = m[2];
			if (sComp.find('-') != string::npos)
				condition.blLess = true;
			if (sComp.find('=') != string::npos)
				condition.blEqueal = true;
			string sValue = m[3];
			if (!sValue.empty())
				condition.dlValue = stod(sValue);
		}
		else
			cout << "error:special condition format error" << endl;
	}

	return condition;
}

static bool meetEVCondition(const double dlEV, const Condition& condition) 
{
	if (condition.blLess && condition.blEqueal)
		return dlEV <= condition.dlValue;
	else if (condition.blLess && !condition.blEqueal)
		return dlEV < condition.dlValue;
	else if (!condition.blLess && condition.blEqueal)
		return dlEV >= condition.dlValue;
	else if (!condition.blLess && !condition.blEqueal)
		return dlEV > condition.dlValue;
	else
		return false;

}

static void getOPActions(const string& rowActionStr, vector<Action>& opActions, bool& blAllActions)
{
	blAllActions = false;
	unordered_map<ActionType, string> actions;
	if (rowActionStr == "whole")
		blAllActions = true;
	else {
		auto actions = getCommandActions(rowActionStr);
		for (auto it : actions) {
			Action a{ fold,0,0,0 };
			double num = 0;
			bool isRatio = false;
			if (it.first == raise)
				fetchRaiseParam(it.second, num, isRatio);
			a.actionType = it.first;
			if (a.actionType == raise) {
				if (fabs(num + 1) < 0.00001)
					a.fBetSizeByCash = -1;
				else {
					if (isRatio)
						a.fBetSizeByPot = (float)num;
					else
						a.fBetSize = (float)num;
				}
			}
			opActions.push_back(a);
		}
	}
}

void CStrategy::DiscardOne(shared_ptr<CStrategyItem>& strategyItem, shared_ptr<CStrategyItem>& foldItem, const unordered_map<std::string, bool>& rangeMap, const Condition& condition)
{
	bool blAllRange = rangeMap.empty() ? true : false;
	for (auto pStrategy = strategyItem->m_strategyData.begin(); pStrategy != strategyItem->m_strategyData.end(); pStrategy++) {	//对策略里的每个组合，在范围中且符合条件的转为fold
		if (blAllRange || rangeMap.find(pStrategy->first) != rangeMap.end()) {	//范围中存在
			//判断是否满足condition
			bool blMeetCondition = false;
			if (condition.sObj == "")
				blMeetCondition = true;
			else if (condition.sObj == "EV") {
				auto pEV = strategyItem->m_evData.find(pStrategy->first);
				if (pEV != strategyItem->m_evData.end()) 
					blMeetCondition = meetEVCondition(pEV->second, condition);
				else
					cout << "error:EV condition not found" << endl;
			}
			else {
				//其他obj的实现（目前只有EV）
				cout << "error:not support other condition obj" << endl;
			}

			//符合条件则转换
			if (blMeetCondition) { 
				auto p = foldItem->m_strategyData.find(pStrategy->first);
				if (p != foldItem->m_strategyData.end()) 
					p->second += pStrategy->second;
				else
					foldItem->m_strategyData[pStrategy->first] = 1;

				pStrategy->second = 0;
			}
		}
	}
}

void CStrategy::Discard(const string &action,const unordered_map<std::string, bool> &rangeMap, const vector<Condition>& conditions)
{
	//转为Action结构
	bool blAllActions = false;
	vector<Action> discardActions;	
	getOPActions(action, discardActions, blAllActions);

	//需要discard的策略
	vector<std::shared_ptr<CStrategyItem>> discardStrategys;
	for (auto it : m_strategy) {
		if (blAllActions || find(discardActions.begin(), discardActions.end(), it->m_action) != discardActions.end()) {
			if (it->m_action.actionType != fold)
				discardStrategys.push_back(it);
		}
	}

	if (discardStrategys.empty()) {
		cout << "error:no discard strategy matched" << endl;
		return;
	}

	//没有fold则加入
	shared_ptr<CStrategyItem> pFold = nullptr;
	for (auto it : m_strategy) {
		if (it->m_action.actionType == fold) {
			pFold = it;
			break;
		}
	}

	if (!pFold) { 
		pFold = make_shared<CStrategyItem>();
		pFold->m_action = {fold,0,0,0};
		for (auto it : ComboMapping)
			pFold->m_strategyData[it] = 0;
		m_strategy.push_back(pFold);
	}


	//对符合的策略，按条件和范围，清0并将数值加到fold下
	for (auto strategy : discardStrategys) {
		if (conditions.empty()) {
			Condition emptyCondition{ "",false,false,0 };
			DiscardOne(strategy, pFold, rangeMap, emptyCondition);
		}
		else {
			for (auto condition : conditions)
				DiscardOne(strategy, pFold, rangeMap, condition);
		}
	}
}

void CStrategy::Assign(const string &action,const unordered_map<string, bool> &rangeMap)
{
	if (rangeMap.size() == 0) {	//必须指定范围
		cout << "error:assign range empty" << endl;
		return;
	}

	//转为Action结构
	bool blAllActions = false;
	Action actionToAssign;
	Action actionFold{ fold,0,0,0 };
	vector<Action> opActions;
	getOPActions(action, opActions, blAllActions);

	if (opActions.size() != 1) {	//只会有一个动作，不能是whole
		cout << "error:can't assign multiply actions" << endl;
		return;
	}

	actionToAssign = opActions[0];

	if (actionToAssign.actionType == fold) {
		cout << "error:can't assign fold" << endl;
		return;
	}

	
	if (m_strategy.size() == 0) {	//原先为空策略则构建策略
		shared_ptr<CStrategyItem> strategyItemAssign(new CStrategyItem);
		shared_ptr<CStrategyItem> strategyItemFold(new CStrategyItem);
		strategyItemAssign->m_action = actionToAssign;
		strategyItemFold->m_action = actionFold;

		for (auto it : ComboMapping) {
			auto p = rangeMap.find(it);
			if (p != rangeMap.end()) {
				strategyItemAssign->m_strategyData[it] = 1;
				strategyItemFold->m_strategyData[it] = 0;
			}
			else {
				strategyItemAssign->m_strategyData[it] = 0;
				strategyItemFold->m_strategyData[it] = 1;
			}
		}
		m_strategy.push_back(strategyItemAssign);
		m_strategy.push_back(strategyItemFold);
	}
	else {	//原先存在策略
		shared_ptr<CStrategyItem> pMatch = nullptr;
		for (auto it: m_strategy) {
			if (it->m_action == actionToAssign) {
				pMatch = it;
				break;
			}
		}

		if (!pMatch) {	//原先没有Assign的action则加入，对应的combo其他action下变为0
			shared_ptr<CStrategyItem> strategyItemAssign(new CStrategyItem);
			strategyItemAssign->m_action = actionToAssign;
			for (auto it : ComboMapping) { //指定的设为1
				auto p = rangeMap.find(it);
				if (p != rangeMap.end()) 
					strategyItemAssign->m_strategyData[it] = 1;
				else
					strategyItemAssign->m_strategyData[it] = 0;
			}

			for (auto it : rangeMap) {	//其他action的设为0
				for (auto strategyIt : m_strategy) {
					auto p = strategyIt->m_strategyData.find(it.first);
					if (p != strategyIt->m_strategyData.end())
						p->second = 0;
				}
			}

			m_strategy.push_back(strategyItemAssign);
		}
		else {	//原先存在action,先清空，对范围中符合条件的加入(其他动作下设为0)，范围外的转为fold
			CStrategyItem matchedOld= *pMatch;

			//清空
			for (auto p = pMatch->m_strategyData.begin(); p != pMatch->m_strategyData.end(); p++) 
				p->second = 0;

			//对范围中符合条件的加入
			for (auto it : rangeMap) { 
				bool blReplaceCondition = false;
				if (actionToAssign.actionType == allin)
					blReplaceCondition = true;
				else {
					auto p = matchedOld.m_strategyData.find(it.first);
					if (p != matchedOld.m_strategyData.end()) {
						if (p->second > 0.000001)
							blReplaceCondition = true;
					}
				}

				if (blReplaceCondition) {	//符合条件的，先清空所有的，再把指定的设为1
					for (auto strategyIt : m_strategy) {
						auto p = strategyIt->m_strategyData.find(it.first);
						if (p != strategyIt->m_strategyData.end())
							p->second = 0;
					}
					pMatch->m_strategyData[it.first] = 1;
				}
			}

			//范围外的转为fold
			shared_ptr<CStrategyItem> pFold = nullptr;
			for (auto it : m_strategy) {
				if (it->m_action.actionType == fold) {
					pFold = it;
					break;
				}
			}

			if (!pFold) { //没有fold则加入
				pFold = make_shared<CStrategyItem>();
				pFold->m_action = actionFold;
				for (auto it : ComboMapping) 
					pFold->m_strategyData[it] = 0;
				m_strategy.push_back(pFold);
			}

			for (auto it : matchedOld.m_strategyData) { //指定范围找不到的因为之前设为0，所以要加到fold上
				auto p = rangeMap.find(it.first);
				if (p == rangeMap.end()) {
					if(pFold->m_strategyData.find(it.first)!= pFold->m_strategyData.end())
						pFold->m_strategyData[it.first] += it.second;
					else
						pFold->m_strategyData[it.first] = it.second;
				}
			}
		}
	}
}

void CStrategy::ReplaceOne(shared_ptr<CStrategyItem>& srcStrategy, shared_ptr<CStrategyItem>& desStrategy, shared_ptr<CStrategyItem>& allinStrategy, const unordered_map<std::string, bool>& rangeMap, const Condition& condition)
{
	bool blAllRange = rangeMap.empty() ? true : false;
	for (auto pSrcData = srcStrategy->m_strategyData.begin(); pSrcData != srcStrategy->m_strategyData.end(); pSrcData++) {	//对策略里的每个组合，在范围中且符合条件的转为目标，目标不存在则判断是否转allin
		if (blAllRange || rangeMap.find(pSrcData->first) != rangeMap.end()) {	//范围中存在
			//判断是否满足condition
			bool blMeetCondition = false;
			if (condition.sObj == "")
				blMeetCondition = true;
			else if (condition.sObj == "EV") {
				auto pEV = desStrategy->m_evData.find(pSrcData->first);
				if (pEV != desStrategy->m_evData.end())
					blMeetCondition = meetEVCondition(pEV->second, condition);
				else
					cout << "error:EV condition not found" << endl;
			}
			else {
				//其他obj的实现（目前只有EV）
				cout << "error:replace not support other condition obj" << endl;
			}

			//符合条件则转换
			if (blMeetCondition) {
				bool blDesExist = false;
				auto p = desStrategy->m_strategyData.find(pSrcData->first); //目标非0
				if (p != desStrategy->m_strategyData.end()) {
					if (p->second > 0.000001) {
						p->second += pSrcData->second;
						pSrcData->second = 0;
						blDesExist = true;
					}
				}

				if (!blDesExist) {	//目标不存在则判断是否转为allin
					if (allinStrategy) {	//allin是否存在
						auto p = allinStrategy->m_strategyData.find(pSrcData->first); 
						if (p != allinStrategy->m_strategyData.end()) {
							auto pAllinEV = allinStrategy->m_evData.find(pSrcData->first);
							if (pAllinEV != allinStrategy->m_evData.end()) {
								if (pAllinEV->second >= MIN_ALLIN_EV && p->second > MIN_ALLIN_RATIO) {	//allin EV大于预设值并且存在allin
									p->second += pSrcData->second;
									pSrcData->second = 0;
								}
							}
						}
					}
				}
			}
		}
	}
}

void CStrategy::Replace(const string &action1,const string &action2,const unordered_map<string, bool> &rangeMap, const vector<Condition>& conditions)
{
	//转为Action结构
	bool blAllActions = false;
	bool blTmp; //只作为参数，不被使用
	Action actionDes;
	Action actionFold{ fold,0,0,0 };
	vector<Action> srcActions,desActions;

	getOPActions(action1, srcActions, blAllActions);
	getOPActions(action2, desActions, blTmp);

	if (desActions.size() != 1) {
		cout << "error:Replace destination not 1 action" << endl;
		return;
	}

	actionDes = desActions[0];

	if (actionDes.actionType == fold) {
		cout << "error:Can't replace to fold" << endl;
		return;
	}

	//需要被replace的策略
	bool blIncludeAllin = false;	//如果allin要被替换，则当目标策略不存在时不转为allin
	vector<std::shared_ptr<CStrategyItem>> srcStrategys;
	for (auto it : m_strategy) {
		if (blAllActions || find(srcActions.begin(), srcActions.end(), it->m_action) != srcActions.end())
			if (it->m_action.actionType != fold && !(it->m_action == actionDes)) { //排除fold和目标
				srcStrategys.push_back(it);
				if (it->m_action.actionType == allin)
					blIncludeAllin = true;
			}
	}
	if (srcStrategys.empty())
		return;

	//replace的目标策略
	shared_ptr<CStrategyItem > desStrategy = nullptr;
	for (auto it : m_strategy) {
		if (it->m_action == actionDes) {
			desStrategy = it;
			break;
		}
	}

	if (!desStrategy) {	//没有目标策略则加入
		desStrategy = make_shared<CStrategyItem>();
		desStrategy->m_action = actionDes;
		for (auto it : ComboMapping)
			desStrategy->m_strategyData[it] = 0;
		m_strategy.push_back(desStrategy);
	}

	//allin策略
	Action allinAction{ allin,0,0,0 };
	shared_ptr<CStrategyItem > allinStrategy = nullptr;
	if (!blIncludeAllin) {	//当被替换包含allin时保持nullptr
		for (auto it : m_strategy) {
			if (it->m_action == allinAction) {
				allinStrategy = it;
				break;
			}
		}
	}

	//对符合的策略，按条件和范围，将源策略转为目标策略
	for (auto strategy : srcStrategys) {
		if (conditions.empty()) {
			Condition emptyCondition{ "",false,false,0 }; //无条件转换
			ReplaceOne(strategy, desStrategy, allinStrategy, rangeMap, emptyCondition);
		}
		else {
			for (auto condition : conditions)
				ReplaceOne(strategy, desStrategy, allinStrategy, rangeMap, condition);
		}
	}

}

void CStrategy::SpecialProcessing(const GameType gmType, const std::string& sCommand)
{
	string sCommandTmp = sCommand;
	//匹配宏指令
	string sActually  = g_specialProcessingMacroConfig[gmType].GetActuallyCommand(sCommand);
	if (!sActually.empty())
		sCommandTmp = sActually;

	//处理指令
	auto commands = GetCommands(sCommandTmp);
	for(auto c : commands) {
		unordered_map<string, bool> rangeMap;
		for(auto r : c.m_range) 		//将ranges中所有的combo添加到_ranges中
			rangeMap[r] = true;

		if(c.m_sCommand == "Assign") {
			if (c.m_sActions1.size() != 1) {
				cout << "error:Assign not 1 action" << endl;
				break;
			}
			Assign(c.m_sActions1[0],rangeMap);
		} 
		else if (c.m_sCommand == "Replace") {
			if (c.m_sActions1.size() != 1 || c.m_sActions2.size() != 1) {
				cout << "error:Replace not 1 action" << endl;
				break;
			}

			vector<Condition> conditions;
			for (auto it : c.m_conditions)
				conditions.push_back(parseCondition(it));

			Replace(c.m_sActions1[0], c.m_sActions2[0], rangeMap, conditions);
		} 
		else if (c.m_sCommand == "Discard") {
			vector<Condition> conditions;
			for (auto it : c.m_conditions)
				conditions.push_back(parseCondition(it));

			for (auto a : c.m_sActions1) 
				Discard(a, rangeMap, conditions);
		}
	}
}

extern bool loadFileAsLine(const string& path,vector<string> &lines);


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
	t += rand();
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

//
bool CStrategy::ReadStrategy(const std::string& sPath,  std::vector<std::shared_ptr<CStrategyItem>>& strategy) {
	vector<string> lines;
	if (loadFileAsLine(sPath, lines) == false)
		return false;
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

	return true;
}