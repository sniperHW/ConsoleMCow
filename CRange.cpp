//#include "pch.h"
#include "CRange.h"
using namespace std;

//file模式
bool CRange::Load(GameType gmType, const string& sNodeName, const CBoard& boardNext)
{
	return true;
}

//wizard模式
bool CRange::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace)
{
	return true;
}

//solver模式
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace)
{
	RangeData OOPRangeRatio; //用于计算range剩余比例，OOP代表第一个行动的玩家
	RangeData IPRangeRatio; //用于计算range剩余比例，IP代表第二个行动的玩家
	RangeData* pRangeRatio = nullptr;
	//用ComboMapping的每一个组合初始化OOPRangeRatio和IPRangeRatio，初始数值为0

	//对actionSquence中每一个动作（只有X，R，C，其中X只会出现在第一个，最后一个一定是C，没有A和O）
		//确定是操作OOP还是IP（奇数是OOP，偶数是IP，赋值pRangeRatio）
		//当前节点（第一个为root）下选择对应的动作，和策略中方法一样，但R不需要匹配A
		//对策略数据中每个组合，用选定动作对应的比例去改写pRangeRatio中对应的数据（如果是第一次行动则填写1*比例，非第一次行动则填写*pRangeRatio[组合] * 比例，区分第一次目的是让不存在的组合保持0）
		//不是最后一个动作则按之前匹配的动作选择子节点，为当前节点

	//同构转换
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(OOPRangeRatio, suitReplace);
		ConvertIsomorphism(IPRangeRatio, suitReplace);
	}

	//和原始范围运算,更新原始范围（每个组合原始范围数据 *RangeRatio对应的比例, 能否一个transform函数加multiplies）

	//范围中去除新的公牌对应的比例(改为0)，sBoardNext中可能是三张或一张，三张每张对应的都有去除
	RemoveComboByBoard(OOPRangeRatio, sBoardNext);
	RemoveComboByBoard(IPRangeRatio, sBoardNext);
	return true;
}

//转为solver配置文件需要的格式
string CRange::GetRangeStr()
{
	string v;
	return v;
}

//？
void CRange::Clear()
{

}
//转换同构
void CRange::ConvertIsomorphism(RangeData& rangeRatio, const SuitReplace& suitReplace)
{
	
}

//排除公牌相关的组合
void CRange::RemoveComboByBoard(RangeData& rangeRatio, const std::string& sBoardNext)
{

}

int CRange::MatchBetSize(double dActuallySize, const vector<double>& candidateSizes, const double dEStatck)
{
	return 0;
}


int CRange::MatchBetRatio(double dActuallyRatio, const vector<double>& candidateRatios, const double dEStatck)
{
	return 0;
}