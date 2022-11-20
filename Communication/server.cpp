#include "Server.h"
#include <stdio.h>
#include <iostream>

using namespace std;

struct SockInfo {
    TcpServer* s;
    TcpSocket* tcp;
    struct sockaddr_in addr;
};

void* working(void* arg) {
    struct SockInfo* pinfo = static_cast<struct SockInfo*>(arg);

    // 5. 通信
    while (1) {
        string msg = pinfo->tcp->recvMessage();
        if (!msg.empty()) {
            cout << msg << endl << endl;
        }
        else {
            break;
        }
        sleep(1);
    }
    delete pinfo->tcp;
    delete pinfo;
    return nullptr;
}

int main() {
    // 1. 创建监听的套接字
    TcpServer s;
    // 2. 绑定本地的IP port并设置监听
    s.setListen(10000);
    // 3. 阻塞并等待客户端的连接
    while (1) {
        SockInfo* info = new SockInfo;
        // 4. 建立连接
        TcpSocket* tcp = s.acceptConnection(&info->addr);
        if (tcp == nullptr) {
            cout << "--------------------重新尝试连接--------------------" << endl;
            continue;
        }

        info->s = &s;
        info->tcp = tcp;

        tcp->sendMessage("开始通信！！！！");

        pthread_t tid;
        pthread_create(&tid, NULL, working, info);
        pthread_detach(tid);
    }

    return 0;
}