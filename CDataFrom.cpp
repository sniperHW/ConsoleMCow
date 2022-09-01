//#include "pch.h"
#include "CDataFrom.h"
using namespace std;

/*
数据按GameType为根目录存放
存在flop和turn用另一深度的数据，按实际来取
*/

string CDataFrom::GetWizardFilePath(GameType gmType, const string& sNodeName, int nStackDepth, Round round)
{
	return string{};
}

string CDataFrom::GetSolverdFilePath(GameType gmType, const string& sNodeName, int nStackDepth, Round round)
{
	return string{};
}

string CDataFrom::GetRangesFilePath(GameType gmType, const string& sNodeName, int nStackDepth, Round round)
{
	return string{};
}
