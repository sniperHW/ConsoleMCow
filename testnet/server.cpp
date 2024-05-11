#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <winsock2.h>
#include <map>
#include "../CNet.h"

inline std::vector<std::string> split(const std::string& s, char c) {
	std::vector<std::string> v;
	std::string::size_type i = 0;
	std::string::size_type j = s.find(c);

	if (j == std::string::npos) {
		v.push_back(s);
	}
	else {
		while (j != std::string::npos) {
			v.push_back(s.substr(i, j - i));
			i = ++j;
			j = s.find(c, j);

			if (j == std::string::npos)
				v.push_back(s.substr(i, s.length()));
		}
	}
	return v;
}


class SolutionContext{
    public:
        void *solution; //solution对象
};


std::map<std::string,SolutionContext*> SolutionContexts;


void onPakcet(SOCKET fd,const std::string &packet){
    auto req = split(packet,':');
    auto solutionID = req[0];
    auto cmd = req[1];
    std::string arg;
    if(req.size()>2) {
        arg = req[2];
    }

    auto context = SolutionContexts.find(solutionID);  
    if(cmd == "start") {
        //创建solution
    } else if(cmd == "stop"){
        //销毁solution
    } else {
        //调用solution里的方法
    }
    std::cout << solutionID << "," << cmd << "," << arg << std::endl;    
    CNet::SendPacket(fd,"world");
}


int main()
{
    int ret = CNet::InitNetSystem();
    std::cout << ret << std::endl;
    CNet::Listen(8000,onPakcet);
    ::getchar();
    return 0;
}
