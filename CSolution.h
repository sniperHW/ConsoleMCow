#pragma once

#ifndef CSOLUTION_H_
#define CSOLUTION_H_

#include "globledef.h"
#include "CGame.h"
#include "CActionLine.h"
#include "CDataFrom.h"
#include "./json/include/json.h"
#include "CStrategy.h"

class CSolution
{
public:
	Action HeroAction(const std::string & sActionLine);
	void FlopReady(const std::string & sActionLine);
	void TurnReady(const std::string & sActionLine);
	void RiverReady(const std::string & sActionLine);


private:
	std::string m_sWholeNodeName; //�����ڵ���
	std::string m_sCurStreetNodeName; //��ǰ�ֵĽڵ�����
	std::string m_sPreflopType; //�ڵ����ͣ�����solverƥ�����������

	CGame m_game;
	Json::Value m_solverResult; //��ǰ�ֵ�solver��
	bool m_blIsSolverPresave; //��ǰ���Ƿ�ʹ��Ԥ��solver��
	CActionLine m_actionLine;
	CDataFrom m_dataFrom;

	Action CalcHeroAction(const CStrategy& strategy);

};

#endif