#pragma once
#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include "globledef.h"

ActionType str2ActionType(const std::string &str) {
	if(str == "CALL"){
		return call;
	} else if(str == "FOLD"){
		return fold;
	} else if(str == "RAISE"){
		return raise;
	} else if(str == "ALLIN"){
		return allin;
	} else if(str == "CHECK"){
		return check;
	} else if(str == "BET"){
		return bet;
	} else {
		return none;
	}
}

template <class Type>
Type stringToNum(const std::string& str)
{
    std::istringstream iss(str);
    Type num;
    iss >> num;
    return num;
}

#endif // !UTIL_H_
