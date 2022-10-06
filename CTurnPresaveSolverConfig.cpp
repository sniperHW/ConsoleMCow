#include "CTurnPresaveSolverConfig.h"
#include <direct.h>
#include <fstream>
#include <regex>
using namespace std;

//for test
#include <iostream>

bool CTurnPresaveSolverConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";
	regex reg_blank(R"(^\s*$)");

	string sLine, sFilePath;

	//º”‘ÿpreflop≈‰÷√
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_TurnPresaveSolverConfig.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	while (getline(fin, sLine)) {
		if (regex_match(sLine, reg_blank))
			continue;
		m_sAbbrNames.insert(sLine);
	}
	fin.close();

	for (auto s : m_sAbbrNames)
		cout << s << endl;

	return true;
}

bool CTurnPresaveSolverConfig::IsPresaveSolver(const std::string sAbbrName)
{
	return m_sAbbrNames.find(sAbbrName) != m_sAbbrNames.end();
}
