//#include "pch.h"
#include "CDataFrom.h"
#include <regex>
#include <sstream>
#include "util.h"
#include "CActionLine.h"
#include <fstream>

using namespace std;

string CDataFrom::GetWizardFilePath(GameType gmType, const string& sNodeName)
{
// \Data\WizardFile\Max6_NL50_SD100\flop\HJ_vsUTG_srp<>R50-R50-R50-A\HJ_vsUTG_srp<KsQs7d>R50-R50-R50-A.json

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	regex reg(R"(.*\<.*\>.*)");
	string sNodeNameWithoutBoard;
	stringstream ss;

	if (regex_match(sNodeName, reg)){	//flop
		sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sNodeName);
		ss << sConfigFolder << "\\Data\\" << "WizardFile\\" << GameTypeName[gmType] << "\\flop\\" << sNodeNameWithoutBoard << "\\" <<sNodeName << ".json";
	}
	else {	//preflop
		ss << sConfigFolder << "\\Data\\" << "WizardFile\\" << GameTypeName[gmType] << "\\preflop\\" << sNodeName << ".json";
	}

	string sReplaceBrackets = ss.str();
	for (int i = 0; i < sReplaceBrackets.size(); i++)
	{
		if (sReplaceBrackets[i] == '<')
			sReplaceBrackets[i] = '{';
		if (sReplaceBrackets[i] == '>')
			sReplaceBrackets[i] = '}';
	}

	return sReplaceBrackets;
}

string CDataFrom::GetSolverFilePath(GameType gmType, const string& sNodeName)
{
	// \Data\SolverFile\Max6_NL50_SD100\flop\HJ_vsUTG_srp<>\HJ_vsUTG_srp<KsQs7d>.json
	// \Data\SolverFile\Max6_NL50_SD100\turn\HJ_vsUTG_srp<>X-R33\HJ_vsUTG_srp<KsQs7d>X-R33<8d>.json

	regex reg(R"((.*\<.*\>.*)\<.*\>)");
	string sFlop;
	string sNodeNameWithoutBoard;
	stringstream ss;
	smatch m;

	if (regex_match(sNodeName, reg)) {	//turn
		if (regex_search(sNodeName, m, reg)) {
			sFlop = m[1];
			sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sFlop);
			ss << m_sDataFolder << "\\Data\\" << "SolverFile\\" << GameTypeName[gmType] << "\\turn\\" << sNodeNameWithoutBoard << "\\" << sNodeName << ".json";
		}
	}
	else {	//flop
		sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sNodeName);
		ss << m_sDataFolder << "\\Data\\" << "SolverFile\\" << GameTypeName[gmType] << "\\flop\\" << sNodeNameWithoutBoard << "\\" << sNodeName << ".json";
	}

	string sReplaceBrackets = ss.str();
	for (int i = 0; i < sReplaceBrackets.size(); i++)
	{
		if (sReplaceBrackets[i] == '<')
			sReplaceBrackets[i] = '{';
		if (sReplaceBrackets[i] == '>')
			sReplaceBrackets[i] = '}';
	}

	return sReplaceBrackets;
}

string CDataFrom::GetRangesFilePath(GameType gmType, const string& sNodeName)
{
	//\Data\RangesFile\Max6_NL50_SD100\3BET_#_vs_4callcold3bet4bet_EPfold.txt

	stringstream ss;

/* 在调用初处理，以保存代码一致性
	//需要去掉<>
	string sPreflopNodeName;
	regex reg(R"((.*)\<.*\>)");
	smatch m;
	if (regex_search(sNodeName, m, reg)) 
		sPreflopNodeName = m[1];
*/

	ss << m_sDataFolder << "\\Data\\" << "RangeFile\\" << GameTypeName[gmType] << "\\" << sNodeName << ".txt";

	return ss.str();
}

string CDataFrom::GetStrategyFilePath(GameType gmType, const string& sNodeName)
{
	// \Data\WizardFile\Max6_NL50_SD100\flop\HJ_vsUTG_srp<>R50-R50-R50-A\HJ_vsUTG_srp<KsQs7d>R50-R50-R50-A.txt

	regex reg(R"(.*\<.*\>.*)");
	string sNodeNameWithoutBoard;
	stringstream ss;

	if (regex_match(sNodeName, reg)) {	//flop(?目前不用)
		sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sNodeName);
		ss << m_sDataFolder << "\\Data\\" << "StrategyFile\\" << GameTypeName[gmType] << "\\flop\\" << sNodeNameWithoutBoard << "\\" << sNodeName << ".txt";
	}
	else {	//preflop
		ss << m_sDataFolder << "\\Data\\" << "StrategyFile\\" << GameTypeName[gmType] << "\\preflop\\" << sNodeName << ".txt";
	}

	string sReplaceBrackets = ss.str();
	for (int i = 0; i < sReplaceBrackets.size(); i++)
	{
		if (sReplaceBrackets[i] == '<')
			sReplaceBrackets[i] = '{';
		if (sReplaceBrackets[i] == '>')
			sReplaceBrackets[i] = '}';
	}

	return sReplaceBrackets;
}

string CDataFrom::GetRTSolverFilePath(const std::string& sGameID)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	stringstream ss;
	ss << sConfigFolder << "\\SolverConfigs\\"  << sGameID << ".json";

	return ss.str();
}

string CDataFrom::GetRTSolverConfigFilePath(const std::string& sGameID)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	stringstream ss;
	ss << sConfigFolder << "\\SolverConfigs\\" << sGameID << ".txt";

	return ss.str();
}

bool CDataFrom::Init()
{
	//读取数据目录配置
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	m_sDataFolder = buffer;	//默认为程序目录
	sConfigFolder = sConfigFolder + "\\configs\\";
	regex reg_blank(R"(^\s*$)");

	string sLine, sFilePath;
	sFilePath = sConfigFolder  + "globalConfigs.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	regex reg_configItem;
	smatch m;
	while (getline(fin, sLine)) {
		if (regex_match(sLine, reg_blank))
			continue;

		reg_configItem = R"(DataFolder=(.*))";
		if (regex_search(sLine, m, reg_configItem)) {
			m_sDataFolder = m[1];
			continue;
		}
	}
	fin.close();

	return true;

}
