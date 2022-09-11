//#include "pch.h"
#include "CStrategyNodeConfigItem.h"

using namespace std;

CStrategyNodeConfigItem::CStrategyNodeConfigItem(string sReplaceNodeName, string sSpecialProcessing, Round round,OOPX oopx) :m_sReplaceNodeName(sReplaceNodeName), m_sSpecialProcessing(sSpecialProcessing), m_round(round) ,m_oopx(oopx) {}
