/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-14 19:28:00
 * @last_edit_time: 2023-03-14 21:19:48
 * @file_path: /Tiny-Cpp-Frame/Communication/src/Server.cpp
 * @description: 服务器类源文件
 */

#include "Server.h"
#include <iostream>
#include <unistd.h>


/**
 * @description: 默认构造函数，创建一个用于 TCP 的监听套接字，但是没有设置监听端口，需要在后续设置
 */
TcpServer::TcpServer() : m_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    // int socket(int domain, int type, int protocol);
    this->m_saddr.sin_family = AF_INET;  // 地址族协议
    this->m_saddr.sin_addr.s_addr = INADDR_ANY;  // 0 = 0.0.0.0; 0 大端小端没有区别，因此不需要转换， 绑定为 0 后，会读取本地网卡实际 IP
}


/**
 * @description: 析构函数，关闭监听套接字
 */
TcpServer::~TcpServer() {
    this->closeConnection();
}


/**
 * @description: 关闭监听套接字
 */
void TcpServer::closeConnection() {
    // 如果连接没有关闭，断开连接
    if (this->m_fd > 0) {
        close(this->m_fd);
        std::cout << "--------------------监听套接字已关闭--------------------" << std::endl;
    }
}


/**
 * @description: 设置监听,
 * @param {in_port_t} port: 监听端口 in_port_t <==> unsigned short int
 * @param {int} max_port_size: 同时能处理的最大连接数，可省略，省略后默认为 128
 * @return {int}: 成功返回 0，失败返回 -1
 */
int TcpServer::setListen(in_port_t port, int max_port_size) {
    /* 配置 sockaddr */
    this->m_saddr.sin_port = htons(port);  // 设置端口

    /* 监听套接字绑定端口 */
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int bind_ret = bind(this->m_fd, (struct sockaddr*)(&this->m_saddr), sizeof(struct sockaddr));
    if (bind_ret == -1) {  // 绑定失败
        std::cerr << "bind failed" << std::endl;
        return bind_ret;
    }
    std::cout << "--------------------监听套接字绑定端口成功--------------------" << std::endl;
    std::cout << "IP: " << inet_ntoa(this->m_saddr.sin_addr)  // 将网络地址转换成点分十进制
        << ", 端口: " << port << std::endl << std::endl;

    /* 设置监听 */
    // int listen(int sockfd, int backlog);
    int listen_ret = listen(this->m_fd, max_port_size);
    if (listen_ret == -1) {  // 绑定失败
        std::cerr << "listen failed" << std::endl;
        return listen_ret;
    }
    std::cout << "--------------------开始监听客户端连接请求--------------------" << std::endl;

    return listen_ret;
}


/**
 * @description: 接受连接请求
 * @param {sockaddr_in*} addr: 客户端的 IP 和 端口信息
 * @return {TcpSocket*}: 返回一个指向用于通信的套接字类对象的指针
 */
TcpSocket* TcpServer::acceptConnection(sockaddr_in* addr) {
    /* 特殊处理 */
    if (addr == NULL) {
        return nullptr;
    }

    /* 接受连接请求 */
    socklen_t addrlen = sizeof(struct sockaddr_in);  // socklen_t <==> unsigned int
    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int cfd = accept(this->m_fd, (struct sockaddr*)addr, &addrlen);
    if (cfd == -1) {
        std::cerr << "accept failed" << std::endl;
        return nullptr;
    }
    std::cout << "客户端 IP: " << inet_ntoa(addr->sin_addr)
        << " —— 端口: " << ntohs(addr->sin_port) << "   请求建立连接。。。" << std::endl << std::endl;
    std::cout << "--------------------与客户端连接--------------------" << std::endl << std::endl;

    return new TcpSocket(cfd, *addr);
}