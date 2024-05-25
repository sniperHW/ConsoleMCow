#include "net.h"
#include <condition_variable>
#include <mutex>
#include <map>
#include "json.hpp"

using json = nlohmann::json;


void onPacket(const net::Connection::Ptr &conn, const net::Buffer::Ptr &packet) {
    /*
      测试服务器直接把客户端发送的数据原样返回
    */
    
    
    auto rawBuf = packet->BuffPtr();
    auto cmd = ::ntohs(*(uint16_t*)rawBuf);
    auto jsonStr = std::string(&rawBuf[2], packet->Len() - 2);
    auto msg = json::parse(jsonStr);
    std::cout << cmd << "," << jsonStr << std::endl;
} 

void onReconnectOK(const net::Connection::Ptr &conn) {
    std::cout << "onReconnectOK" << std::endl;
}

int main() {

	int nResult;
	WSADATA wsaData;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != nResult)
	{
		std::cout << "Error occurred while executing WSAStartup()" << std::endl;
		return -1; //error
	}

    auto conn = net::Connection::Connect("127.0.0.1",8110,onPacket,onReconnectOK);



    //向计算进程发送任务
    //CmdDispatchJob
    json j;
    j["TaskID"] = "task111";
    j["Cfg"] = "cfgfile";
    j["Compress"] = false;



    auto jStr = j.dump();
    auto packet = net::Buffer::New(6 + jStr.length());
    packet->Append(uint32_t(2 + jStr.length()));
    packet->Append(uint16_t(2)); //CmdDispatchJob
    packet->Append(jStr);
    conn->Send(packet);
    
    getchar();
}
