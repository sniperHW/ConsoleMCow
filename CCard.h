#pragma once

#ifndef CCARD_H_
#define CCARD_H_

#include "globledef.h"

class CCard
{
public:
	
	int m_nCardPoint;	// 点数
	Suit m_cardSuit;	// 花色
	std::string m_sSymbol;	// 标识,例“Ah”

	CCard();
	CCard(const std::string& sSymbol);	//构造函数
};

#endif // !CCCARD_H_