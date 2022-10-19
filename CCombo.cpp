//#include "pch.h"
#include "CCombo.h"

using namespace std;


//0:红色,1:黑色
static int getColor(char c){
	if(c == 'd' || c =='h') {
		return 0;
	} else {
		return 1;
	}
}

vector<string> CCombo::GetCombosByAbbr(const string& sAbbr)
{
	string sCard1, sCard2;
	array<string, 4> sSuits{ "s","h","d","c" };
	vector<string> v;

	if (sAbbr.size() == 2) {
		sCard1 = sAbbr[0];
		sCard2 = sAbbr[1];
		if (sCard1 == sCard2) {	//pair
			v.push_back(sCard1 + "s" + sCard1 + "h");
			v.push_back(sCard1 + "s" + sCard1 + "d");
			v.push_back(sCard1 + "h" + sCard1 + "d");
			v.push_back(sCard1 + "s" + sCard1 + "c");
			v.push_back(sCard1 + "h" + sCard1 + "c");
			v.push_back(sCard1 + "d" + sCard1 + "c");
		}
		else {	//AK
			for (auto a : sSuits) {
				for (auto b : sSuits)
					v.push_back(sCard1 + a + sCard1 + b);
			}
		}
	}
	else {
		sCard1 = sAbbr[0];
		sCard2 = sAbbr[1];
		if (sAbbr[2] == 'o') {	//o
			v.push_back(sCard1 + "s" + sCard2 + "h");
			v.push_back(sCard1 + "s" + sCard2 + "d");
			v.push_back(sCard1 + "s" + sCard2 + "c");
			v.push_back(sCard1 + "h" + sCard2 + "s");
			v.push_back(sCard1 + "h" + sCard2 + "d");
			v.push_back(sCard1 + "h" + sCard2 + "c");
			v.push_back(sCard1 + "d" + sCard2 + "s");
			v.push_back(sCard1 + "d" + sCard2 + "h");
			v.push_back(sCard1 + "d" + sCard2 + "c");
			v.push_back(sCard1 + "c" + sCard2 + "s");
			v.push_back(sCard1 + "c" + sCard2 + "h");
			v.push_back(sCard1 + "c" + sCard2 + "d");
		}
		else if (sAbbr[2] == 's') {	//s
			v.push_back(sCard1 + "s" + sCard2 + "s");
			v.push_back(sCard1 + "h" + sCard2 + "h");
			v.push_back(sCard1 + "d" + sCard2 + "d");
			v.push_back(sCard1 + "c" + sCard2 + "c");
		}
	}

	return v;
}


string CCombo::GetAbbrSymble(const string sCombo)
{
	string ret;

	if (sCombo[0] == sCombo[2]) { //pair
		ret.push_back(sCombo[0]);
		ret.push_back(sCombo[0]);
	}
	else if (sCombo[1] == sCombo[3]) { //s
		ret.push_back(sCombo[0]);
		ret.push_back(sCombo[2]);
		ret.push_back('s');
	}
	else { //o
		ret.push_back(sCombo[0]);
		ret.push_back(sCombo[2]);
		ret.push_back('o');
	}


	return ret;
}

void CCombo::SetHands(const string sCombo)
{
	m_sSymbol = sCombo;

	CCard m_Card1;
	CCard m_Card2;

	string sCard1, sCard2;
	sCard1 = m_sSymbol.substr(0, 2);
	sCard2 = m_sSymbol.substr(2, 2);
	m_Card1.setCard(sCard1);
	m_Card2.setCard(sCard2);

	//顺序转为1326中一个

}

