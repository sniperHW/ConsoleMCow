#pragma once

#ifndef CSOLUTIONS_H_
#define CSOLUTIONS_H_

#include "globledef.h"
#include "CSolution.h"


class CSolutions
{
private:
	std::map<std::string, std::shared_ptr<CSolution>> m_Solutions;	//Key:游戏ID（机器码，台子码，时间码）

public:
	std::shared_ptr<CSolution> NewSolution(const std::string& sGameID);	//网络指令InitGame时，要先new
	std::shared_ptr<CSolution> GetSolutionByGameID(const std::string& sGameID);
	void FinishSolution(const std::string& sGameID);	//网络指令FinishGame或一小时后还存在的


};

#endif