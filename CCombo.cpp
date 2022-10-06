//#include "pch.h"
#include "CCombo.h"

using namespace std;


vector<string> CCombo::GetCombosByAbbr(const string& sAbbr)
{
	return vector<string>{};
}

string CCombo::GetAbbrSymble(const string sCombo)
{
	return "";
}

void CCombo::SetHands(const string sCombo)
{
	m_sSymbol = sCombo;

	CCard m_Card1;
	CCard m_Card2;

	string sCard1, sCard2;
	sCard1 = m_sSymbol.substr(0, 2);
	sCard1 = m_sSymbol.substr(2, 2);
	m_Card1.setCard(sCard1);
	m_Card2.setCard(sCard2);

	//顺序转为1326中一个

}

