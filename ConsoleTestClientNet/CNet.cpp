#include <winsock2.h>
#include <WinBase.h>
#include <Winerror.h>
#include <iostream>
#include <WS2tcpip.h>
#include "CNet.h"

void Response::emit(std::string data) {
	std::lock_guard<std::mutex> guard(this->mtx);
	if (!this->ready) {
		this->ready = true;
		this->data = data;
		this->cv.notify_all();
	}
}

void Response::error() {
	std::lock_guard<std::mutex> guard(this->mtx);
	if (!this->ready) {
		this->ready = true;
		this->isError = true;
		this->cv.notify_all();
	}
}

bool Response::GetData(std::string &data) {
	for (;;) {
		std::lock_guard<std::mutex> guard(this->mtx);
		if (this->ready) {
			data = this->data;
			break;
		} else {
			this->cv.wait(this->mtx);
		}
	}
	return isError==false;
}

void ThreadPool::threadFunc(ThreadPool::TaskQueue* queue_) {
	for (;;) {
		Task::Ptr task = queue_->Get();
		if (task == nullptr) {
			return;
		}
		task->Do();
	}
}


void ThreadPool::threadFuncSwap(ThreadPool::TaskQueue* queue_) {
	ThreadPool::TaskQueue::taskque local_tasks;
	for (;;) {
		if (!queue_->Get(local_tasks)) {
			return;
		}
		while (!local_tasks.empty()) {
			const Task::Ptr& task = local_tasks.front();
			task->Do();
			local_tasks.pop_front();
		}
	}
}


void ThreadPool::TaskQueue::Close() {
	std::lock_guard<std::mutex> guard(this->mtx);
	if (!this->closed) {
		this->closed = true;
		this->cv.notify_all();
	}
}


bool ThreadPool::TaskQueue::Get(ThreadPool::TaskQueue::taskque& out) {
	std::lock_guard<std::mutex> guard(this->mtx);
	for (; ;) {
		if (this->closed) {
			if (this->tasks.empty()) {
				return false;
			}
			else {
				this->tasks.swap(out);
				return true;
			}
		}
		else {
			if (this->tasks.empty()) {
				++this->watting;
				this->cv.wait(this->mtx);
				--this->watting;
			}
			else {
				this->tasks.swap(out);
				return true;
			}
		}
	}
}


Task::Ptr ThreadPool::TaskQueue::Get() {
	std::lock_guard<std::mutex> guard(this->mtx);
	for (; ;) {
		if (this->closed) {
			if (this->tasks.empty()) {
				return nullptr;
			}
			else {
				Task::Ptr& task = this->tasks.front();
				this->tasks.pop_front();
				return task;
			}
		}
		else {

			if (this->tasks.empty()) {
				++this->watting;
				this->cv.wait(this->mtx);
				--this->watting;	
			}
			else {
				Task::Ptr task = this->tasks.front();
				this->tasks.pop_front();
				return task;
			}
		}
	}
}


void ThreadPool::TaskQueue::PostTask(const Task::Ptr& task) {
	std::lock_guard<std::mutex> guard(this->mtx);
	if (this->closed) {
		return;
	}
	this->tasks.push_back(task);
	if (this->watting > 0) {
		this->cv.notify_one();
	}
}

bool ThreadPool::Init(int threadCount, int swapMode) {

	bool expected = false;
	if (!this->inited.compare_exchange_strong(expected, true)) {
		return false;
	}

	if (threadCount <= 0) {
		threadCount = 1;
	}

	for (auto i = 0; i < threadCount; ++i) {
		if (swapMode == 0) {
			threads_.push_back(std::thread(threadFunc, &queue_));
		}
		else {
			threads_.push_back(std::thread(threadFuncSwap, &queue_));
		}
	}

	return true;
}

ThreadPool::~ThreadPool() {
	queue_.Close();
	size_t i = 0;
	for (; i < threads_.size(); ++i) {
		threads_[i].join();
	}
}

# define _WINSOCK_DEPRECATED_NO_WARNINGS

static SOCKET connect(const std::string &host, int port) {
	//std::cout << "connect " << host << ":" << port << std::endl;
	struct sockaddr_in remote;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("\nError occurred while opening socket: %d.", WSAGetLastError());
		return INVALID_SOCKET;
	}
	remote.sin_family = AF_INET;
	remote.sin_port = ::htons((u_short)port);
	inet_pton(AF_INET, host.c_str(), &remote.sin_addr);
	int ret = connect(sock, (struct sockaddr*)&remote, sizeof(remote));
	if (ret >= 0)
	{
		return sock;
	}
	else {
		std::cout << "connect failed " << ret << "," << WSAGetLastError() << std::endl;
		closesocket(sock);
		return INVALID_SOCKET;
	}
}

bool CNet::SendPacket(SOCKET sock, const std::string& packet) {
	char* buff = (char*)::malloc(4 + packet.length());
	*(unsigned int*)buff = ::htonl(packet.length());
	::memcpy(&buff[4], packet.c_str(), packet.length());
	int offset = 0;
	int total = packet.length() + 4;
	for (; offset < total;){
		int n = ::send(sock, &buff[offset],total-offset, 0);
		if (n <= 0) {
			free(buff);
			return false;
		}
		else {
			offset += n;
		}
	}
	return true;
}


bool CNet::RecvPacket(SOCKET sock,std::string &packet) {
	char len[4];
	int offset = 0;
	for (;offset < sizeof(len);) {
		int n = ::recv(sock, &len[offset], sizeof(len)-offset, 0);
		if (n <= 0) {
			return false;
		}
		else {
			offset += n;
		}
	}

	int payload = ntohl(*(unsigned int*)len);

	char* buff = (char*)::malloc(payload);
	offset = 0;
	for (; offset < payload;) {
		int n = ::recv(sock, &buff[offset], payload - offset, 0);
		if (n <= 0) {
			free(buff);
			return false;
		}
		else {
			offset += n;
		}
	}

	packet = std::string(buff, payload);
	return true;
}


void RequestTask::Do() {
	//for (;;) {
	SOCKET sock;
	for (;;) {
		sock = connect(host, port);
		if (sock != INVALID_SOCKET) {
			break;
		}
		else {
			::Sleep(100);
		}
	}


	if (!CNet::SendPacket(sock, request)) {
		closesocket(sock);
		this->resp->error();
		return;
	}

	auto thread = std::thread([](SOCKET sock){
		::Sleep(10000);
		closesocket(sock);
	},sock);
	thread.detach();

	std::string resp;
	if (!CNet::RecvPacket(sock, resp)) {
		closesocket(sock);
		this->resp->error();
		return;
	}
	else {
		this->resp->emit(resp);
		closesocket(sock);
		return;
	}
	//}
}

ThreadPool CNet::threadpool;

int CNet::InitNetSystem()
{
	int nResult;
	WSADATA wsaData;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != nResult)
	{
		std::cout << "Error occurred while executing WSAStartup()" << std::endl;
		return -1; //error
	}

	threadpool.Init(16);

	return 0;
}

std::shared_ptr<Response> CNet::Call(const std::string& host, int port, const std::string& request) {
	std::shared_ptr<Response> resp = std::shared_ptr<Response>(new Response);
	threadpool.PostTask(RequestTask::New(host,port,request,resp));
	return resp;
}

void serveFunc(SOCKET fd,std::function<void(SOCKET,const std::string&)> onPacket) {
	for (;;) {
		//recv packet
		std::string packet;
		if (!CNet::RecvPacket(fd, packet)) {
			closesocket(fd);
			return;
		}
		onPacket(fd,packet);
	}
}


void listenFunc(SOCKET listenFd,std::function<void(SOCKET,const std::string&)> onPacket) {
	std::cout << "start listen" << std::endl;
	for (;;) {
		struct sockaddr sa;	
		int   addrlen = sizeof(sa);
		auto fd = ::accept(listenFd,&sa,&addrlen);
		if(fd == INVALID_SOCKET) {
			return;
		}
		//std::cout << "on new client" << std::endl;

    	auto thread = std::thread(serveFunc, fd,onPacket);
		thread.detach();
	}
}

int CNet::Listen(int port,std::function<void(SOCKET,const std::string&)> onPacket) {
	struct sockaddr_in servaddr;
	memset((void*)&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	//inet_pton(AF_INET, "0.0.0.0", &servaddr.sin_addr.s_addr);
	inet_pton(AF_INET, "0.0.0.0", &servaddr.sin_addr);


	servaddr.sin_port = htons(port);

	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET)
	{
		printf("\nError occurred while opening socket: %d.", WSAGetLastError());
		return -1;
	}

	int yes = 1;
	::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

	if(::bind(fd,(const sockaddr *)&servaddr,sizeof(servaddr)) < 0)
		return -1;

	if(::listen(fd,256) < 0)
		return -1;

    auto thread = std::thread(listenFunc, fd,onPacket);
	thread.detach();

	return 0;
}
