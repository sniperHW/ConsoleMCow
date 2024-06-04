#pragma once

#include "globledef.h"


#ifndef CSPECIAL_PROCESSING_MACRO_CONFIG_H_
#define CSPECIAL_PROCESSING_MACRO_CONFIG_H_

class CSpecialProcessingMacroConfig
{
private:
	std::map<std::string, std::string> m_configItems; //keyΪ��ָ��

public:
	bool Init(GameType gmType); //�������ļ��м���
	std::string GetActuallyCommand(const std::string& sMacro);
};

#endif
