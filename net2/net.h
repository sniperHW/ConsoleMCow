#pragma once
#include <mutex>
#include <thread>
#include <winsock2.h>
#include <WinBase.h>
#include <Winerror.h>
#include <stdio.h>
#include <WS2tcpip.h>
#include <list>
#include <atomic>
#include <functional>
#include <iostream>
#include "./buffer.h"


namespace net {

	class Connection : public std::enable_shared_from_this<Connection> {
		friend class SOCKET connect(struct sockaddr_in *addr);
	public:
		typedef std::shared_ptr<Connection> Ptr;

		static Connection::Ptr Connect(const std::string &ip,uint32_t port, const std::function<void(const Connection::Ptr &conn,const Buffer::Ptr&)> onPacket, const std::function<void(const Connection::Ptr &conn)>& onReconnectOK) {
			struct sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_port = htons((u_short)port);
			int ret = inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr);
			auto sock = Connection::connect(&addr);
			if (sock == INVALID_SOCKET) {
				auto err = ::WSAGetLastError();
				return nullptr;
			}
			else {
				auto ptr = Ptr(new Connection(addr, sock));
				ptr->onReconnectOK = onReconnectOK;
				ptr->startclient(onPacket);
				return ptr;
			}
		}


		static int Listen(uint32_t port,const std::function<void(const Connection::Ptr &conn, const Buffer::Ptr&)> onPacket,const std::function<void(const Connection::Ptr &conn)> onDisconnected) {
			struct sockaddr_in servaddr;
			memset((void*)&servaddr,0,sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_addr.s_addr = inet_addr("0.0.0.0");
			servaddr.sin_port = htons(port);

			SOCKET fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

			auto thd = std::thread(listenFunc, fd,onPacket,onDisconnected);
			thd.detach();

			return 0;			
		}

		void Send(const Buffer::Ptr &buff) {
			mtx.lock();
			sendqueue.push_back(buff);
			mtx.unlock();
			std::cout << "Send" << std::endl;
		}



		void Close() {
			bool expected = false;
			if(die.compare_exchange_strong(expected,true)){
				::closesocket(socket);
				die.store(true);
			}
		}



		~Connection() {
			std::cout << "~Connection()" << std::endl;
		}

		void SetUserData(const std::any &data) {
			user_data = data;
		}

		std::any GetUserData() {
			return user_data;
		}

	private:

		static void listenFunc(SOCKET listenFd,const std::function<void(const Connection::Ptr &conn, const Buffer::Ptr&)> onPacket,const std::function<void(const Connection::Ptr &conn)> onDisconnected) {
			std::cout << "start listen" << std::endl;
			for (;;) {
				struct sockaddr sa;	
				int   addrlen = sizeof(sa);
				auto fd = ::accept(listenFd,&sa,&addrlen);
				if(fd == INVALID_SOCKET) {
					return;
				}
				std::cout << "on new client" << std::endl;

				auto ptr = Ptr(new Connection(fd));
				ptr->onDisconnected = onDisconnected;
				ptr->startserver(onPacket);
			}
		}

		Connection(SOCKET socket):socket(socket),w(0),r(0),die(false) {}

		Connection(struct sockaddr_in addr, SOCKET socket):serverAddr(addr),socket(socket),w(0),r(0),die(false) {}

		void startclient(const std::function<void(const Connection::Ptr &conn,const Buffer::Ptr&)> &onPacket);
		void startserver(const std::function<void(const Connection::Ptr &conn,const Buffer::Ptr&)> &onPacket);
		static void loopclient(const Connection::Ptr &c);
		static void loopserver(const Connection::Ptr &c);
		bool looponce(int ms);

		bool onRead();
		bool onWrite();

		static bool SetNonBlock(SOCKET sock) {
			int ioctlvar = 1;
			auto ret = ioctlsocket(sock, FIONBIO, (unsigned long*)&ioctlvar);
			if (ret != 0) {
				return false;
			}
			return true;
		}

		static SOCKET connect(struct sockaddr_in *addr) {
			SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			int ret = ::connect(sock, (struct sockaddr *)addr, sizeof(*addr));
			if (ret >= 0){
				SetNonBlock(sock);
				return sock;
			}
			else {
				::closesocket(sock);
				return INVALID_SOCKET;
			}
		}


		struct sockaddr_in serverAddr;
		SOCKET                    socket;
		std::mutex		          mtx;
		std::list<Buffer::Ptr>    sendqueue;
		std::function<void(const Connection::Ptr &conn,const Buffer::Ptr&)> onPacket;
		char  readbuff[1024*64];
		int   w;
		int   r;
		std::atomic_bool die;
		std::function<void(const Connection::Ptr &conn)> onReconnectOK;
		std::function<void(const Connection::Ptr &conn)> onDisconnected;
		std::any user_data;
	};
}