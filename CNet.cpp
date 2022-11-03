#include "CNet.h"
#include <winsock2.h>
#include <WinBase.h>
#include <Winerror.h>
#include <iostream>
#include <WS2tcpip.h>


void Response::emitData(std::string data) {
	std::lock_guard<std::mutex> guard(this->mtx);
	if (!this->ready) {
		this->ready = true;
		this->data = data;
		this->cv.notify_all();
	}
}

std::string Response::GetData() {
	for (;;) {
		std::lock_guard<std::mutex> guard(this->mtx);
		if (this->ready) {
			break;
		} else {
			this->cv.wait(this->mtx);
		}
	}
	return this->data;
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
				++this->watting;//增加等待数量
				this->cv.wait(this->mtx);
				--this->watting;//减少正在等待的线程数量
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
				++this->watting;//增加等待数量
				this->cv.wait(this->mtx);
				--this->watting;//减少正在等待的线程数量	
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
	std::cout << "connect " << host << ":" << port << std::endl;
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

static bool sendRequest(SOCKET sock, const std::string& request) {
	char* buff = (char*)::malloc(4 + request.length());
	*(unsigned int*)buff = ::htonl(request.length());
	::memcpy(&buff[4], request.c_str(), request.length());
	int offset = 0;
	int total = request.length() + 4;
	for (; offset < total;){
		//发送请求
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


static bool recvPacket(SOCKET sock,std::string &resp) {
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

	resp = std::string(buff, payload);
	return true;
}


void RequestTask::Do() {

	
	for (;;) {
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

		if (!sendRequest(sock, request)) {
			closesocket(sock);
			continue;
		}

		std::string resp;
		if (!recvPacket(sock, resp)) {
			closesocket(sock);
		}
		else {
			this->resp->emitData(resp);
			return;
		}
	}
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

std::shared_ptr<Response> CNet::Request(const std::string& host, int port, const std::string& request) {
	std::shared_ptr<Response> resp = std::shared_ptr<Response>(new Response);
	threadpool.PostTask(RequestTask::New(host,port,request,resp));
	return resp;
}
