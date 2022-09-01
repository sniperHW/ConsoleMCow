#pragma once

#ifndef CCOMBO_H_
#define CCOMBO_H_

#include "CCard.h"


class CCombo
{
public:
	CCard m_Card1;
	CCard m_Card2;
	std::string m_sSymbol;

	CCombo();
	CCombo(const std::string & sSymbol);
};

#endif