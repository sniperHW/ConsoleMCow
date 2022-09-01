#pragma once

#ifndef CDATAFROM_H_
#define CDATAFROM_H_

#include "globledef.h"

class CDataFrom
{
public:

	std::string GetWizardFilePath(GameType gmType, const std::string & sNodeName, int nStackDepth, Round round);
	std::string GetSolverdFilePath(GameType gmType, const std::string & sNodeName, int nStackDepth, Round round);
	std::string GetRangesFilePath(GameType gmType, const std::string & sNodeName, int nStackDepth, Round round);

private:
	const GameType& m_gmType;
};

#endif