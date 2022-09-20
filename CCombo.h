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

	static std::vector<std::string> GetCombosByAbbr(const std::string& sAbbr); //AKo�����ͬɫ��AKs����ͬɫ��AA������ӣ�AK����AKo+AKs��
	static std::string GetAbbrSymble(const std::string sCombo); //AKo�����ͬɫ��AKs����ͬɫ��AA�������,��������֮һ
};

#endif