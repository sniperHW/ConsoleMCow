#pragma once

#ifndef CSOLVER_H_
#define CSOLVER_H_

#include "globledef.h"
#include "json/json.h"
#include "CRange.h"
#include "CStrategyTreeConfig.h"

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
	bool ToSolve(const std::string& sGameID, const CSolverConfig & config, Json::Value & result, const SolverCalcMode calcMode = calc_sync); //返回需要定义
	void ToStop(int nGameID); //停止运算，当turn对抗结束，solver还没除结果时

private:
	int m_nDefaultThreadNum;
	int m_nDefaultMaxIteration;
	int m_nDefaultRaiseLimit;
	float m_fDefaultAllinThreshold; //0.85
	float m_fDefaultAccuracy; //0.5
	//群集相关

	void MakeConfigFile(int nGameID, const CSolverConfig& config);
};

#endif
