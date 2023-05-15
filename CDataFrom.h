#pragma once

#ifndef CDATAFROM_H_
#define CDATAFROM_H_

#include "globledef.h"

class CDataFrom
{
public:

	static std::string GetWizardFilePath(GameType gmType, const std::string & sNodeName);
	static std::string GetSolverFilePath(GameType gmType, const std::string & sNodeName);
	static std::string GetRangesFilePath(GameType gmType, const std::string & sNodeName);
	static std::string GetStrategyFilePath(GameType gmType, const std::string& sNodeName);	//�Զ����ʽ
	static std::string GetRTSolverFilePath(const std::string& sGameID);

private:
	
};

#endif