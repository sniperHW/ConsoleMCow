#include "CSpecialProcessingMacroConfig.h"
#include "util.h"
#include <fstream>
#include <regex>
#include "CActionLine.h"


using namespace std;

bool CSpecialProcessingMacroConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";

	string sLine, sFilePath;
	regex sep(R"(\s*:\s*)");
	regex reg_blank(R"(^\s*$)");
	vector<string> vTmp;

	sFilePath = sConfigFolder + GameTypeName[gmType] + "_SpecialProcessingMacroConfig.txt";
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

		m_configItems[vTmp[0]] = vTmp[1];

	}
	fin.close();

	return true;
}

std::string CSpecialProcessingMacroConfig::GetActuallyCommand(const std::string& sMacro)
{
	auto p = m_configItems.find(sMacro);
	if (p != m_configItems.end())
		return p->second;
	else
		return "";
}