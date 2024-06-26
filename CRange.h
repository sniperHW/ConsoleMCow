#pragma once

#ifndef CRANGE_H_
#define CRANGE_H_

#include "globledef.h"
#include "json/json.h"
#include "CBoard.h"

class CRange
{
public:
	RangeData m_OOPRange;
	RangeData m_IPRange;

	bool Load(GameType gmType, const std::string & sNodeName); //file模式，同时排除公牌
	bool Load(GameType gmType, const std::string & sNodeName, const SuitReplace& suitReplace, const std::string& sIsoBoard); //wizard模式，同构转换，排除公牌
	//bool Load(GameType gmType, const Json::Value & root, const std::string & sActionSquence, const Stacks& stacks, const Stacks& stacksByStrategy, const SuitReplace& suitReplace); //solver模式，同构转换，排除公牌
	bool Load(GameType gmType, const Json::Value& root, const std::string& sActionSquence, const Stacks& stacks, const SuitReplace& suitReplace); //solver模式，同构转换，排除公牌(stack从json文件中取)

	static RangeData getAverageRange(const RangeData& rangeDataLow ,const RangeData& rangeDataUp, const double dSegmentRatio);

	std::string GetRangeStr(); //转为solver配置文件需要的格式
	void Clear(); 

	void DumpRange(const std::string& sComment, const RelativePosition heroRPosition);
	static void DumpRange(const std::string& sComment, const RangeData* pRange);
	void ReadRange(const std::string& sPath, RangeData& range);	//本系统不用

private:
	void ConvertIsomorphism(RangeData& rangeRatio, const SuitReplace & suitReplace); //转换同构
	void RemoveComboByBoard(RangeData& rangeRatio, const std::string& sBoardNext); //排除公牌相关的组合


};

#endif
