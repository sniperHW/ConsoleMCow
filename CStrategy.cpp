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


std::vector<Action> parseActionSquence(const string& sActionSquence) {
	vector<Action> ret; 
	for(auto i = sActionSquence.size()-1;i >=0;i--) {
		if(sActionSquence[i] == '>'){
			auto v = split(sActionSquence.substr(i+1,sActionSquence.size()),'-');
			for(auto it = v.begin();it != v.end();it++) {
				if((*it).size() > 0) {
					switch((*it)[0]) {
						case 'X':
							ret.push_back(Action{check});
						break;
						case 'R':{
							Action action;
							action.actionType = raise;
							action.fBetSize = stringToNum<float>((*it).substr(1, (*it).size()));
							ret.push_back(action);
							}
						break;
						case 'B':{
							Action action;
							action.actionType = bet;
							action.fBetSize = stringToNum<float>((*it).substr(1, (*it).size()));
							ret.push_back(action);
						}
						break;						
						case 'A':
							ret.push_back(Action{allin});
						break;
						case 'F':
							ret.push_back(Action{fold});
						break;
						case 'C':
							ret.push_back(Action{call});
						break;																		
						case 'O':
						ret.push_back(Action{none});
						break;
					}
				} 	
			}
			break;
		}
	}
	return ret;
}

float getBet(const string &str) {
	auto v = split(str,' ');
	if(v.size()==2){
		return stringToNum<float>(v[1]);
	} else {
		return 0;
	}
}

Action getActionByStr(const string &str) {
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

const Json::Value* getActionNode(const Json::Value& node,const Action &action) {
	cout << "getActionNode " << action.actionType << endl; 
	if(action.actionType == none){
		return &node;
	} else {
		auto members = node["childrens"].getMemberNames();
		string maxName = "";
		float  maxBet = 0.0f;
		for(auto it = members.begin();it != members.end();++it){
			switch(action.actionType) {
				case check:
					if(*it=="CHECK"){
						cout << *it << endl;
 						return &(node["childrens"][*it]);
					}
				break;
				case fold:
					if(*it=="FOLD"){
						cout << *it << endl;
 						return &(node["childrens"][*it]);
					}
				break;
				case call:
					if(*it=="CALL"){
						cout << *it << endl;
 						return &(node["childrens"][*it]);
					}
				break;				
				case allin:
				case raise:
				case bet:
					if((*it).find("BET") != string::npos || (*it).find("RAISE") != string::npos) {
						auto bet = getBet(*it);
						if(action.actionType==allin) {
							if(bet>=maxBet){
								maxBet=bet;
								maxName=*it;
							}
						} else {
							if(abs(action.fBetSize-bet) < 0.1){
								cout << *it << endl;
								return &(node["childrens"][*it]); 
							}
						} 
					}
				break;
				default:
				break;
			}	
		}

		if(maxName != "") {
			cout << maxName << endl;
			return &(node["childrens"][maxName]); 
		}
	}

	return nullptr;
}

void CStrategy::load(const Json::Value& node,std::vector<Action> &actions,int pos) {
	//cout << "load:" << pos << "," << actions[pos].actionType << "," << actions[pos].fBetSize <<endl ;
	if(pos == int(actions.size())) {
		//加载strategy
		Json::Value nodeStrategy = node["strategy"];
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
	} else {
		auto next = getActionNode(node,actions[pos]); 
		if(next != nullptr) {
			load(*next,actions,pos+1);
		} else {
			cout << "nullptr" << endl;
		}
	}
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
	auto actions = parseActionSquence(sActionSquence);
	//sCSquence为“O”，则目标节点为根节点
	//对每个“-”分割的动作sAction，逐层匹配子节点，如果sAction = X,则选择CHECK子节点为目标节点，如果sAction = A,则选择BET(最大值)为目标节点
	//	如果sAction = R？，则所有子节点为BET* 的，将*依次放入vector<double>& candidates，调用MatchBetSize(?,candidates),返回的序号既选择该子节点
	//目标节点后无子节点则返回false
	//加载数据到m_strategy，（action对应： CHECK:check,BET(最大值):allin,BET:raise,FOLD:fold,CALL:call）(BET*:*对应fBetSize,fBetSizeByPot不填)
	load(root,actions,0);
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