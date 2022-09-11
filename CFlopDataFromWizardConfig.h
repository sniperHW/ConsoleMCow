#pragma once

#ifndef CFLOP_DATA_FROM_WIZARD_H_
#define CFLOP_DATA_FROM_WIZARD_H_

#include "globledef.h"


class CFlopDataFromWizardConfig
{
public:
	bool Init(GameType gmType);
	bool IsFrWizard(const std::string sPreflopType);

private:
	std::set<std::string> m_preflopTypes;
};

#endif