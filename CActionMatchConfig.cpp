
#include "CActionMatchConfig.h"
#include <regex>
#include "util.h"
#include <fstream>
#include "CActionLine.h"

using namespace std;

bool CActionMatchConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";

	string sLine, sFilePath;
	regex sep(R"(\s+)");
	regex not_ation("[^-CRXFC]");
	regex reg_blank(R"(^\s*$)");
	vector<string> vTmp;
	string sTmp;
	smatch m;

	//����preflop����
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_preflop_ActionMatchConfig.txt";
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

		if (!regex_search(vTmp[0], m, not_ation))	//������Ϊ����ƥ��
			m_preflopFullNameTB[vTmp[0]] = vTmp[1];
		else
			m_preflopRegexTB.push_back(make_pair(vTmp[0], vTmp[1]));
	}
	fin.close();

	//����toflop����
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_toflop_ActionMatchConfig.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	while (getline(fin, sLine)) {
		if (sLine.size() == 0)
			continue;

		sregex_token_iterator p(sLine.cbegin(), sLine.cend(), sep, -1);
		sregex_token_iterator e;
		vTmp.clear();
		for (; p != e; ++p) {
			string s = p->str();
			CActionLine::EraseBlank(s);
			vTmp.push_back(s);
		}

		if (!regex_search(vTmp[0], m, not_ation))	//������Ϊ����ƥ��
			m_toflopFullNameTB[vTmp[0]] = vTmp[1];
		else
			m_toflopRegexTB.push_back(make_pair(vTmp[0], vTmp[1]));
	}
	fin.close();

	return true;
}

//?��gmType�Ĺ�ϵ
string CActionMatchConfig::GetAbbrName(const ActionMatchMode mode, const string& sActionSquence) //����NodeName��д
{
	regex reg;
	unordered_map<string,string>* pFullNameTB = nullptr;
	RegexTB* pRegexTB = nullptr;

	if (mode == match_preflop) {
		pFullNameTB = &m_preflopFullNameTB;
		pRegexTB = &m_preflopRegexTB;
	}
	else if (mode == match_toflop) {
		pFullNameTB = &m_toflopFullNameTB;
		pRegexTB = &m_toflopRegexTB;
	}
	else
		return "";

	//�Ȳ�������ƥ��
	auto pos = pFullNameTB->find(sActionSquence);
	if (pos != pFullNameTB->end())
		return pos->second;
	else {
		//�ٰ�����ƥ��
		for (auto matchItem : *pRegexTB) {
			reg = matchItem.first;
			if (regex_match(sActionSquence, reg))
				return matchItem.second;
		}
	}
	return "";
}