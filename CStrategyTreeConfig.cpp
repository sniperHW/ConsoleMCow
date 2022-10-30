//#include "pch.h"
#include "CStrategyTreeConfig.h"
#include <regex>
#include "util.h"
#include <fstream>
#include "CActionLine.h"


using namespace std;

bool CStrategyTreeConfig::Init(GameType gmType)
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\" + GameTypeName[gmType] + "\\";

	string sLine, sFilePath;
	regex sep(R"(\s*;\s*)");
	regex reg_blank(R"(^\s*$)");
	vector<string> vTmp;
	smatch m;

	//����flop config����
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_FlopTreeConfig.txt";
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
			vTmp.push_back(p->str());
		}
		
		CFlopTreeConfigItem item;
		ParseDigitals(vTmp[1], item.m_flopBet);
		ParseDigitals(vTmp[2], item.m_flopRaise);
		ParseDigitals(vTmp[3], item.m_flopDonk);
		m_FlopTreeConfigItems.push_back(make_pair(vTmp[0], item));
	}
	fin.close();

	//����ʵʱ��������
	sFilePath = sConfigFolder + GameTypeName[gmType] + "_RTTreeConfig.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	sep = R"(\s*//\s*)";
	while (getline(fin, sLine)) {
		if (regex_match(sLine, reg_blank))
			continue;
		sregex_token_iterator p(sLine.cbegin(), sLine.cend(), sep, -1);
		sregex_token_iterator e;
		vTmp.clear();
		for (; p != e; ++p) {
			vTmp.push_back(p->str());
		}

		//(14 96)spr = 6.85	//	turn=bet:33 67 130; raise:50,100; donk:33		//	river=bet: bet:33 67 130; raise:50; donk:50
		double dlSpr;
		regex regSpr(".*spr=(.*)");
		if (regex_search(vTmp[0], m, regSpr))
			dlSpr = stod(m[1]);

		vector<string> vTmp2;
		regex sep1(R"(\s*;\s*)");
		CRTTreeConfigItem item;

		sregex_token_iterator p1(vTmp[1].cbegin(), vTmp[1].cend(), sep1, -1);
		sregex_token_iterator e1;
		vTmp2.clear();
		for (; p1 != e1; ++p1) {
			vTmp2.push_back(p1->str());
		}
		ParseDigitals(vTmp2[0], item.m_turnBet);
		ParseDigitals(vTmp2[1], item.m_turnRaise);
		ParseDigitals(vTmp2[2], item.m_turnDonk);

		sregex_token_iterator p2(vTmp[2].cbegin(), vTmp[2].cend(), sep1, -1);
		sregex_token_iterator e2;
		vTmp2.clear();
		for (; p2 != e2; ++p2) {
			vTmp2.push_back(p2->str());
		}
		ParseDigitals(vTmp2[0], item.m_riverBet);
		ParseDigitals(vTmp2[1], item.m_riverRaise);
		ParseDigitals(vTmp2[2], item.m_riverDonk);

		m_RTTreeConfigItems.push_back(make_pair(dlSpr, make_shared<CRTTreeConfigItem>(item)));
	}
	fin.close();
	sort(m_RTTreeConfigItems.begin(), m_RTTreeConfigItems.end(), [](const pair<double, shared_ptr<CRTTreeConfigItem>>& elem1, const pair<double, shared_ptr<CRTTreeConfigItem>>& elem2) {return elem1.first < elem2.first; });

	return true;
}

//用于flop wizard模式和turn presave模式
void CStrategyTreeConfig::GetFlopCandidateRatios(const string& sPreflopName, vector<double>& candidateRatios, const SpaceMode mode)
{
	candidateRatios.clear();
	smatch m;
	for (auto it : m_FlopTreeConfigItems) {
		regex reg(it.first);
		if (regex_search(sPreflopName, m, reg)) {
			switch (mode) {
			case space_bet:
				copy(it.second.m_flopBet.cbegin(), it.second.m_flopBet.cend(), back_inserter(candidateRatios));
				break;
			case space_raise:
				copy(it.second.m_flopRaise.cbegin(), it.second.m_flopRaise.cend(), back_inserter(candidateRatios));
				break;
			case space_donk:
				copy(it.second.m_flopDonk.cbegin(), it.second.m_flopDonk.cend(), back_inserter(candidateRatios));
				break;
			}
		}
	}
}

shared_ptr<CRTTreeConfigItem> CStrategyTreeConfig::GetRTTreeConfig(const Stacks& stack)
{
	if (m_RTTreeConfigItems.size() == 0)
		return nullptr;
	double dlSrp = stack.dEStack / stack.dPot;
	pair<double, std::shared_ptr<CRTTreeConfigItem>> value = make_pair(dlSrp,nullptr);
	auto pos = lower_bound(m_RTTreeConfigItems.begin(), m_RTTreeConfigItems.end(), value, [](auto& e1, auto& e2) {return e1.first < e2.first; });
	if (pos != m_RTTreeConfigItems.end()) {
		return pos->second;
	}
	else
		return m_RTTreeConfigItems.back().second;
}

void CStrategyTreeConfig::ParseDigitals(const string& sDigitals, vector<double>& v)
{
	regex sepBlank(R"(\s+)");
	string s = sDigitals;

	auto idx = sDigitals.find(':');
	if (idx != string::npos)
		s = sDigitals.substr(idx + 1, sDigitals.size());

	sregex_token_iterator p(s.cbegin(), s.cend(), sepBlank, -1);
	sregex_token_iterator e;
	for (; p != e; ++p) 
		v.push_back(stod(p->str()));

}
