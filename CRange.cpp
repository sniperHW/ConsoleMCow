//#include "pch.h"
#include "CRange.h"
using namespace std;

CRange::CRange(RelativePosition heroRelativePosition) :m_heroRelativePosition(heroRelativePosition) {}

bool CRange::Load(GameType gmType, const string& sNodeName, const CBoard& boardNext)
{
	return true;
}

bool CRange::Load(GameType gmType, const string& sNodeName, const CBoard& boardNext, bool blIsWizard)
{
	return true;
}

bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const CBoard& boardNext)
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
