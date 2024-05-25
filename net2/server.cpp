#include "net.h"


void onPacket(const net::Connection::Ptr &conn, const net::Buffer::Ptr &packet) {
    auto resp = net::Buffer::New(4 + packet->Len());
    resp->Append(uint32_t(packet->Len()));
    resp->Append(packet);
    conn->Send(resp);
} 

void onDisconnected(const net::Connection::Ptr &conn) {
    std::cout << "onDisconnected" << std::endl; 
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

    net::Connection::Listen(8110,onPacket,onDisconnected);
    getchar();
}


