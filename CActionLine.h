#pragma once
#ifndef CACTIONLINE_H_
#define CACTIONLINE_H_

#include "globledef.h"
#include "CGame.h"

class CActionLine
{
private:
	std::string m_sReserve; //��ǰ���Ѿ�������
	std::string m_sPreflopType; 

	bool MatchPreflopNode(const std::string& sAbbrLine, std::string& sNodeName);

public:
	bool Parse(const std::string& sActionLine, ActionLineMode msgMode, std::string & sNodeName, CGame & game); //offline����˷���false,����game��أ�ȡ��NodeName, ��¼raisesize����¼preflop���ͣ�sActionLineΪ����
	const std::string & GetPreflopType();
	void ChangeRound();

};

#endif
