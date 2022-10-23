//#include "pch.h"
#include "CDataFrom.h"
#include <regex>
#include <sstream>
#include "util.h"
#include "CActionLine.h"

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

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	regex reg(R"((.*\<.*\>.*)\<.*\>)");
	string sFlop;
	string sNodeNameWithoutBoard;
	stringstream ss;
	smatch m;

	if (regex_match(sNodeName, reg)) {	//turn
		if (regex_search(sNodeName, m, reg)) {
			sFlop = m[1];
			sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sFlop);
			ss << sConfigFolder << "\\Data\\" << "SolverFile\\" << GameTypeName[gmType] << "\\turn\\" << sNodeNameWithoutBoard << "\\" << sNodeName << ".json";
		}
	}
	else {	//flop
		sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sNodeName);
		ss << sConfigFolder << "\\Data\\" << "SolverFile\\" << GameTypeName[gmType] << "\\flop\\" << sNodeNameWithoutBoard << "\\" << sNodeName << ".json";
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
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	stringstream ss;

	//ÐèÒªÈ¥µô<>
	string sPreflopNodeName;
	regex reg(R"((.*)\<.*\>)");
	smatch m;
	if (regex_search(sNodeName, m, reg)) 
		sPreflopNodeName = m[1];

	ss << sConfigFolder << "\\Data\\" << "RangesFile\\" << GameTypeName[gmType] << "\\" << sPreflopNodeName << ".txt";

	return ss.str();
}
