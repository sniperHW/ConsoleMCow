#pragma once
#ifndef CSTACK_BY_STRATEGY_CONFIG_H_
#define CSTACK_BY_STRATEGY_CONFIG_H_

#include "globledef.h"
#include <memory>
#include <map>

//��solverԤ����Ӧ�����ڽ�solver���sizeתΪ����
//����ֻ����preflop�������ƶ�Ӧ�ĳ���ֵ������ȡ���ݵĲ�����Ϊpreflop��flop�������ƣ������preflop��ֱ��ȡֵ����flop����preflop��ʼֵ��flop������������
//�������ݴӷ�Χ�ļ��м���
class CStackByStrategyConfig
{
private:
	std::map<std::string, std::shared_ptr<Stacks>> m_configItems; //keyΪpreflop��flop��������

public:
	bool Init(GameType gmType); 
	const std::shared_ptr<Stacks> GetItemByName(const std::string& sNodeName) const; //ֻ����preflop��flop
};

#endif