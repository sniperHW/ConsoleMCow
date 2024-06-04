#pragma once

#ifndef CACTION_MATCH_CONFIG_H_
#define CACTION_MATCH_CONFIG_H_

#include "globledef.h"
	
class CActionMatchConfig
{
public:
	bool Init(GameType gmType);
	std::string GetAbbrName(const ActionMatchMode mode, const std::string & sActionSquence); //����NodeName��д

private:
	std::unordered_map<std::string, std::string> m_preflopFullNameTB; //����ƥ��
	std::unordered_map<std::string, std::string> m_toflopFullNameTB; //����ƥ��	
	RegexTB m_preflopRegexTB; //����ƥ��
	RegexTB m_toflopRegexTB;  //����ƥ��
};

#endif