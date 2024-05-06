//#include "pch.h"
#include "CSolver.h"
#include "util.h"
#include "globledef.h"
#include <fstream>
#include <algorithm>
#include <cmath>
#include <numeric>
#include "CStrategyTreeConfig.h"
#include "CDataFrom.h"
#include <regex>

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/bzip2.hpp>
#include <boost/iostreams/filter/lzma.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/beast/core/detail/base64.hpp>


using namespace std;
extern map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略节点配置

bool CSolver::Init()
{
	ifstream fin;

	char buffer[_MAX_PATH];
	_getcwd(buffer, _MAX_PATH);
	string sConfigFolder = buffer;
	sConfigFolder = sConfigFolder + "\\configs\\";

	string sLine, sFilePath;
	regex sep(R"(\s*,\s*)");
	smatch m;

	sFilePath = sConfigFolder + "globalConfigs.txt";
	fin.open(sFilePath, ios_base::in);
	if (!fin.is_open())
		return false;

	regex reg_IP(R"(SolverIPs=(.*))");
	while (getline(fin, sLine)) {
		if (regex_search(sLine, m, reg_IP)) {
			string sIPs = m[1];

			sregex_token_iterator p(sIPs.cbegin(), sIPs.cend(), sep, -1);
			sregex_token_iterator e;
			for (; p != e; ++p) {
				string s = p->str();
				regex reg_item(R"((.*):(.*))");
				regex_search(s, m, reg_item);

				CSolverNode oneNode{ "",false,false,false };
				oneNode.m_sIP = m[1];
				string sNodeType = m[2];
				oneNode.m_isHighPerformance = sNodeType == "high" ? true : false;

				m_solverNodes[oneNode.m_sIP] = oneNode;
			}
			break;
		}
	}
	fin.close();

	return true;
}

bool CSolver::ToSolve(const string& sGameID, const CSolverConfig& config, Json::Value& result, const SolverCalcMode calcMode, const bool isRiver)
{
	string sConfigPath = MakeConfigFile(sGameID, config);
	string sResultFilePath = CDataFrom::GetRTSolverFilePath(sGameID);





	//使用完后删除配置文件和result文件
	//remove(sConfigPath.c_str());
	//remove(sResultFilePath.c_str());
	return true;
}

//目前不需要
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
	sLine = "set_raise_limit " + to_string(m_nDefaultRaiseLimit);
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
	//sLine = "dump_result "  + sConfigFolder + sGameID + ".json";
	sLine = "dump_result " + CDataFrom::GetRTSolverFilePath(sGameID);
	ofs << sLine << endl;

	ofs.close();
	return sPath;
}

string CSolver::MakeConfigFileSimple(const std::string& sGameID, const Round round, const vector<double>& AbnormalSizes, const CSolverConfig& config)
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



	if (round == flop) {
		if (AbnormalSizes.size() == 1) { // oop betsize
			sLine = "set_bet_sizes oop,flop,bet," + double2String(AbnormalSizes[0], 1);
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,flop,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,flop,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes ip,flop,bet,100" ;
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,flop,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,flop,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes oop,turn,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes ip,turn,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,allin";
			ofs << sLine << endl;
		}
		else if (AbnormalSizes.size() == 2) { //ip raisesize
			sLine = "set_bet_sizes oop,flop,bet," + double2String(AbnormalSizes[0], 1);
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,flop,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,flop,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes ip,flop,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,flop,raise," + double2String(AbnormalSizes[1], 1);;
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,flop,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes oop,turn,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes ip,turn,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,allin";
			ofs << sLine << endl;
		}
		else {
			cout << "error:AbnormalSizes cout not 1 or 2" << endl;
			return "";
		}
	}
	else if (round == turn) {
		if (AbnormalSizes.size() == 1) { // oop betsize
			sLine = "set_bet_sizes oop,turn,bet," + double2String(AbnormalSizes[0], 1);
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes ip,turn,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,allin";
			ofs << sLine << endl;
		}
		else if (AbnormalSizes.size() == 2) { //ip raisesize
			sLine = "set_bet_sizes oop,turn,bet," + double2String(AbnormalSizes[0], 1);
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,raise,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes oop,turn,allin";
			ofs << sLine << endl;

			sLine = "set_bet_sizes ip,turn,bet,100";
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,raise," + double2String(AbnormalSizes[1], 1);;
			ofs << sLine << endl;
			sLine = "set_bet_sizes ip,turn,allin";
			ofs << sLine << endl;
		}
		else {
			cout << "error:AbnormalSizes cout not 1 or 2" << endl;
			return "";
		}
	}
	else {
		cout << "error:MakeConfigFileSimple not flop && turn" << endl;
		return "";
	}
		
	sLine = "set_bet_sizes oop,river,bet,100";
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,river,raise,100";
	ofs << sLine << endl;
	sLine = "set_bet_sizes oop,river,allin";
	ofs << sLine << endl;

	sLine = "set_bet_sizes ip,river,bet,100";
	ofs << sLine << endl;
	sLine = "set_bet_sizes ip,river,raise,100";
	ofs << sLine << endl;
	sLine = "set_bet_sizes ip,river,allin";
	ofs << sLine << endl;

	//set_allin_threshold
	sLine = "set_allin_threshold " + double2String(m_fDefaultAllinThreshold, 2);
	ofs << sLine << endl;
	//set_raise_limit
	sLine = "set_raise_limit " + to_string(m_nDefaultRaiseLimit);
	ofs << sLine << endl;
	//build_tree
	sLine = "build_tree";
	ofs << sLine << endl;
	//set_thread_num
	sLine = "set_thread_num " + to_string(m_nDefaultThreadNum);
	ofs << sLine << endl;
	//set_accuracy
	sLine = "set_accuracy " + double2String(m_fDefaultAccuracySimple, 1);
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
	//sLine = "dump_result " + sConfigFolder + sGameID + ".json";
	sLine = "dump_result " + CDataFrom::GetRTSolverFilePath(sGameID);

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

bool CSolver::ToSolveSimple(const vector<double>& AbnormalSizes, const Round round, const string& sGameID, const CSolverConfig& config, Json::Value& result, const SolverCalcMode calcMode)
{
	string sConfigPath = MakeConfigFileSimple(sGameID, round, AbnormalSizes, config);
	string sResultFilePath = CDataFrom::GetRTSolverFilePath(sGameID);

	//to solve


//for test
	sResultFilePath = "D:\\VCCode\\ConsoleMCow\\SolverConfigs\\RTResult_.json";

	//重载m_solverResult
	if (!LoadSolverFile(sResultFilePath, result))
		return false;

	//删除临时文件
	//remove(sConfigPath.c_str());
	//remove(sResultFilePath.c_str());

	return true;
}

bool CSolver::LoadSolverFile(const std::string sPath, Json::Value& result, bool isNeedDecompress)
{
	if (isNeedDecompress) {
		std::ifstream is{ sPath, std::ios::binary | std::ios::ate };
		if (!is.is_open()) {
			cout << "error:solver_file open failed，  " << sPath << endl;
			return false;
		}

		auto size = is.tellg();
		std::string str(size, '\0'); // construct string to stream size
		is.seekg(0);
		is.read(&str[0], size);
		is.close();

		std::vector<char> decompressed;
		decompress(str, decompressed);
		string s3;
		s3.assign((const char*)decompressed.data(), decompressed.size());

		stringstream ss;
		ss << s3;

		JSONCPP_STRING errs;
		Json::CharReaderBuilder builder;
		result.clear();
		if (!parseFromStream(builder, ss, &result, &errs)) {
			is.close();
			return false;
		}
	}
	else {
		ifstream ifs;
		ifs.open(sPath);
		if (ifs.is_open()) {
			JSONCPP_STRING errs;
			Json::CharReaderBuilder builder;
			result.clear();
			if (!parseFromStream(builder, ifs, &result, &errs)) {
				ifs.close();
				return false;
			}
			ifs.close();
		}
		else {
			cout << "error:solver_file open failed，  " << sPath << endl;
			return false;
		}
	}

	return true;

}

void CSolver::compress(const string& in, std::vector<char>& out)
{
	using namespace boost::iostreams;
	filtering_ostream fos;    // 具有filter功能的输出流
	//fos.push(bzip2_compressor());// gzip_compressor(gzip_params(gzip::best_compression)));  // gzip压缩功能
	fos.push(lzma_compressor(lzma_params(lzma::best_compression)));
	fos.push(boost::iostreams::back_inserter(out));     // 输出流的数据的目的地
	fos.write((const char*)(in.c_str()), in.length());
	boost::iostreams::close(fos);  // flush. 此函数调用后,存储的是gzip压缩后的数据
}

void CSolver::decompress(const string& in, std::vector<char>& out)
{
	using namespace boost::iostreams;
	filtering_ostream fos;    // 具有filter功能的输出流
	//fos.push(bzip2_compressor());// gzip_compressor(gzip_params(gzip::best_compression)));  // gzip压缩功能
	fos.push(lzma_decompressor(lzma_params(lzma::best_compression)));
	fos.push(boost::iostreams::back_inserter(out));     // 输出流的数据的目的地
	fos.write((const char*)(in.c_str()), in.length());
	boost::iostreams::close(fos);  // flush. 此函数调用后,存储的是gzip压缩后的数据
}

// 编码
bool CSolver::Base64Encode(const string& input, string& output)
{
	std::size_t len = input.size();
	output.resize(boost::beast::detail::base64::encoded_size(len));
	output.resize(boost::beast::detail::base64::encode(&output[0], input.c_str(), len));

	return true;
}

bool CSolver::Base64Decode(const string& input, string& output)
{
	std::size_t len = input.size();
	output.resize(boost::beast::detail::base64::decoded_size(len));
	output.resize(boost::beast::detail::base64::decode(&output[0], input.c_str(), len).first);
	return true;
}
