#include "CServerForSolver.h"

#include "json/json.h"

void CServerForSolver::start(port int) {
    net::Connection::Listen(port,onPacket,onDisconnected);
}

void onHeartBeat(const net::Connection::Ptr &conn,const Json::Value& packet) {
    g_solver.onNodeHeartBeat(conn,packet);        
}

void onCommitTask(const net::Connection::Ptr &conn,const Json::Value& packet) {
    g_solver.onCommitTask(conn,packet);        
}


void CServerForSolver::onPacket(const net::Connection::Ptr &conn, const net::Buffer::Ptr &packet) {
    auto rawBuf = packet->BuffPtr();
    auto cmd = ::ntohs(*(uint16_t*)rawBuf);
   
    auto jsonStr = std::string(&rawBuf[2], packet->Len() - 2);
    
    std::stringstream ss;
    ss << jsonStr;
    JSONCPP_STRING errs;
    Json::CharReaderBuilder builder;
    Json::Value value;
    if (!parseFromStream(builder, ss, &value, &errs)) {
        return;
    }
    switch(cmd) {
        case 1:
            onHeartBeat(conn,value);
            break;
        case 3:
            onCommitTask(conn,value);
            break;
    }

}

void CServerForSolver::onDisconnected(const net::Connection::Ptr &conn) {

}