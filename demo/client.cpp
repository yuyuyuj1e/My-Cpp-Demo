/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-11-10 18:53:10
 * @last_edit_time: 2022-11-21 10:43:39
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/demo/client.cpp
 * @description: 多客户端通信测试，客户端文件
 */

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "../ThreadPool/ThreadPool.h"
#include "../Communication/Server.h"
#include "../Communication/Socket.h"


int main() {
    // 1. 创建通信套接字
    TcpSocket cs;

    // 2. 绑定服务器 IP port
    std::string server_ip = "192.168.11.132";
    unsigned short server_port = 8989; 
    cs.connectToHost(server_ip, server_port);
    
    // 3. 通信
    int number = 0;
    while (1) {
        // 发送数据
        char buff[1024];
        sprintf(buff, "你好, %d ... ", number++);
        cs.sendMessage(buff, strlen(buff) + 1);

        // 接受数据
        std::string str = cs.recvMessage();
        std::cout << "服务器回复: " << str << std::endl << std::endl;
        sleep(3);
    }
}