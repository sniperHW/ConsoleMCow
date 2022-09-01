#pragma once

#ifndef CSOLVER_H_
#define CSOLVER_H_
#include "CSolverConfig.h"

class CSolver
{
public:
	int ToSolve(int nGameID, const CSolverConfig & config, Json::Value & result); //返回未定义
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
