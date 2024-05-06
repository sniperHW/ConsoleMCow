#pragma once

#ifndef CSOLUTIONS_H_
#define CSOLUTIONS_H_

#include "globledef.h"
#include "CSolution.h"


class CSolutions
{
private:
	std::map<std::string, std::shared_ptr<CSolution>> m_Solutions;	//Key:��ϷID�������룬̨���룬ʱ���룩

public:
	std::shared_ptr<CSolution> NewSolution(const std::string& sGameID);	//����ָ��InitGameʱ��Ҫ��new
	std::shared_ptr<CSolution> GetSolutionByGameID(const std::string& sGameID);
	void FinishSolution(const std::string& sGameID);	//����ָ��FinishGame��һСʱ�󻹴��ڵ�


};

#endif