#pragma once

#ifndef CBOARD_H_
#define CBOARD_H_

#include "globledef.h"
#include "CCard.h"

//取wizard策略、wizard范围、solver预存解需要用同构board去匹配（只转flop的）
class CBoard
{
public:
	std::vector<CCard>m_flopCards;

private:
	std::string m_sIsomorphismBoard;
	SuitReplace m_suitReplace;

public:
	void SetFlop(const std::string & sSymbol); //同时设置同构面
	void SetTurn(const std::string & sSymbol); //同时设置同构面
	void SetRiver(const std::string & sSymbol);
	std::string GetBoardSymbol(); //按round拼接
	std::string GetIsoBoardSymbol(); //flop为Iso，turn和river按原样（目前没用）

	const std::string & GetIsomorphismSymbol(); //同构后的标记
	const SuitReplace & GetIsomorphismSuitReplace(); //花色替换
	void ClearSuitReplace();
private:
	void ConvetIsomorphism();  //取得board的同构面，填写m_sIsomorphismBoard和m_suitReplace
	void SortThreeCard(char* csCards);
	void CompareAndSwapCard(char* cCard1, char* cCard2);
	void CompareAndSwapCard(CCard& cCard1, CCard& cCard2);

	//三张牌同一个花色的同构转换
	void ConvertCardSuit1(char cFlopOri);
	//三张牌有两个花色时的同构转换，最复杂的一种
	void ConvertCardSuit2(const char* cFlopOri, const char* cFlopReplace);
	//三张牌三个花色时的同构转换
	void ConvertCardSuit3(const char* cFlopSuitOri);

	bool GetIsomorphismNodeName(const char* csFlopOri);
	bool RevertSuitReplace();
};

#endif