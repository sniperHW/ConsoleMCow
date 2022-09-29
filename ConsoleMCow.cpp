// ConsoleMCow.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "globledef.h"
#include "CStrategyNodeConfig.h"
#include "CRangeNodeConfig.h"
#include "CStrategyTreeConfig.h"
#include "CFlopDataFromWizardConfig.h"
#include "CStrategy.h"
#include <fstream>
#include "CActionMatchConfig.h"
#include "util.h"
#include "CStackByStrategyConfig.h"
#include "CTurnPresaveSolverConfig.h"

using namespace std;

//全局变量
map<GameType, CStrategyNodeConfig> g_strategyNodeConfigs; //策略节点配置
map<GameType, CRangeNodeConfig> g_rangeNodeConfigs;	//范围节点配置
map<GameType, CStrategyTreeConfig> g_strategyTreeConfigs; //策略树配置
map<GameType, CActionMatchConfig> g_actionMatchConfigs; //preflop节点匹配表
map<GameType, CFlopDataFromWizardConfig> g_flopDataFromWizardConfigs; //flop策略数据为wizard匹配表
map<GameType, CTurnPresaveSolverConfig> g_turnPresaveSolverConfigs; //flop策略数据为wizard匹配表
map<GameType, CStackByStrategyConfig> g_stackByStrategyConfig; //预存solver策略对应的筹码

int main()
{


	/*{
		CStrategy strategy;
		StackByStrategy stackTest;
		strategy.Load(Max6_NL50_SD150, "sTest", stackTest, SuitReplace{}, "test");
	}*/

	{
		Json::Value root;
  		std::ifstream ifs;
  		ifs.open("./test/output_result_dump1.json");

  		Json::CharReaderBuilder builder;
  		JSONCPP_STRING errs;
  		if (!parseFromStream(builder, ifs, &root, &errs)) {
    		std::cout << errs << std::endl;
    		return -1;
  		} else {
			  			
			{
				CStrategy strategy;
				Stacks stackTest,stackByStrategy;
				strategy.Load(Max6_NL50_SD150,root,"BTN_vsUTG_srp<KsQsTh>O",stackTest, stackByStrategy, SuitReplace{});
				for(auto it = strategy.m_strategy.begin();it != strategy.m_strategy.end();it++){
					cout << (*it)->m_action.actionType << "," << (*it)->m_action.fBetSize << endl;
				}
			}

			{
				CStrategy strategy;
				Stacks stackTest, stackByStrategy;
				strategy.Load(Max6_NL50_SD150,root,"BTN_vsUTG_srp<KsQsTh>X-R15",stackTest, stackByStrategy, SuitReplace{});
				for(auto it = strategy.m_strategy.begin();it != strategy.m_strategy.end();it++){
					cout << (*it)->m_action.actionType << "," << (*it)->m_action.fBetSize << endl;
				}
			}


			{
				CStrategy strategy;
				Stacks stackTest, stackByStrategy;
				strategy.Load(Max6_NL50_SD150,root,"BTN_vsUTG_srp<KsQsTh>X-R15-A",stackTest, stackByStrategy,  SuitReplace{});
				for(auto it = strategy.m_strategy.begin();it != strategy.m_strategy.end();it++){
					cout << (*it)->m_action.actionType << "," << (*it)->m_action.fBetSize << endl;
				}
			}
  		}
	}

	{
		CStrategy strategy;
		strategy.LoadMacro("./test/macro.txt");
		for(auto it = strategy.m_macro.begin();it != strategy.m_macro.end();it++){
			cout << it->first << "," << it->second << endl;
		}		
	}

	cout << "hello";
	cout << "by xu" << endl;

}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

