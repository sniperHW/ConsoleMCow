#pragma once

#ifndef CSOLVER_H_
#define CSOLVER_H_

#include "globledef.h"
#include "json/json.h"
#include "CRange.h"
#include "CStrategyTreeConfig.h"
#include <net2/net.h>
#include <mutex>


//using json = nlohmann::json;
//#include <list>


class CSolverTask
{
public:
	std::string m_sGameID;
	time_t m_tBeginTm;
	time_t m_tBeginSolveTm;
};
	 
class CSolverNode
{
public:
	void Send(const net::Buffer::Ptr &buff) {
		mtx.lock();
		conn->Send(buff);
		mtx.unlock();
	}

	bool m_isBusy;
	std::string m_sRunningTaskGameID;
	time_t m_tLastConnect;
	std::mutex mtx;
	net::Connection::Ptr conn;
};

class CSolverConfig
{
public:
	Stacks m_stacks;
	std::string m_sBoard;
	CRange* m_pRange;
	std::shared_ptr<CRTTreeConfigItem> m_pRTTreeConfigItem;
};

class CSolver
{
public:
	bool ToSolve(const std::string& sGameID, const CSolverConfig & config);
	bool GetSolverResult(const std::string& sGameID, Json::Value& result);
	void ToStop(int nGameID); //停止运算，当turn对抗结束，solver还没除结果时，目前不用

	void AdjustTree(CSolverConfig& config, const recalc_actionline recalcMode, const int& nBetBeforeBigRaise, const int& nRaiseSize); ////当常用size需要重新解算时，按模式调整m_pRTTreeConfigItem,dBetBeforeBigRaise,dRaiseSize只有bigraise模式才用
	static recalc_actionline IsRecalcActionline(const double dPot, const std::string& sActionLine, double& dBetBeforeBigRaise, double& dRaiseSize); //判断是否为turn需要转实时计算的行动线(srp不用判断)，为bigraise模式时填写dBetBeforeBigRaise和dRaiseSize

	bool Init();
	static bool LoadSolverFile(const std::string sPath, Json::Value& result, bool isNeedDecompress = false);

	void onNodeHeartBeat(const net::Connection::Ptr &conn,const Json::Value& packet);
	void onCommitTask(const net::Connection::Ptr &conn,const Json::Value& packet);

private:
	int m_nDefaultThreadNum = 64;
	int m_nDefaultMaxIteration = 800;
	int m_nDefaultRaiseLimit = 5;
	float m_fDefaultAllinThreshold = (float)0.67; 
	float m_fDefaultAccuracy = (float)0.15; 


	std::shared_ptr<CSolverNode> getSolverNode(const std::string &id) {
		std::lock_guard guard(nodesMtx);
		auto it = m_solverNodes.find(id);
		if(it == m_solverNodes.end()) {
			return nullptr;
		} else {
			return it->second;
		}
	}

	std::mutex nodesMtx;
	std::map<std::string, std::shared_ptr<CSolverNode>> m_solverNodes; //key:gameID
	
	//std::mutex taskMtx;
	//std::list<CSolverTask> m_SolverTasks; //key:ip

	std::mutex finishMtx;
	std::set<std::string> m_FinishedTasks; //key:gameID

	bool AssignTask(const std::string& sGameID, const std::shared_ptr<CSolverNode> &node);
	void DispatchTask(CSolverTask *task);//触发：新任务，有任务完成，增加计算节点
	void OnTimerCheck(); //超时节点删除
	//void AddSolverNode(const std::string& sIP); //加入新的计算节点
	//void ReceiveSolverResult(const std::string& sGameID, const std::string& sIP); //收到解算结果

	std::string MakeConfigFile(const std::string& sGameID, const CSolverConfig& config); //返回配置文件路径(默认以GameID命名),recalc_actionline!=recalc_none时按预设tree

	double static GetboostRatio(const double dlSrc);
	std::string GetSplitBoard(const std::string& sBoardOld);

	static void compress(const std::string& in, std::vector<char>& out);
	static void decompress(const std::string& in, std::vector<char>& out);
	static bool Base64Encode(const std::string& input, std::string& output);
	static bool Base64Decode(const std::string& input, std::string& output);
};

#endif
