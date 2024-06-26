//#include "pch.h"
#include "CRangeNodeConfig.h"
#include "CActionLine.h"
#include <regex>
#include "util.h"
#include <fstream>


using namespace std;

bool CRangeNodeConfig::Init(GameType gmType)
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

	//����preflop����
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_RangeNodeConfig.txt";
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

		CRangeNodeConfigItem item;
		item.m_sReplaceNodeName = vTmp[1];
		item.blReplace = false;
		if (vTmp.back() == "replace")
			item.blReplace = true;

		if (vTmp.back() == "regex" || vTmp.back() == "replace") //����ƥ��
			m_regexConfigItems.push_back(make_pair(vTmp[0], make_shared<CRangeNodeConfigItem>(item)));
		else //��ͨƥ��
			m_configItems[vTmp[0]] = make_shared<CRangeNodeConfigItem>(item);
	}
	fin.close();

	return true;
}

string CRangeNodeConfig::GetReplace(const string& sNodeName)
{
	//需要去掉最后一个<.*>
	string sPrefix, sSuffix;
	auto p = sNodeName.find_last_of('<');
	sPrefix = sNodeName.substr(0, p);
	sSuffix = sNodeName.substr(p, sNodeName.size());

	auto result = m_configItems.find(sPrefix);
	if (result != m_configItems.end())
		return result->second->m_sReplaceNodeName + sSuffix;

	regex reg;
	for (auto it : m_regexConfigItems) {
		reg = it.first;
		if (regex_match(sPrefix, reg)) {
			if (it.second->blReplace) {
				return regex_replace(sPrefix, reg, it.second->m_sReplaceNodeName) + sSuffix;
			}
			else
				return it.second->m_sReplaceNodeName + sSuffix;
		}
	}

	return "";
}

