//#include "pch.h"
#include "CSolver.h"
#include "util.h"
#include "globledef.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "CStrategyTreeConfig.h"

using namespace std;
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略节点配置

bool CSolver::ToSolve(const string& sGameID, const CSolverConfig& config, Json::Value& result, const SolverCalcMode calcMode)
{
	string sConfigPath = MakeConfigFile(sGameID, config);


	//使用完后删除配置文件
	//remove(sConfigPath.c_str());
	return true;
}

void CSolver::ToStop(int nGameID)
{

}

string CSolver::MakeConfigFile(const string& sGameID, const CSolverConfig& config)
{
	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\SolverConfigs\\";
	string sPath = sConfigFolder + sGameID + ".txt";

	std::ofstream ofs;
	ofs.open(sPath, std::ofstream::out);
	if (!ofs.is_open()) {
		//记录错误
		return "";
	}

	string sLine;
	//pot
	sLine = "set_pot " + double2String(config.m_stacks.dPot, 1);
	ofs << sLine << endl;
	//set_effective_stack
	sLine = "set_effective_stack " + double2String(config.m_stacks.dEStack, 1);
	ofs << sLine << endl;
	//set_effective_stack
	sLine = "set_board " + GetSplitBoard(config.m_sBoard);
	ofs << sLine << endl;

	//rang去掉0的,同比放大
	RangeData OOPRange, IPRange;
	copy_if(config.m_pRange->m_OOPRange.begin(), config.m_pRange->m_OOPRange.end(), inserter(OOPRange, OOPRange.begin()), [](pair<string, double> e) { return e.second > 0; });
	copy_if(config.m_pRange->m_IPRange.begin(), config.m_pRange->m_IPRange.end(), inserter(IPRange, IPRange.begin()), [](pair<string, double> e) { return e.second > 0; });
	auto itOOPMax = max_element(OOPRange.begin(), OOPRange.end(), [](pair<string, double> e1, pair<string, double> e2) {return e1.second < e2.second; });
	auto itIPMax = max_element(IPRange.begin(), IPRange.end(), [](pair<string, double> e1, pair<string, double> e2) {return e1.second < e2.second; });
	for (auto p = OOPRange.begin(); p != OOPRange.end(); ++p)
		p->second *= GetboostRatio(itOOPMax->second);
	for (auto p = IPRange.begin(); p != IPRange.end(); ++p)
		p->second *= GetboostRatio(itOOPMax->second);

	//set_range_oop
	sLine = "set_range_oop ";
	for (auto p = OOPRange.cbegin(); p != OOPRange.cend(); p++) {
		if (p != OOPRange.cbegin())
			sLine += ",";
		sLine = sLine + p->first + ":" + double2String(p->second, 7);	
	}
	ofs << sLine << endl;
	//set_range_ip
	sLine = "set_range_ip ";
	for (auto p = IPRange.cbegin(); p != IPRange.cend(); p++) {
		if (p != IPRange.cbegin())
			sLine += ",";
		sLine = sLine + p->first + ":" + double2String(p->second, 7);
	}
	ofs << sLine << endl;
	//set tree

	sLine = "set_bet_sizes oop,turn,bet";
	for(auto it: config.m_pRTTreeConfigItem->m_turnBet)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,turn,raise";
	for (auto it : config.m_pRTTreeConfigItem->m_turnRaise)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,turn,donk";
	for (auto it : config.m_pRTTreeConfigItem->m_turnDonk)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,turn,allin";
	ofs << sLine << endl;

	sLine = "set_bet_sizes ip,turn,bet";
	for (auto it : config.m_pRTTreeConfigItem->m_turnBet)
		sLine = sLine += "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes ip,turn,raise";
	for (auto it : config.m_pRTTreeConfigItem->m_turnRaise)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes ip,turn,allin";
	ofs << sLine << endl;

	sLine = "set_bet_sizes oop,river,bet";
	for (auto it : config.m_pRTTreeConfigItem->m_riverBet)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,river,raise";
	for (auto it : config.m_pRTTreeConfigItem->m_riverRaise)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,river,donk";
	for (auto it : config.m_pRTTreeConfigItem->m_turnDonk)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,river,allin";
	ofs << sLine << endl;

	sLine = "set_bet_sizes ip,river,bet";
	for (auto it : config.m_pRTTreeConfigItem->m_riverBet)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes ip,river,raise";
	for (auto it : config.m_pRTTreeConfigItem->m_riverRaise)
		sLine = sLine + "," + double2String(it, 1);
	ofs << sLine << endl;
	sLine = "set_bet_sizes ip,river,allin";
	ofs << sLine << endl;

	//set_allin_threshold
	sLine = "set_allin_threshold " + double2String(m_fDefaultAllinThreshold, 2);
	ofs << sLine << endl;
	//set_raise_limit
	sLine = "set_allin_threshold " + to_string(m_nDefaultRaiseLimit);
	ofs << sLine << endl;
	//build_tree
	sLine = "build_tree";
	ofs << sLine << endl;
	//set_thread_num
	sLine = "set_thread_num " + to_string(m_nDefaultThreadNum);
	ofs << sLine << endl;
	//set_accuracy
	sLine = "set_accuracy " + double2String(m_fDefaultAccuracy, 1);
	ofs << sLine << endl;
	//set_max_iteration
	sLine = "set_max_iteration " + to_string(m_nDefaultMaxIteration);
	ofs << sLine << endl;
	//set_print_interval 10
	sLine = "set_print_interval 10";
	ofs << sLine << endl;
	//set_use_isomorphism 1
	sLine = "set_use_isomorphism 1";
	ofs << sLine << endl;
	//start_solve
	sLine = "start_solve";
	ofs << sLine << endl;
	//set_dump_rounds
	sLine = "set_dump_rounds 1";
	ofs << sLine << endl;
	//dump_result
	sLine = "dump_result "  + sConfigFolder + sGameID + ".json";
	ofs << sLine << endl;

	ofs.close();
	return sPath;
}

//返回10的n次方
double CSolver::GetboostRatio(const double dlSrc)
{
	for (int i = 0; i < 10; i++) {
		double dlBoost = pow(10, i) * dlSrc;
		if (dlBoost >= 0.1 && dlBoost <= 1)
			return pow(10, i);
	}
	return 0;
}

string CSolver::GetSplitBoard(const string& sBoardOld)
{
	string sNew = sBoardOld;
	auto p = sNew.end() - 2;
	while (p != sNew.begin()) {
		sNew.insert(p, 1, ',');
		p -= 2;
	}
	return sNew;
}
