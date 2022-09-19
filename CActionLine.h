#pragma once
#ifndef CACTIONLINE_H_
#define CACTIONLINE_H_

#include "globledef.h"
#include "CGame.h"

class CActionLine
{

private:
	std::string m_sNodeName; //策略名称，preflop为完整名称，flop为：preflop完整名称<>flop策略空间ActionSquence，用于获取筹码数据
	std::string m_sActionSquence; //当前街的完整动作序列，size为实际下注
	std::string m_sPreflopAbbrName; //preflop的简写，用于匹配策略树

public:
	bool Parse(const std::string& sActionLine, const ActionLineMode msgMode, std::string & sActionSquence, CGame & game, Stacks& curStacks); //offline或多人返回false,更新game相关，取得NodeName, 记录raisesize，记录preflop类型，sActionLine为增量
	void ChangeRound();

};

#endif
