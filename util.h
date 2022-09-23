#pragma once
#ifndef UTIL_H_
#define UTIL_H_

#include <string>
#include <iostream>
#include <cstring>
#include <sstream>
#include "globledef.h"

inline ActionType str2ActionType(const std::string &str) {
	if(str == "CALL" || str == "call") {
		return call;
	} else if(str == "FOLD" || str == "fold"){
		return fold;
	} else if(str == "CHECK" ||str == "check"){
		return check;
	} else if(str == "BET" || str == "RAISE" || str == "bet" || str == "raise"){
		return raise;
	} else if (str == "ALLIN" || str == "allin") {
		return allin;
	}	else {
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


inline std::vector<std::string> split(const std::string& s, char seperator)
{
   std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}


inline float getBetByStr(const std::string &str){ 
	auto v = split(str,' ');
	if(v.size()==2){
		return stringToNum<float>(v[1]);
	} else {
		return 0;
	}
}


#endif // !UTIL_H_
