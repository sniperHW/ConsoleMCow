#pragma once

#ifndef CBOARD_H_
#define CBOARD_H_

#include "globledef.h"
#include "CCard.h"

class CBoard
{
public:
	std::array<CCard, 3>m_flopCards;
	CCard m_turnCard;
	CCard m_riverCard;

	std::string m_sFlop;
	std::string m_sTurn;
	std::string m_sRiver;

	Round m_curRound;

private:
	std::string m_sIsomorphismBoard;
	SuitReplace m_suitReplace;

public:
	void SetFlop(const std::string & sSymbol); //ͬʱ����ͬ����
	void SetTurn(const std::string & sSymbol); //ͬʱ����ͬ����
	void SetRiver(const std::string & sSymbol);
	std::string GetBoardSymbol(); //��roundƴ��

	void ConvetIsomorphism();  //ȡ��board��ͬ���棬��дm_sIsomorphismBoard��m_suitReplace
	const std::string & GetIsomorphismSymbol(); //ͬ����ı��
	const SuitReplace & GetIsomorphismSuitReplace(); //��ɫ�滻
};

#endif