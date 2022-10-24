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

inline std::vector<std::string> split(const std::string& s, char c){
    std::vector<std::string> v; 
    std::string::size_type i = 0;
    std::string::size_type j = s.find(c);

    while (j != std::string::npos) {
        v.push_back(s.substr(i, j-i));
        i = ++j;
        j = s.find(c, j);

        if (j == std::string::npos)
            v.push_back(s.substr(i, s.length()));
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
