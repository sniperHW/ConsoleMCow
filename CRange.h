#pragma once

#ifndef CRANGE_H_
#define CRANGE_H_

#include "globledef.h"
#include "./json/include/json.h"
#include "CBoard.h"

class CRange
{
public:
	RangeData m_heroRange;
	RangeData m_rivalRange;
	RelativePosition m_heroRelativePosition;

	CRange(RelativePosition heroRelativePosition);

	bool Load(GameType gmType, const std::string & sNodeName, const CBoard & boardNext); //fileģʽ��ͬʱ�ų�����
	bool Load(GameType gmType, const std::string & sNodeName, const CBoard & boardNext, bool blIsWizard); //wizardģʽ��ͬ��ת�����ų����ƣ�����blIsWizard
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const CBoard & boardNext); //solverģʽ��ͬ��ת�����ų�����

	std::string GetRangeStr(); //תΪsolver�����ļ���Ҫ�ĸ�ʽ
	void Clear();

private:
	void ConvertIsomorphism(const SuitReplace & suitReplace); //ת��ͬ��
	void RemoveComboByBoard(const CBoard & pBoard); //�ų�������ص����
};

#endif
