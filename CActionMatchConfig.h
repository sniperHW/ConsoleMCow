#pragma once

#ifndef CACTION_MATCH_CONFIG_H_
#define CACTION_MATCH_CONFIG_H_

#include "globledef.h"
	
class CActionMatchConfig
{
public:
	bool Init(GameType gmType);
	std::string GetAbbrName(const ActionMatchMode mode, const std::string & sActionSquence); //返回NodeName简写

private:
	std::unordered_map<std::string, std::string> m_preflopFullNameTB; //完整匹配
	std::unordered_map<std::string, std::string> m_toflopFullNameTB; //完整匹配	
	RegexTB m_preflopRegexTB; //正则匹配
	RegexTB m_toflopRegexTB;  //正则匹配
};

#endif