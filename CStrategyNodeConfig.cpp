//#include "pch.h"
#include "CStrategyNodeConfig.h"

using namespace std;

bool CStrategyNodeConfig::Init(GameType gmType)
{
	return true;
}

const shared_ptr<CStrategyNodeConfigItem> CStrategyNodeConfig::GetItemByName(const string& sNodeName) const
{
	return nullptr;
}
