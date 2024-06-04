#pragma once

#include "globledef.h"


#ifndef CSPECIAL_PROCESSING_MACRO_CONFIG_H_
#define CSPECIAL_PROCESSING_MACRO_CONFIG_H_

class CSpecialProcessingMacroConfig
{
private:
	std::map<std::string, std::string> m_configItems; //key为宏指令

public:
	bool Init(GameType gmType); //从配置文件中加载
	std::string GetActuallyCommand(const std::string& sMacro);
};

#endif
