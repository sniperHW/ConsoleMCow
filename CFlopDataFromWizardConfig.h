#pragma once

#ifndef CFLOP_DATA_FROM_WIZARD_H_
#define CFLOP_DATA_FROM_WIZARD_H_

#include "globledef.h"


class CFlopDataFromWizardConfig
{
public:
	bool Init(GameType gmType);
	bool IsFrWizard(const std::string sAbbrName);

private:
	std::set<std::string> m_sAbbrNames;
};

#endif