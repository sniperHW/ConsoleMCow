//#include "pch.h"
#include "CRange.h"
#include "CStrategy.h"
#include "CDataFrom.h"
using namespace std;
extern CDataFrom g_dataFrom;
#include <regex>
#include "CRangeNodeConfig.h"
#include "CRangeNodeConfigItem.h"
#include "CStrategyTreeConfig.h"

//file模式
bool CRange::Load(GameType gmType, const string& sNodeName, const string& sBoardNext)
{
	//获取节点名称对应的文件路径，未找到则返回false,代表offline
	string sPath = g_dataFrom.GetRangesFilePath(gmType, sNodeName);
	if (sPath.size() == 0)
		return false;

	//从范围文件加载数据，OOP和IP对应

	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//wizard模式
bool CRange::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace, const string& sIsoBoard)
{
/*
	Round curRound;
	string sPrefix, actionStr, sNodeName;
	vector<Action> actions;
	string sStrategyFilePath;
	string sActionSquenceTmp;
	RangeData rangeRatio;
	string sLastPlayerPosition;
	RangeData* pRangeRatio = nullptr;

	//获取NodeName(注释）
	//去掉sActionSquence中最后一个动作(C或X)
	auto idx = sActionSquence.find_last_of('-');
	if (idx != string::npos)
		sActionSquenceTmp = sActionSquence.substr(0, idx);

	parseActionSquence(sActionSquenceTmp, sPrefix, curRound, actions, actionStr);
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
	auto RangeNodeConfig = g_rangeNodeConfigs[gmType];
	auto pRangeNodeConfigItem = RangeNodeConfig.GetItemByName(sNodeName);
	if (pRangeNodeConfigItem != nullptr) {
		if (!pRangeNodeConfigItem->m_sReplaceNodeName.empty())
			sNodeName = pRangeNodeConfigItem->m_sReplaceNodeName;
	}

	//获取节点名称对应的文件路径，未找到则返回false,代表offline
	sStrategyFilePath = g_dataFrom.GetWizardFilePath(gmType, sNodeName, curRound);
	if(sStrategyFilePath.size() == 0)
		return false;
	*/

	//解析range(注释）
	//clear(); //先清空原始range（不需要按原始range来计算）
	//solution下找出action.code = "X"或"C"(忽略大小写)的[n]，rangeRatio = solution.[n].strategy；sLastPlayerPosition = solution.[n].action.position
	//如果player_info[0].player.relative_postflop_position == "OOP"
		//m_OOPRange = player_info[0].player.range ；m_IPRange = player_info[1].player.range
	 //else m_OOPRange = player_info[1].player.range; m_IPRange = player_info[0].player.range

	//对最后行动的玩家范围结合C或X的策略进行运算(注释）
	//如果player_info[0].player.position == sLastPlayerPosition
		//如果 player_info[0].player.relative_postflop_position == "OOP" 则 pRangeRatio = &m_OOPRange else pRangeRatio = &m_IPRange
	//else
		//如果 player_info[1].player.relative_postflop_position == "OOP" 则 pRangeRatio = &m_OOPRange else pRangeRatio = &m_IPRange

	//rangeRatio和pRangeRatio指向的范围运算，更新范围

	//同构转换
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(m_OOPRange, suitReplace);
		ConvertIsomorphism(m_IPRange, suitReplace);
	}

	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//solver模式
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace)
{
	RangeData OOPRangeRatio; //用于计算range剩余比例，OOP代表第一个行动的玩家
	RangeData IPRangeRatio; //用于计算range剩余比例，IP代表第二个行动的玩家
	RangeData* pRangeRatio = nullptr;
	//用ComboMapping的每一个组合初始化OOPRangeRatio和IPRangeRatio，初始数值为0


	//解析ActionSquence,取最后一个<>后的序列sCSquence
	vector<Action> actionSquence={};
	string sPrefix=""; 
	Round round;
	string actionStr="";
	if(!CStrategy::parseActionSquence(sActionSquence,sPrefix,round,actionSquence,actionStr)) {
		return false;
	}
	if(actionSquence.size()==0) {
		return false;
	}

	//对actionSquence中每一个动作（只有X，R，C，其中X只会出现在第一个，最后一个一定是C或X，没有A和O）
		//确定是操作OOP还是IP（奇数是OOP，偶数是IP，赋值pRangeRatio）
		//当前节点（第一个为root）下选择对应的动作，和策略中方法一样，但R不需要匹配A
		//对策略数据中每个组合，用选定动作对应的比例去改写pRangeRatio中对应的数据（如果是第一次行动则填写1*比例，非第一次行动则填写*pRangeRatio[组合] * 比例，区分第一次目的是让不存在的组合保持0）
		//不是最后一个动作则按之前匹配的动作选择子节点，为当前节点

	const Json::Value *node = &root;
	for(auto i = 0;i<int(actionSquence.size());i++){
		if(i%2==0) {
			pRangeRatio = &OOPRangeRatio;
		} else {
			pRangeRatio = &IPRangeRatio;		
		}

		auto a = actionSquence[i];

		const Json::Value &nodeStrategy = (*node)["strategy"];
		const Json::Value &nodeActions = nodeStrategy["actions"];
		Json::ArrayIndex j = 0;
		for(;j<nodeActions.size();j++){
			auto aa = CStrategy::getActionByStr(nodeActions[j].asString());
			if(aa.actionType == a.actionType) {
				break;
			}
		}
		if(j==nodeActions.size()) {
			return false;
		}

		auto members = nodeStrategy["strategy"].getMemberNames();
		for(auto it = members.begin();it != members.end();++it){
			auto name = *it;
			auto value = nodeStrategy["strategy"][name][j].asFloat();

			auto mapIt = pRangeRatio->find(name);
			if(mapIt == pRangeRatio->end()) {
				//第一次1*value
				(*pRangeRatio)[name] = value;
			} else {
				mapIt->second *= value;
			}
		}

		double sstack = 0;
		if(i==int(actionSquence.size()-1)) {
			sstack = stack.fEStack;
		}

		//根据动作选择下一节点
		node = CStrategy::geActionNode(node,a,sstack);
		if(node==nullptr){
			return false;
		}
	}


	//同构转换
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(OOPRangeRatio, suitReplace);
		ConvertIsomorphism(IPRangeRatio, suitReplace);
	}

	//和原始范围运算,更新原始范围（每个组合原始范围数据 *RangeRatio对应的比例, 能否一个transform函数加multiplies）

	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(m_IPRange, sBoardNext);
	RemoveComboByBoard(m_OOPRange, sBoardNext);

	return true;
}

//转为solver配置文件需要的格式
string CRange::GetRangeStr()
{
	//参照file模式里的格式
	string v;
	return v;
}

//清空双方范围，置为0
void CRange::Clear()
{

}
//同构转换
void CRange::ConvertIsomorphism(RangeData& rangeRatio, const SuitReplace& suitReplace)
{
	
}

//排除公牌相关的组合
void CRange::RemoveComboByBoard(RangeData& rangeRatio, const std::string& sBoardNext)
{

}
