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
#include "CStrategy.h"
#include <thread>
#include <chrono>
#include <io.h>


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

	return true;
}

bool CSolver::ToSolve(const string& sGameID, const CSolverConfig& config)
{
	string sConfigPath = MakeConfigFile(sGameID, config);
	CSolverTask newTask;
	newTask.m_sGameID = sGameID;
	time(&newTask.m_tBeginTm);
	DispatchTask(&newTask);
	return true;
}

//目前不需要
void CSolver::ToStop(int nGameID)
{

}

//循环等待查询结果，超时返回false
bool CSolver::GetSolverResult(const std::string& sGameID, Json::Value& result)
{
	int nWaitingSeconds = 0;
	cout << "waiting solver result,GameID:" << sGameID;
	while (true) {
		finishMtx.lock();
		auto it = m_FinishedTasks.find(sGameID);
		if(it != m_FinishedTasks.end()){
			m_FinishedTasks.erase(it);
			finishMtx.unlock();

			string sResultFilePath = CDataFrom::GetRTSolverFilePath(sGameID);
			string sConfigFilePath = CDataFrom::GetRTSolverConfigFilePath(sGameID);
			bool r = LoadSolverFile(sResultFilePath, result);

			//删除结果文件和配置文件
			if (r) {
				remove(sResultFilePath.c_str());
				remove(sConfigFilePath.c_str());
				cout << "sucess" << endl;
			}
			else
				cout << "RT LoadSolverFile failed" << endl;

			return r;
		}
		else{
			finishMtx.unlock();
			nWaitingSeconds++;
			if (nWaitingSeconds > SOLVE_OVERTIME) {
				cout << "overtime" << endl;
				return false;
			}
			else {
				std::this_thread::sleep_for(std::chrono::seconds(1));
				cout << ".";
			}
		}
	}

	return true;
}

//(bigraise)spr=1000	//	turn=bet:50 100; raise:50 100	//	river=bet:50 100; raise:50 100; donk:50
//1)按自己bet的size替换turn betsize
//2)turn加raise的size
void CSolver::AdjustTree(CSolverConfig& config, const recalc_actionline recalcMode, const int& nBetBeforeBigRaise, const int& nRaiseSize)
{
	if (recalcMode != recalc_raisebig)
		return;

	config.m_pRTTreeConfigItem->m_turnBet = possible_bets.at(nBetBeforeBigRaise);
	config.m_pRTTreeConfigItem->m_turnRaise.push_back(nRaiseSize);
}

//{ recalc_bet66, recalc_bet100, recalc_raisebig, recalc_none }recalc_actionline
//action size = 1, 判断recalc_bet66, recalc_bet100
//action size = 2，判断recalc_bet66, recalc_bet100, recalc_raisebig, recalc_none
recalc_actionline CSolver::IsRecalcActionline(const double dPot, const std::string& sActionLine, double& dBetBeforeBigRaise, double& dRaiseSize)
{
	vector<Action> actionSquence = {};
	string sPrefix = "";
	Round round;
	string actionStr = "";
	CStrategy::parseActionSquence(sActionLine, sPrefix, round, actionSquence, actionStr);

	if (actionSquence.size() < 1 || actionSquence.size() > 2)
		return recalc_none;

	if (actionSquence.size() == 1) {
		if(actionSquence[0].actionType != raise)
			return recalc_none;

		double dRatio = CStrategy::CalcBetRatio(dPot, actionSquence, 0, 0);
		if (abs(dRatio - 66) < 3)
			return recalc_bet66;
		else if(abs(dRatio - 100) < 3)
			return recalc_bet100;
	}
	else if(actionSquence.size() == 2){
		if (actionSquence[1].actionType != raise)
			return recalc_none;

		if (actionSquence[0].actionType == check) {
			double dRatio = CStrategy::CalcBetRatio(dPot, actionSquence, 1, 0);
			if (abs(dRatio - 66) < 3)
				return recalc_bet66;
			else if (abs(dRatio - 100) < 3)
				return recalc_bet100;
		}
		else if (actionSquence[0].actionType == raise) {
			double dBetRatio = CStrategy::CalcBetRatio(dPot, actionSquence, 0, 0);
			double dRaiseRatio = CStrategy::CalcBetRatio(dPot, actionSquence, 1, 0);
			if (dRaiseRatio > 120) {
				dBetBeforeBigRaise = dBetRatio;
				dRaiseSize = dRaiseRatio;
				return recalc_raisebig;
			}
		}
	}

	return recalc_none;
}

//指派一个任务
bool CSolver::AssignTask(const string& sGameID, const CSolverNode *node)
{
	string sConfigFilePath = CDataFrom::GetRTSolverConfigFilePath(sGameID);
    if (std::ifstream is{ homepath + task->taskID + ".json", std::ios::binary | std::ios::ate}) {
        auto size = is.tellg();
        std::string cfg(size, '\0'); // construct string to stream size
        is.seekg(0);
        if (is.read(&str[0], size))
            logfile << __FILE__ << ":" << __LINE__ << " read result ok commitTaskRoutine task:" << task->taskID << endl;
        else
            logfile << __FILE__ << ":" << __LINE__ << " read result failed commitTaskRoutine task:" << task->taskID << endl;
        is.close();
	

		Json::Value value;
		value["sGameID"] = gameID;
		value["Cfg"] = cfg;
		std::stringstream ss;
		ss << value;
		auto jsonStr = ss.str();
		auto packet = net::Buffer::New(6 + jsonStr.length());
		packet->Append(uint32_t(2 + jsonStr.length()));
		packet->Append(uint16_t(2));
		packet->Append(jStr);
		node->conn->Send(packet);
	}

	return true;
}


//调度任务执行
void CSolver::DispatchTask(CSolverTask *task)
{
	std::lock_guard guard(nodesMtx);
	for (auto oneSolverNode : m_solverNodes) {
		oneSolverNode.second.lock();
		if(oneSolverNode.second.m_isBusy) {
			continue;
		}
		if(AssignTask(task->m_sGameID,&oneSolverNode.second)) {
			cout << "DispatchTask sucess. IP:" << oneSolverNode.first << " GameID:" << sGameID << endl;
			oneSolverNode.second.m_isBusy = true;
			oneSolverNode.second.m_sRunningTaskGameID = sGameID;
			oneSolverNode.second.unlock();
			return;
		} else {
			cout << "DispatchTask failed. IP:" << oneSolverNode.first << endl;
		}
		oneSolverNode.second.unlock();
		m_solverNodes.erase(oneSolverNode);
	}
}

//删除120秒无握手的节点
void CSolver::OnTimerCheck()
{
	time_t tCurTm;
	time(&tCurTm);
	std::lock_guard guard(this->nodesMtx);
	for (auto it = m_solverNodes.begin(); it != m_solverNodes.end(); ) {
		if (difftime(it->second.m_tLastConnect, tCurTm) > 120) {
			it = m_solverNodes.erase(it); 
		}
		else {
			++it;
		}
	}
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

void CSolver::onNodeHeartBeat(const net::Connection::Ptr &conn,const Json::Value& packet)
{
	auto workerID = packet["WorkerID"].asString();
	auto tasks = packet["Tasks"];
	conn->SetUserData(workerID);
	this->nodesMtx.lock();
	auto it = this->m_solverNodes.find(workerID);
	if(it!=this->m_solverNodes.end()) {
		it->second.mtx.lock();
		it->second.conn = conn;
		time(&it->second.m_tLastConnect);
		if(tasks.size() > 0) {
			it->second.m_isBusy = true;
			auto first = tasks[0];
			it->second.m_sRunningTaskGameID = first["TaskID"].asString();
		} else {
			it->second.m_isBusy = false;
			it->second.m_sRunningTaskGameID = "";
		}
		it->second.mtx.unlock();
	} else {
		CSolverNode node;
		node.conn = conn;
		time(&node.m_tLastConnect);
		if(tasks.size() > 0) {
			node.m_isBusy = true;
			auto first = tasks[0];
			node.m_sRunningTaskGameID = first["TaskID"].asString();
		}
		this->m_solverNodes[workerID] = node;
	}
	this->nodesMtx.unlock();
}


net::Buffer::Ptr makeAcceptJobPacket(const std::string &gameID) {
	Json::Value value;
	value["TaskID"] = gameID;
	std::stringstream ss;
	ss << value;
	auto jsonStr = ss.str();
	auto packet = net::Buffer::New(6 + jsonStr.length());
    packet->Append(uint32_t(2 + jsonStr.length()));
    packet->Append(uint16_t(4));
    packet->Append(jStr);
	return packet;
}

net::Buffer::Ptr makeCancelJobPacket(const std::string &gameID) {
	Json::Value value;
	value["TaskID"] = gameID;
	std::stringstream ss;
	ss << value;
	auto jsonStr = ss.str();
	auto packet = net::Buffer::New(6 + jsonStr.length());
    packet->Append(uint32_t(2 + jsonStr.length()));
    packet->Append(uint16_t(5));
    packet->Append(jStr);
	return packet;
}


void CSolver::onCommitTask(const net::Connection::Ptr &conn,const Json::Value& packet)
	auto sGameID = packet["TaskID"].asString();
	auto result = packet["Result"].asString();
	std::any ud = conn->GetUserData();
	auto workerID = std::any_cast<std::string>(ud);
	auto node = getSolverNodes(workerID);
	if(node == nullptr) {
		//通告取消任务
		conn->Send(makeCancelJobPacket(sGameID));
		return;
	}
	auto ok = false;
	node->mtx.lock();
	if(node->m_sRunningTaskGameID == sGameID) {
		ok = true;
	} 
	node->mtx.unlock();
	if(!ok) {
		//通告取消任务
		conn->Send(makeCancelJobPacket(sGameID));
		return;
	}

	//通告接受任务
	conn->Send(makeAcceptJobPacket(sGameID));

	//将result保存到文件
	string sResultFilePath = CDataFrom::GetRTSolverFilePath(sGameID);
	std::ofstream ofs;
	ofs.open(sResultFilePath, std::ios::out);
	ofs <<result;
	ofs.close();


	finishMtx.lock();
	m_FinishedTasks.insert(sGameID);
	finishMtx.unlock();
}
