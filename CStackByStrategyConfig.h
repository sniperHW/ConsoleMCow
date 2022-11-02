#pragma once
#ifndef CSTACK_BY_STRATEGY_CONFIG_H_
#define CSTACK_BY_STRATEGY_CONFIG_H_

#include "globledef.h"


//��solverԤ����Ӧ�����ڽ�solver���sizeתΪ����
class CStackByStrategyConfig
{
private:
	std::map<std::string, std::shared_ptr<Stacks>> m_configItems; //keyΪpreflop��flop��������

public:
	bool Init(GameType gmType); 
	const std::shared_ptr<Stacks> GetItemByName(const std::string& sNodeName) const; //ֻ����preflop��flop,sNodeNameתΪwithoutBoardƥ��
};

#endif