#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include "../CNet.h"

int main()
{
    int ret = CNet::InitNetSystem();
    std::cout << ret << std::endl;

    auto resp = CNet::Call("127.0.0.1",8000,"111111:start");

    std::string data;
    auto ok = resp->GetData(data);

    if(ok) {
        std::cout << data << std::endl;
    } else {
        std::cout << "call error" << std::endl;
    }

    return 0;
}
