//#include "pch.h"
#include "CActionLine.h"
using namespace std;

bool CActionLine::MatchPreflopNode(const string& sAbbrLine, string& sNodeName)
{
	return true;
}

bool CActionLine::Parse(const string& sActionLine, ActionLineMode msgMode, string& sNodeName, CGame& game)
{
	return true;
}

const string& CActionLine::GetPreflopType()
{
	return m_sPreflopType;
}

void CActionLine::ChangeRound()
{

}
