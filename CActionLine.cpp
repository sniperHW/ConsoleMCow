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
	//flop为wizard解需要将size折算成比例，其他用size(bb数)
	return true;
}

const string& CActionLine::GetNodeType()
{
	return m_sNodeType;
}

void CActionLine::ChangeRound()
{

}
