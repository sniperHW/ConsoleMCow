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
					v.push_back(sCard1 + a + sCard2 + b);
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
	string sAligned = Align(sCombo);
	string ret;

	if (sAligned[0] == sAligned[2]) { //pair
		ret.push_back(sAligned[0]);
		ret.push_back(sAligned[0]);
	}
	else if (sAligned[1] == sAligned[3]) { //s
		ret.push_back(sAligned[0]);
		ret.push_back(sAligned[2]);
		ret.push_back('s');
	}
	else { //o
		ret.push_back(sAligned[0]);
		ret.push_back(sAligned[2]);
		ret.push_back('o');
	}


	return ret;
}

void CCombo::SetHands(const string sCombo)
{
	m_sSymbol = Align(sCombo);

	CCard m_Card1;
	CCard m_Card2;

	string sCard1, sCard2;
	sCard1 = m_sSymbol.substr(0, 2);
	sCard2 = m_sSymbol.substr(2, 2);
	m_Card1.setCard(sCard1);
	m_Card2.setCard(sCard2);

	//顺序转为1326中一个

}

std::string CCombo::Align(const std::string sSrc)
{
	if (sSrc.size() != 4)
		return "";

	string r;
	map<char, int> charsOrdered{ {'A', 13}, { 'K',12 }, { 'Q',11 }, { 'J',10 }, { 'T',9 }, { '9',8 }, { '8',7 }, { '7',6 }, { '6',5 }, { '5',4 }, { '4',3 }, { '3',2 }, { '2',1 }};
	set<char> charsSuit{'h','d','c','s'};

	if (charsOrdered.find(sSrc[0]) == charsOrdered.end() || charsOrdered.find(sSrc[2]) == charsOrdered.end())
		return "";
	if (charsSuit.find(sSrc[1]) == charsSuit.end() || charsSuit.find(sSrc[3]) == charsSuit.end())
		return "";



	if (sSrc[0] == sSrc[2]) {	//对子
		if (!(sSrc[1] == 's' && sSrc[3] == 'h' || sSrc[1] == 's' && sSrc[3] == 'd' || sSrc[1] == 'h' && sSrc[3] == 'd' || sSrc[1] == 's' && sSrc[3] == 'c' || sSrc[1] == 'h' && sSrc[3] == 'c' || sSrc[1] == 'd' && sSrc[3] == 'c')) {
			r.append(1, sSrc[0]);
			r.append(1, sSrc[3]);
			r.append(1, sSrc[2]);
			r.append(1, sSrc[1]);
		}
		else
			r = sSrc;
	}
	else {	
		if(charsOrdered[sSrc[0]] > charsOrdered[sSrc[2]])
			r = sSrc;
		else {
			r.append(1, sSrc[2]);
			r.append(1, sSrc[1]);
			r.append(1, sSrc[0]);
			r.append(1, sSrc[3]);
		}
	}
	return r;
}

