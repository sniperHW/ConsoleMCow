#pragma once
#ifndef CACTIONLINE_H_
#define CACTIONLINE_H_

#include "globledef.h"
#include "CGame.h"

class CActionLine
{

private:
	std::string m_sReserve; //当前街已经解析的
	std::string m_sPreflopType; 

	bool MatchPreflopNode(const std::string& sAbbrLine, std::string& sNodeName);

public:
	bool Parse(const std::string& sActionLine, ActionLineMode msgMode, std::string & sNodeName, CGame & game); //offline或多人返回false,更新game相关，取得NodeName, 记录raisesize，记录preflop类型，sActionLine为增量
	const std::string & GetPreflopType();
	void ChangeRound();
	
};

#endif
