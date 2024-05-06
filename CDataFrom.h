#pragma once

#ifndef CDATAFROM_H_
#define CDATAFROM_H_

#include "globledef.h"

class CDataFrom
{
public:
	bool Init();

	static std::string GetWizardFilePath(GameType gmType, const std::string & sNodeName);//Ŀǰ����
	std::string GetSolverFilePath(GameType gmType, const std::string & sNodeName);
	std::string GetRangesFilePath(GameType gmType, const std::string & sNodeName);
	std::string GetStrategyFilePath(GameType gmType, const std::string& sNodeName);	//�Զ����ʽ
	static std::string GetRTSolverFilePath(const std::string& sGameID);

private:
	std::string m_sDataFolder;	//Data��Ŀ¼�������ڸ�Ŀ¼��
};

#endif