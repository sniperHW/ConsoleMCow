//#include "pch.h"
#include "CActionLine.h"
#include "globledef.h"
using namespace std;

bool CActionLine::MatchPreflopNode(const string& sAbbrLine, string& sNodeName)
{
	return true;
}

bool CActionLine::Parse(const string& sActionLine, const ActionLineMode msgMode, string& sNodeName, CGame& game,StackByStrategy& curStackByStrategy)
{
	//flopΪwizard����Ҫ��size����ɱ�����������size(bb��)
	return true;
}

const string& CActionLine::GetNodeType()
{
	return m_sNodeType;
}

void CActionLine::ChangeRound()
{

}
