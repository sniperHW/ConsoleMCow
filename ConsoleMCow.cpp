// ConsoleMCow.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "globledef.h"
#include "CStrategyNodeConfig.h"
#include "CRangeNodeConfig.h"
#include "CStrategyTreeConfig.h"
#include "CTurnDataFromWizardConfig.h"

using namespace std;

//全局变量
map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;	//范围节点配置
map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
map<GameType, RegexTB> g_preflopRegexTBs; //preflop节点匹配表
map<GameType, RegexTB> g_toflopRegexTBs; //toflop节点匹配表
map<GameType, CTurnDataFromWizardConfig> g_turnDataFromWizardConfigs; //trun策略数据为wizard匹配表

int main()
{


}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

