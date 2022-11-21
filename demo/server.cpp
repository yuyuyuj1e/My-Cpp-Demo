/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-11-21 10:22:23
 * @last_edit_time: 2022-11-21 11:10:58
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/demo/server.cpp
 * @description: 多客户端通信测试，服务器文件
 */

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "../ThreadPool/ThreadPool.h"
#include "../Communication/Server.h"
#include "../Communication/Socket.h"


struct SockInfo {
    TcpServer* s;
    TcpSocket* tcp;
    struct sockaddr_in addr;
};


void working(TcpServer* ss, TcpSocket* tcp, struct sockaddr_in* saddr) {
    int number = 0;
    while (1) {
        // 发送数据
        char buff[1024];
        sprintf(buff, "hello, %d ... ", number++);
        tcp->sendMessage(buff, strlen(buff) + 1);

        // 接受数据
        std::string str = tcp->recvMessage();
        tcp->getSockaddr();
        std::cout << "客户端回复: " << str << std::endl << std::endl;
        sleep(3);
    }
}


int main() {
    // 0. 创建线程池
    ThreadPool pool(4);

    // 1. 创建监听套接字
    TcpServer ss;
    struct sockaddr_in saddr;

    // 2. 绑定并监听端口
    unsigned short server_port = 8989;
    ss.setListen(server_port, 4);

    // 3. 阻塞并等待客户端的连接
    while (1) {
        // 4. 建立连接
        TcpSocket* tcp = ss.acceptConnection(&saddr);
        if (tcp == nullptr) {
            std::cout << "--------------------重新尝试连接--------------------" << std::endl;
            continue;
        }

        // 5. 通信
        tcp->sendMessage("开始通信！！！！");
        pool.submitTask(working, &ss, tcp, &saddr);
    }
}