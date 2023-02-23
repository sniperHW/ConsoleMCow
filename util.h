#pragma once
#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include "globledef.h"

inline ActionType str2ActionType(const std::string &str, const std::string&code = "") {
	if(str == "CALL" || str == "call") {
		return call;
	} else if(str == "FOLD" || str == "fold"){
		return fold;
	} else if(str == "CHECK" ||str == "check"){
		return check;
	} else if(str == "BET" || str == "RAISE" || str == "bet" || str == "raise"){
		if (code == "RAI")
			return allin;
		else
			return raise;
	} else if (str == "ALLIN" || str == "allin") {
		return allin;
	}	else {
		return none;
	}
}

inline std::string actionType2String(ActionType a) {
	switch(a) {
		case call:
			return "CALL";
		case fold:
			return "FOLD";
		case raise:
			return "RAISE";
		case allin:
			return "ALLIN";
		case check:
			return "CHECK";
		default:
			return "NONE";
	}
}

inline std::string double2String(const double dlValue, const int nFixed) {
	char buffer[100];
#ifdef MAC
	switch (nFixed)
	{
	case 1:
		snprintf(buffer, sizeof(buffer), "%.1f", dlValue);
		break;
	case 2:
		snprintf(buffer, sizeof(buffer), "%.2f", dlValue);
		break;
	case 7:
		snprintf(buffer, sizeof(buffer), "%.7f", dlValue);
		break;
	default:
		snprintf(buffer, sizeof(buffer), "%.1f", dlValue);
		break;
	}
#else
	switch (nFixed)
	{
	case 1:
		sprintf_s(buffer, sizeof(buffer), "%.1f", dlValue);
		break;
	case 2:
		sprintf_s(buffer, sizeof(buffer), "%.2f", dlValue);
		break;
	case 7:
		sprintf_s(buffer, sizeof(buffer), "%.7f", dlValue);
		break;
	default:
		sprintf_s(buffer, sizeof(buffer), "%.1f", dlValue);
		break;
	}
#endif

	std::string sValue = buffer;
	auto p = sValue.end() - 1;
	while (*p == '0' || *p == '.')
	{
		if (*p == '.') {
			sValue.pop_back();
			break;
		}
		else {
			p--;
			sValue.pop_back();
		}
	}

	return sValue;
}

template <class Type>
Type stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

template <class Type>
bool compareBySymbol(const Type value, const CompareSymbol compare_symbol, const Type base_value)
{
	//cs_less, cs_less_equal,cs_equal,cs_large,cs_large_equal
	if (base_value == 0)
		return true;

	switch (compare_symbol) {
	case cs_less:
		return value < base_value;
	case cs_less_equal:
		return value <= base_value;
	case cs_equal:
		return value == base_value;
	case cs_large:
		return value > base_value;
	case cs_large_equal:
		return value >= base_value;
	}
	return false;
}

inline Round str2round(const std::string& sRound)
{
	if (sRound == "flop")
		return flop;
	else if(sRound == "turn")
		return turn;
	else if (sRound == "river")
		return river;
	else
		return preflop; 
}

//typedef enum { cs_less, cs_less_equal,cs_equal,cs_large,cs_large_equal }CompareSymbol;
inline CompareSymbol str2compareSymbol(const std::string& sSymbol)
{
	if (sSymbol == "<")
		return cs_less;
	else if (sSymbol == "<=")
		return cs_less_equal;
	else if (sSymbol == "=")
		return cs_equal;
	else if (sSymbol == ">")
		return cs_large;
	else if (sSymbol == ">=")
		return cs_large_equal;
	else
		return cs_equal;
}

//typedef enum { multi_front, multi_between, multi_back, multi_none }Multi_Position; //none代表不做判断
inline Multi_Position str2multiPosition(const std::string& sSymbol)
{
	if (sSymbol == "front")
		return multi_front;
	else if (sSymbol == "between")
		return multi_between;
	else if (sSymbol == "back")
		return multi_back;
	else
		return multi_none;
}

//typedef enum { hero_initiative, hero_passive, hero_none }Multi_HeroActive; //none代表不做判断
inline Multi_HeroActive str2heroActive(const std::string& sSymbol)
{
	if (sSymbol == "initiative")
		return hero_initiative;
	else if (sSymbol == "passive")
		return hero_passive;
	else
		return hero_none;
}
/*
const int ev_nuts = 0;
const int ev_sec_nuts = 1;
const int ev_large = 2;
const int ev_sec_large = 3;
const int ev_middle = 4;
const int ev_small = 5;
const int ev_catch_bulff = 6;

const int evsub_large = 0;
const int evsub_middle = 1;
const int evsub_small = 2;
*/
inline RankGroup str2rankGroup(const std::string& sSymbol)
{
	RankGroup rg;
	std::string sMain, sSub;
	auto sz = sSymbol.find(',');
	sMain = sSymbol.substr(0, sz);
	sSub = sSymbol.substr(sz+1, std::string::npos);

	if (sMain == "ev_nuts")
		rg.nMainGroup = 0;
	else if (sMain == "ev_sec_nuts")
		rg.nMainGroup = 1;
	else if (sMain == "ev_large")
		rg.nMainGroup = 2;
	else if (sMain == "ev_sec_large")
		rg.nMainGroup = 3;
	else if (sMain == "ev_middle")
		rg.nMainGroup = 4;
	else if (sMain == "ev_small")
		rg.nMainGroup = 5;
	else if (sMain == "ev_catch_bulff")
		rg.nMainGroup = 6;

	if (sSub == "evsub_large")
		rg.nSubGroup = 0;
	else if (sSub == "evsub_middle")
		rg.nSubGroup = 1;
	else if (sSub == "evsub_small")
		rg.nSubGroup = 2;

	return rg;
}

inline PublicConditionToCheck str2pcToCheck(const std::string& sSymbol) 
{
	if (sSymbol == "nNeedtoFlush")
		return pc_flush;
	else if (sSymbol == "nNeedtoStraight")
		return pc_straight;
	else if (sSymbol == "nPair")
		return pc_pair;
	else if (sSymbol == "nMaxRank")
		return pc_high;
	else
		return pc_none;
}

inline LogicCompareSymbol str2LogicComparer(const std::string& sSymbol)
{
	if (sSymbol == "&&")
		return lc_and;
	else if (sSymbol == "||")
		return lc_or;
	else
		return lc_none;
}

inline

std::string action2symbol(const Action& a) 
{
	std::string ret;
	if (a.actionType == raise) {
		ret = "R";
		ret += double2String(a.fBetSize, 1);
		ret = ret + "(" + double2String(a.fBetSizeByPot*100, 1) + "%)";
		return ret;
	}
	else if (a.actionType == fold)
		return "F";
	else if (a.actionType == check)
		return "X";
	else if (a.actionType == allin)
		return "A";
	else if (a.actionType == call)
		return "C";

	return "E"; //错误标志
}



#ifdef MAC 

inline std::string getTimeString()
{
	return "";
}

#else 

inline std::string getTimeString()
{
	struct tm newtime;
	char am_pm[] = "AM";
	__time64_t long_time;
	char timebuf[26];
	errno_t err;

	// Get time as 64-bit integer.
	_time64(&long_time);
	// Convert to local time.
	err = _localtime64_s(&newtime, &long_time);
	if (err)
		exit(1);

	if (newtime.tm_hour > 12)        // Set up extension.
		strcpy_s(am_pm, sizeof(am_pm), "PM");
	if (newtime.tm_hour > 12)        // Convert from 24-hour
		newtime.tm_hour -= 12;        // to 12-hour clock.
	if (newtime.tm_hour == 0)        // Set hour to 12 if midnight.
		newtime.tm_hour = 12;

	// Convert to an ASCII representation.
	err = asctime_s(timebuf, 26, &newtime);
	if (err)
		exit(1);

	char buff[100];
	sprintf_s(buff, sizeof(buff), "%.19s %s", timebuf, am_pm);
	return std::string(buff);
}

#endif

inline std::vector<std::string> split(const std::string& s, char c) {
	std::vector<std::string> v;
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);

	if (j == std::string::npos) {
		v.push_back(s);
	}
	else {
		while (j != std::string::npos) {
			v.push_back(s.substr(i, j - i));
			i = ++j;
			j = s.find(c, j);

			if (j == std::string::npos)
				v.push_back(s.substr(i, s.length()));
		}
	}
	return v;
}

inline float getBetByStr(const std::string &str){ 
	auto v = split(str,' ');
	if(v.size()==2){
		return stringToNum<float>(v[1]);
	} else {
		return 0;
	}
}


#ifdef MAC 
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

#ifndef _MAX_PATH
#define _MAX_PATH PATH_MAX
#endif

#ifndef _getcwd
#define _getcwd getcwd
#endif

#else
#include <direct.h>
#endif


#endif // !UTIL_H_
