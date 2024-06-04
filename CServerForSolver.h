#pragma once

#ifndef CSERVERFORSOLVER_H_
#define CSERVERFORSOLVER_H_

//处理来自clientSolverManager的连接

#include <iostream>
#include <net2/net.h>

class CServerForSolver {
    public:
        static void start(port int); 
    
    private:
        static void onPacket(const net::Connection::Ptr &conn, const net::Buffer::Ptr &packet);
        static void onDisconnected(const net::Connection::Ptr &conn);
};

#endif