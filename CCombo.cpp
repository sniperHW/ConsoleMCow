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
	vector<string> combos;
	
	auto card1 = sAbbr[0];
	auto card2 = sAbbr[1];

	//对子
	if(card1==card2) {
		for(auto c : ComboMapping) {
			if(c[0] == c[2]) {
				combos.push_back(c);
			}			
		}
	} else {
		//AKo,AK
		
		if(sAbbr.size() == 2 || sAbbr[2] == 'o' ) {
			for(auto c : ComboMapping) {
				if(getColor(c[1]) != getColor(c[3])){
					if(c[0] == card1 && c[2] == card2) {
						combos.push_back(c);
					}
				}
			}
		}

		//AKs,AK
		if(sAbbr.size() == 2 || sAbbr[2] == 'o' ) {
			for(auto c : ComboMapping) {
				if(getColor(c[1]) == getColor(c[3])){
					if(c[0] == card1 && c[2] == card2) {
						combos.push_back(c);
					}
				}
			}				
		}
	}

	return combos;
}


string CCombo::GetAbbrSymble(const string sCombo)
{
	string ret;
	ret.push_back(sCombo[0]);
	ret.push_back(sCombo[2]); 

	if(sCombo[0] != sCombo[2]) {
		if(getColor(sCombo[1]) == getColor(sCombo[3])) {
			ret.push_back('s');
		}else{
			ret.push_back('o');
		}	
	}
	return ret;
}

void CCombo::SetHands(const string sCombo)
{

}