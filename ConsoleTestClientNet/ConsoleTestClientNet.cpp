// ConsoleTestClientNet.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include "CNet.h"
using namespace std;


int main()
{
    int ret = CNet::InitNetSystem();
    //std::cout << ret << std::endl;

    string sGameID = "1716358164";
    string sCmd = "InitGame";
    string sArg = "GameID=1716358164;GameType=Max6_NL50_SD100;BBSize=1;Pot=1.5;Plays=[UTG]100.0,[HJ]100.0,[CO]100.0,[BTN]100.0,[SB]100.0,[BB]100.0;Hero=[BTN];Hands=<AhAd>";
    string sSendMsg = sGameID + ":" + sCmd + ":" + sArg;

    auto resp = CNet::Call("127.0.0.1", 8000, sSendMsg);

    std::string data;
    auto ok = resp->GetData(data);

    if (ok) {
        std::cout << data << std::endl;
    }
    else {
        std::cout << "call error" << std::endl;
    }
/*

    sCmd = "HeroAction";
    sArg = "[UTG]F,[HJ]F,[CO]F";
    sSendMsg = sGameID + ":" + sCmd + ":" + sArg;
    resp = CNet::Call("127.0.0.1", 8000, sSendMsg);

    ok = resp->GetData(data);
    if (ok) {
        std::cout << data << std::endl;
    }
    else {
        std::cout << "call error" << std::endl;
    }
*/

    return 0;
}

