//#include "pch.h"
#include "CStrategyNodeConfig.h"
#include "CActionLine.h"
#include <regex>
#include <direct.h>
#include <fstream>

//for test
#include <iostream>

using namespace std;

bool CStrategyNodeConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";

	string sLine, sFilePath;
	regex sep(R"(\s*//\s*)");
	regex reg_blank(R"(^\s*$)");
	vector<string> vTmp;
	smatch m;

	//加载preflop配置
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_StrategyNodeConfig.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	while (getline(fin, sLine)) {
		if (regex_match(sLine, reg_blank))
			continue;
		sregex_token_iterator p(sLine.cbegin(), sLine.cend(), sep, -1);
		sregex_token_iterator e;
		vTmp.clear();
		for (; p != e; ++p) {
			string s = p->str();
			CActionLine::EraseBlank(s);
			vTmp.push_back(s);
		}
		
		CStrategyNodeConfigItem item;
		item.m_sReplaceNodeName = vTmp[1];
		item.m_sSpecialProcessing = "";
		item.blReplace = false;
		if(vTmp.size()>2)
			item.m_sSpecialProcessing = vTmp[2];
		if(vTmp.back() == "replace")
			item.blReplace = true;

		if (vTmp.back() == "regex" || vTmp.back() == "replace") //正则匹配
			m_regexConfigItems.push_back(make_pair(vTmp[0], make_shared<CStrategyNodeConfigItem>(item)));
		else //普通匹配
			m_configItems[vTmp[0]] = make_shared<CStrategyNodeConfigItem>(item);
	}
	fin.close();

	return true;
}

string CStrategyNodeConfig::GetReplace(const string& sNodeName)
{
	//查找map
	auto result = m_configItems.find(sNodeName);
	if (result != m_configItems.end()) 
		return result->second->m_sReplaceNodeName;

	//按正则匹配
	regex reg;
	for (auto it : m_regexConfigItems) {
		reg = it.first;
		if (regex_match(sNodeName, reg)) {
			if (it.second->blReplace) {
				return regex_replace(sNodeName, reg, it.second->m_sReplaceNodeName);
			}
			else
				return it.second->m_sReplaceNodeName;
		}
	}

	return "";
}

string CStrategyNodeConfig::GetSpecial(const string& sNodeName)
{
	//查找map
	auto result = m_configItems.find(sNodeName);
	if (result != m_configItems.end())
		return result->second->m_sSpecialProcessing;

	//按正则匹配
	regex reg;
	for (auto it : m_regexConfigItems) {
		reg = it.first;
		if (regex_match(sNodeName, reg))
			return it.second->m_sSpecialProcessing;
	}

	return "";
}



