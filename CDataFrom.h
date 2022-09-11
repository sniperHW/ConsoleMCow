#pragma once

#ifndef CDATAFROM_H_
#define CDATAFROM_H_

#include "globledef.h"

class CDataFrom
{
public:

	std::string GetWizardFilePath(GameType gmType, const std::string & sNodeName, Round round);
	std::string GetSolverdFilePath(GameType gmType, const std::string & sNodeName, Round round);
	std::string GetRangesFilePath(GameType gmType, const std::string & sNodeName, Round round);

private:
	
};

#endif