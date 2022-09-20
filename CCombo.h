#pragma once

#ifndef CCOMBO_H_
#define CCOMBO_H_

#include "CCard.h"
#include <vector>


class CCombo
{
public:
	CCard m_Card1;
	CCard m_Card2;
	std::string m_sSymbol;

	CCombo();
	CCombo(const std::string & sSymbol);

	static std::vector<std::string> GetCombosByAbbr(const std::string& sAbbr); //AKo代表非同色；AKs代表同色；AA代表对子，AK代表AKo+AKs。
	static std::string GetAbbrSymble(const std::string sCombo); //AKo代表非同色；AKs代表同色；AA代表对子,返回三种之一
};

#endif