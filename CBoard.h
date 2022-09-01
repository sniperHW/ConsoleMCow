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
	void SetFlop(const std::string & sSymbol); //同时设置同构面
	void SetTurn(const std::string & sSymbol); //同时设置同构面
	void SetRiver(const std::string & sSymbol);
	std::string GetBoardSymbol(); //按round拼接

	void ConvetIsomorphism();  //取得board的同构面，填写m_sIsomorphismBoard和m_suitReplace
	const std::string & GetIsomorphismSymbol(); //同构后的标记
	const SuitReplace & GetIsomorphismSuitReplace(); //花色替换
};

#endif