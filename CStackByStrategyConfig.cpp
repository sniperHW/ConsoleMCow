#include "CStackByStrategyConfig.h"
#include "CActionLine.h"

#include <regex>
#include "util.h"
#include <fstream>


using namespace std;

bool CStackByStrategyConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";

	string sLine, sFilePath;
	regex sep(R"(\s+)");
	regex reg_blank(R"(^\s*$)");
	vector<string> vTmp;
	smatch m;

	//¼ÓÔØpreflopÅäÖÃ
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_StackByStrategyConfig.txt";
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

		Stacks stacks;
		regex regDouble("(.*)/(.*)");
		if (regex_search(vTmp[1], m, regDouble)) {
			stacks.dPot = stod(m[1]);
			stacks.dEStack = stod(m[2]);
		}
		else 
			return false;

		m_configItems[vTmp[0]] = make_shared<Stacks>(stacks);

	}
	fin.close();

	return true;
}

const shared_ptr<Stacks> CStackByStrategyConfig::GetItemByName(const string& sNodeName) const
{
	string sNodeNameWithoutBoard = CActionLine::GetNodeNameWithoutBoard(sNodeName);

	auto pos = m_configItems.find(sNodeNameWithoutBoard);
	if (pos != m_configItems.end())
		return pos->second;
	else
		return nullptr;
}