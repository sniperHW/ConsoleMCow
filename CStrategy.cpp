//#include "pch.h"
#include "CStrategy.h"
using namespace std;

//��wizard��ȡ
bool CStrategy::Load(GameType gmType, const string& sNodeName, const SuitReplace& suitReplace, const string& sIsoBoard)
{
	//sNodeNameΪ�������ƣ���������ǰround����flop��ǰ������
	// 
	//flop��Ҫ����ͬ��������ͬ���滻���滻�ڵ������е�board����

	//flop��Ҫƥ���滻���һ��betsize,�޸Ľڵ�����

	//�Ӳ��������л�ȡ�滻��special���ã������滻�������滻�����ƣ�turn��ͨ��ƥ�䷨

	//��ȡ�ڵ����ƶ�Ӧ���ļ�·����δ�ҵ��򷵻�false,����offline

	//�������ݵ�m_strategy��code X:check,RAI:allin,R*:raise,F:fold,C:call��(betsize:fBetSize,betsize_by_pot:fBetSizeByPot)

	//����special

	//ͬ��ת��

	return true;
}

//��solver��ȡ
bool CStrategy::Load(GameType gmType, const Json::Value& root, const string& sActionSquence, const SuitReplace& suitReplace)
{
	//��sActionSquence��㶨λ����Ҫѡ��Ľڵ㣬�޺�ѡ�򷵻�false,����offline
	// 
	//���ڶ��ѡ���򰴺�ѡbetsize��ƥ��ѡ��ڵ�
	// 
	//�������ݵ�m_strategy
	// 
	//ͬ��ת��

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