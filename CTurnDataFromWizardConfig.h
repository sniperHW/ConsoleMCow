#pragma once

#ifndef CTURN_DATA_FROM_WIZARD_H_
#define CTURN_DATA_FROM_WIZARD_H_

#include "globledef.h"


class CTurnDataFromWizardConfig
{
public:
	bool Init(GameType gmType);
	bool IsFrWizard(const std::string sPreflopType);

private:
	std::set<std::string> m_preflopTypes;
};

#endif