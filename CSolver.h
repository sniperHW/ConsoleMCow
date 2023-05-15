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

	//simple solve(AbnormalSize为超纲，一个为oop bet,两个为ip raise，其他都用100)
	bool ToSolveSimple(const std::vector<double>& AbnormalSizes, const Round round, const std::string& sGameID, const CSolverConfig& config, Json::Value& result, const SolverCalcMode calcMode = calc_sync);
	bool LoadSolverFile(const std::string sPath, Json::Value& result);

private:
	int m_nDefaultThreadNum = 64;
	int m_nDefaultMaxIteration = 500;
	int m_nDefaultRaiseLimit = 5;
	float m_fDefaultAllinThreshold = (float)0.67; //0.85
	float m_fDefaultAccuracy = (float)0.5; //0.5？
	float m_fDefaultAccuracySimple = (float)5.0; //0.5？

	//群集相关

	std::string MakeConfigFile(const std::string& sGameID, const CSolverConfig& config); //返回配置文件路径
	std::string MakeConfigFileSimple(const std::string& sGameID, const Round round, const std::vector<double>& AbnormalSizes, const CSolverConfig& config);
	double static GetboostRatio(const double dlSrc);
	std::string GetSplitBoard(const std::string& sBoardOld);
};

#endif
