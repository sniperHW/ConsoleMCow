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
	void setCard(const std::string& sSymbol);
	static void GetCardDetail(int& nCardPoint, Suit& cardSuit, const std::string& sSymbol);
	std::string GetCardSymbol();

	static Suit  SuitFromChar(char cCardSuit);
	static  char PointFromChar(char cCard);
	static  char CharFromPoint(int iCard);
	static char CharacterFromsuit(Suit cCardSuit);
};

#endif // !CCCARD_H_