//#include "pch.h"
#include "CStrategy.h"
#include "util.h"
#include <fstream>
#include <iostream>

using namespace std;

//从wizard读取
bool CStrategy::Load(GameType gmType, const string& sNodeName, const SuitReplace& suitReplace, const string& sIsoBoard)
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

	return true;
}

//从solver读取
bool CStrategy::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const SuitReplace& suitReplace)
{
	//按sActionSquence逐层定位到需要选择的节点，无候选则返回false,代表offline
	// 
	//存在多个选择，则按候选betsize的匹配选择节点
	// 
	//加载数据到m_strategy
	// 
	//同构转换

	return true;
}

void CStrategy::ConvertIsomorphism(const SuitReplace& suitReplace)
{

}

void CStrategy::SpecialProcessing()
{

}

double CStrategy::MatchBetSize(double fActually, const set<double>& candidates)
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