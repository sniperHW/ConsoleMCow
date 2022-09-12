//#include "pch.h"
#include "CRange.h"
using namespace std;

//fileģʽ
bool CRange::Load(GameType gmType, const string& sNodeName, const CBoard& boardNext)
{
	return true;
}

//wizardģʽ
bool CRange::Load(GameType gmType, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace)
{
	return true;
}

//solverģʽ
bool CRange::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const StackByStrategy& stack, const string& sBoardNext, const SuitReplace& suitReplace)
{
	RangeData OOPRangeRatio; //���ڼ���rangeʣ�������OOP�����һ���ж������
	RangeData IPRangeRatio; //���ڼ���rangeʣ�������IP����ڶ����ж������
	RangeData* pRangeRatio = nullptr;
	//��ComboMapping��ÿһ����ϳ�ʼ��OOPRangeRatio��IPRangeRatio����ʼ��ֵΪ0

	//��actionSquence��ÿһ��������ֻ��X��R��C������Xֻ������ڵ�һ�������һ��һ����C��û��A��O��
		//ȷ���ǲ���OOP����IP��������OOP��ż����IP����ֵpRangeRatio��
		//��ǰ�ڵ㣨��һ��Ϊroot����ѡ���Ӧ�Ķ������Ͳ����з���һ������R����Ҫƥ��A
		//�Բ���������ÿ����ϣ���ѡ��������Ӧ�ı���ȥ��дpRangeRatio�ж�Ӧ�����ݣ�����ǵ�һ���ж�����д1*�������ǵ�һ���ж�����д*pRangeRatio[���] * ���������ֵ�һ��Ŀ�����ò����ڵ���ϱ���0��
		//�������һ��������֮ǰƥ��Ķ���ѡ���ӽڵ㣬Ϊ��ǰ�ڵ�

	//ͬ��ת��
	if (suitReplace.blIsNeeded) {
		ConvertIsomorphism(OOPRangeRatio, suitReplace);
		ConvertIsomorphism(IPRangeRatio, suitReplace);
	}

	//��ԭʼ��Χ����,����ԭʼ��Χ��ÿ�����ԭʼ��Χ���� *RangeRatio��Ӧ�ı���, �ܷ�һ��transform������multiplies��

	//��Χ��ȥ���µĹ��ƶ�Ӧ�ı���(��Ϊ0)��sBoardNext�п��������Ż�һ�ţ�����ÿ�Ŷ�Ӧ�Ķ���ȥ��
	RemoveComboByBoard(OOPRangeRatio, sBoardNext);
	RemoveComboByBoard(IPRangeRatio, sBoardNext);
	return true;
}

//תΪsolver�����ļ���Ҫ�ĸ�ʽ
string CRange::GetRangeStr()
{
	string v;
	return v;
}

//��
void CRange::Clear()
{

}
//ת��ͬ��
void CRange::ConvertIsomorphism(RangeData& rangeRatio, const SuitReplace& suitReplace)
{
	
}

//�ų�������ص����
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