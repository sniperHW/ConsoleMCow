//#include "pch.h"
#include "CRange.h"
using namespace std;

CRange::CRange(RelativePosition heroRelativePosition) :m_heroRelativePosition(heroRelativePosition) {}

bool CRange::Load(GameType gmType, const string& sNodeName, const CBoard& boardNext)
{
	return true;
}

bool CRange::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const CBoard& boardNext, bool blIsWizard)
{
	return true;
}

bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const CBoard& boardNext)
{
	return true;
}

string CRange::GetRangeStr()
{
	string v;
	return v;
}

void CRange::Clear()
{

}

void CRange::ConvertIsomorphism(const SuitReplace& suitReplace)
{
	
}

void CRange::RemoveComboByBoard(const CBoard& pBoard)
{

}

//按实际下注bb数，匹配子节下注空间，用于sover解计算，参数都为实际size，返回为匹配的siz
int CRange::MatchBetSize(double dActuallySize, const vector<double>& candidateSizes, GameType gmType, const StackByStrategy& stack)
{
	return 0;
}

//按实际下注比例，匹配子节下注空间，用于wizard解计算，需要先将size转为比例，候选在策略树设置中，返回为匹配的比例
int CRange::MatchBetRatio(double dActuallyRatio, const vector<double>& candidateRatios, GameType gmType, const StackByStrategy& stack)
{
	return 0;
}