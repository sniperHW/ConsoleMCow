//#include "pch.h"
#include "CStrategy.h"
#include "util.h"
#include <fstream>
#include <iostream>

using namespace std;

//从wizard读取
bool CStrategy::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const SuitReplace& suitReplace, const string& sIsoBoard)
{
	//sNodeName为完整名称，解析出当前round，和flop当前街名称
	// 
	//flop需要处理同构，存在同构替换则替换节点名称中的board部分

	//flop需要匹配替换最后一个betsize,修改节点名称

	//从策略配置中获取替换和special设置，存在替换则启用替换的名称，turn用通配匹配法

	//获取节点名称对应的文件路径，未找到则返回false,代表offline

	//加载数据到m_strategy（code X:check,RAI:allin,R*:raise,F:fold,C:call）(betsize:fBetSize,betsize_by_pot:fBetSizeByPot)
	Json::Value root;
  	std::ifstream ifs;
  	ifs.open("./test/2h2d2c.json");

  	Json::CharReaderBuilder builder;
  	//builder["collectComments"] = true;
  	JSONCPP_STRING errs;
  	if (!parseFromStream(builder, ifs, &root, &errs)) {
    	std::cout << errs << std::endl;
    	return false;
  	} else {
  		Json::Value solutions = root["solutions"];
		for(auto it = solutions.begin();it != solutions.end();++it){
			std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
			auto action = (*it)["action"];
			strategyItem->m_action.actionType = str2ActionType(action["type"].asString());
			strategyItem->m_action.fBetSize = stringToNum<float>(action["betsize"].asString());
			strategyItem->m_action.fBetSizeByPot = stringToNum<float>(action["betsize_by_pot"].asString());
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
  	} 
	//处理special

	//同构转换

	//flop后更新sNodeName

	return true;
}

/*
ActionSquence格式：例：BTN_vsUTG_srp<KsQsTh>X-R16-A，动作范围为O(代表第一个行动),X,R,A，R后跟betsize,可能的组合如下：
O
R
RR
RRR
RRRR
RRRRA
A
RA
RRA
RRRA
X
XR
XRR
XRRR
XRRRR
XRRRRA
XA
XRA
XRRA
XRRRA
R
*/
//从solver读取
bool CStrategy::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const SuitReplace& suitReplace)
{
	//解析ActionSquence,取最后一个<>后的序列sCSquence
	vector<Action> actionSquence;
	auto pos = sActionSquence.find('>');
	if(pos == string::npos) {
		//找不到>
		return false;
	}else if(pos == sActionSquence.size()-1) {
		return false;
	} else {
		auto str = sActionSquence.substr(pos+1,sActionSquence.size());
		cout << "---------------\n" << str << endl;
		auto v = split(str,'-');
		for(auto it = v.begin();it != v.end();it++) {
			auto c = (*it)[0];
			if(c == 'X') {
				actionSquence.push_back(Action{check});
			} else if(c == 'R') {
				actionSquence.push_back(Action{raise,stringToNum<float>((*it).substr(1, (*it).size()))});
			} else if(c == 'A') {
				actionSquence.push_back(Action{allin});
			} else if(c == 'O') {
				break;
			} else {
				return false;
			}
		}
	}

	cout << "size," << actionSquence.size() << endl; 
	auto getBetByStr = [](const string &str)->float{ 
		auto v = split(str,' ');
		if(v.size()==2){
			return stringToNum<float>(v[1]);
		} else {
			return 0;
		}
    }; 

	auto getActionByStr = [] (const string &str) -> Action {
		Action a = Action{none};
		auto v = split(str,' ');
		if(v.size() > 0 ) {
			a.actionType = str2ActionType(v[0]);
		}

		if(v.size() > 1) {
			a.fBetSize = stringToNum<float>(v[1]);
		}

		return a;
	};
		
	//sCSquence为“O”，则目标节点为根节点
	//对每个“-”分割的动作sAction，逐层匹配子节点，如果sAction = X,则选择CHECK子节点为目标节点，如果sAction = A,则选择BET(最大值)为目标节点
	//	如果sAction = R？，则所有子节点为BET* 的，将*依次放入vector<double>& candidates，调用MatchBetSize(?,candidates),返回的序号既选择该子节点
	//目标节点后无子节点则返回false
	//加载数据到m_strategy，（action对应： CHECK:check,BET(最大值):allin,BET:raise,FOLD:fold,CALL:call）(BET*:*对应fBetSize,fBetSizeByPot不填)

	auto MatchBetSize = [](float bet,vector<float>  &bets)->int {
		for(auto i = 0;i<int(bets.size());i++) {
			if(abs(bet-bets[i]) < 0.1){
				return i;
			}
		}
		return -1;
	};
	
	const Json::Value *node = &root;
	for(auto it = actionSquence.begin();it != actionSquence.end();it++) {
		const Json::Value *next; 
		auto members = (*node)["childrens"].getMemberNames();
		if(it->actionType == check) {
			for(auto it2 = members.begin();it2 != members.end();++it2){
				if(*it2 == "CHECK") {
					next = &((*node)["childrens"][*it2]);
					cout << *it2 << endl;
					break;
				}
			}
		} else if (it->actionType == allin) {
			float  maxBet = 0.0f;
			string maxName = "";
			for(auto it2 = members.begin();it2 != members.end();++it2){
				if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
					auto bet = getBetByStr(*it2);
					if(bet>=maxBet){
						maxBet=bet;
						maxName=*it2;
						next=&((*node)["childrens"][*it2]);
					}
				}				
			}
			if(maxName != "") {
				cout << maxName << endl;
			}
		} else if (it->actionType == raise) {
			vector<string> names;
			vector<float>  bets;
			for(auto it2 = members.begin();it2 != members.end();++it2){
				if((*it2).find("BET") != string::npos || (*it2).find("RAISE") != string::npos) {
					names.push_back(*it2);
					bets.push_back(getBetByStr(*it2));
				}
			}
			auto i = MatchBetSize(it->fBetSize,bets);
			if(i>=0){
				next = &((*node)["childrens"][names[i]]);
				cout << names[i] << endl;
			}
		}

		if(next == nullptr){
			return false;
		}else{
			node = next;
		}
	}

	Json::Value nodeStrategy = (*node)["strategy"];
	Json::Value nodeActions = nodeStrategy["actions"];
	for(Json::ArrayIndex i = 0;i<nodeActions.size();i++){
		std::shared_ptr<CStrategyItem> strategyItem(new CStrategyItem);
		auto a = getActionByStr(nodeActions[i].asString());
		strategyItem->m_action.actionType = a.actionType;
		strategyItem->m_action.fBetSize = a.fBetSize;
		auto members = nodeStrategy["strategy"].getMemberNames();
		for(auto it = members.begin();it != members.end();++it){
			strategyItem->m_strategyData[*it] = nodeStrategy["strategy"][*it][i].asFloat();
		}
		m_strategy.push_back(strategyItem);
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

//按实际下注bb数，匹配子节下注空间，用于sover解计算，参数都为实际size，返回为匹配的序号
int CStrategy::MatchBetSize(double dActuallySize, const vector<double>& candidateSizes, GameType gmType, const StackByStrategy& stack)
{
	return 0;
}

//按实际下注比例，匹配子节下注空间，用于wizard解计算，需要先将size转为比例，候选在策略树设置中，返回为匹配的序号
int  CStrategy::MatchBetRatio(double dActuallyRatio, const vector<double>& candidateRatios, GameType gmType, const StackByStrategy& stack)
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