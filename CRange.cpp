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

//��ʵ����עbb����ƥ���ӽ���ע�ռ䣬����sover����㣬������Ϊʵ��size������Ϊƥ���siz
int CRange::MatchBetSize(double dActuallySize, const vector<double>& candidateSizes, GameType gmType, const StackByStrategy& stack)
{
	return 0;
}

//��ʵ����ע������ƥ���ӽ���ע�ռ䣬����wizard����㣬��Ҫ�Ƚ�sizeתΪ��������ѡ�ڲ����������У�����Ϊƥ��ı���
int CRange::MatchBetRatio(double dActuallyRatio, const vector<double>& candidateRatios, GameType gmType, const StackByStrategy& stack)
{
	return 0;
}