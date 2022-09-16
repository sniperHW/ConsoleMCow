#pragma once

#ifndef CDATAFROM_H_
#define CDATAFROM_H_

#include "globledef.h"

class CDataFrom
{
public:

	static std::string GetWizardFilePath(GameType gmType, const std::string & sNodeName, Round round);
	static std::string GetSolverFilePath(GameType gmType, const std::string & sNodeName, Round round);
	static std::string GetRangesFilePath(GameType gmType, const std::string & sNodeName);

private:
	
};

#endif