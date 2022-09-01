#pragma once

#ifndef CSOLVER_H_
#define CSOLVER_H_
#include "CSolverConfig.h"

class CSolver
{
public:
	int ToSolve(int nGameID, const CSolverConfig & config, Json::Value & result); //����δ����
	void ToStop(int nGameID); //ֹͣ���㣬��turn�Կ�������solver��û�����ʱ

private:
	int m_nDefaultThreadNum;
	int m_nDefaultMaxIteration;
	int m_nDefaultRaiseLimit;
	float m_fDefaultAllinThreshold; //0.85
	float m_fDefaultAccuracy; //0.5
	//Ⱥ�����

	void MakeConfigFile(int nGameID, const CSolverConfig& config);
};

#endif
