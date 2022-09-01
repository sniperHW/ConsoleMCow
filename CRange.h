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

	bool Load(GameType gmType, const std::string & sNodeName, const CBoard & boardNext); //file模式，同时排除公牌
	bool Load(GameType gmType, const std::string & sNodeName, const CBoard & boardNext, bool blIsWizard); //wizard模式，同构转换，排除公牌，忽略blIsWizard
	bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const CBoard & boardNext); //solver模式，同构转换，排除公牌

	std::string GetRangeStr(); //转为solver配置文件需要的格式
	void Clear();

private:
	void ConvertIsomorphism(const SuitReplace & suitReplace); //转换同构
	void RemoveComboByBoard(const CBoard & pBoard); //排除公牌相关的组合
};

#endif
