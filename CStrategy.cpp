//#include "pch.h"
#include "CStrategy.h"
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