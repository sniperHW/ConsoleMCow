#pragma once

#ifndef CDATAFROM_H_
#define CDATAFROM_H_

#include "globledef.h"

class CDataFrom
{
public:
	bool Init();

	static std::string GetWizardFilePath(GameType gmType, const std::string & sNodeName);//目前不用
	std::string GetSolverFilePath(GameType gmType, const std::string & sNodeName);
	std::string GetRangesFilePath(GameType gmType, const std::string & sNodeName);
	std::string GetStrategyFilePath(GameType gmType, const std::string& sNodeName);	//自定义格式
	static std::string GetRTSolverFilePath(const std::string& sGameID);

private:
	std::string m_sDataFolder;	//Data子目录不包含在该目录中
};

#endif