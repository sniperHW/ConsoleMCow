#include "CTurnPresaveSolverConfig.h"
#include "util.h"
#include <fstream>
#include <regex>
using namespace std;


bool CTurnPresaveSolverConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";
	regex reg_blank(R"(^\s*$)");

	string sLine, sFilePath;

	//����preflop����
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

	return true;
}

bool CTurnPresaveSolverConfig::IsPresaveSolver(const std::string sAbbrName)
{
	return m_sAbbrNames.find(sAbbrName) != m_sAbbrNames.end();
}
