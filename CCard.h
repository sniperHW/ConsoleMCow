#pragma once

#ifndef CCARD_H_
#define CCARD_H_

#include "globledef.h"

class CCard
{
public:
	
	int m_nCardPoint;	// ����
	Suit m_cardSuit;	// ��ɫ
	std::string m_sSymbol;	// ��ʶ,����Ah��

	CCard();
	CCard(const std::string& sSymbol);	//���캯��
};

#endif // !CCCARD_H_