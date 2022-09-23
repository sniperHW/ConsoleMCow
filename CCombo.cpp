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
	
	if(sAbbr == "AA"){
		for(auto it = ComboMapping.begin();it != ComboMapping.end();it++) {
			if((*it)[0] == (*it)[1]) {
				combos.push_back(*it);
			}			
		}
	}



	if(sAbbr == "AKo" || sAbbr == "AK") {
		for(auto it = ComboMapping.begin();it != ComboMapping.end();it++) {
			if(getColor((*it)[1]) != getColor((*it)[3])){
				combos.push_back(*it);
			}
		}
	}


	if(sAbbr == "AKs" || sAbbr == "AK") {
		for(auto it = ComboMapping.begin();it != ComboMapping.end();it++) {
			if(getColor((*it)[1]) == getColor((*it)[3])){
				combos.push_back(*it);
			}
		}
	}

	return combos;
}


string CCombo::GetAbbrSymble(const string sCombo)
{
	if(sCombo[0] == sCombo[2]) {
		return "AA";
	}

	if(getColor(sCombo[1]) == getColor(sCombo[3])) {
		return "AKs";
	}else{
		return "AKo";
	}	
}

void CCombo::SetHands(const string sCombo)
{

}