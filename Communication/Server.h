#ifndef SERVER_H__
#define SERVER_H__

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "Socket.h"

class TcpServer {
private:
    /* 私有成员变量 */
    int m_fd;  // 监听套接字
    struct sockaddr_in m_saddr;  // sockaddr 端口(2字节) + IP地址(4字节) + 填充(8字节)
public:
    TcpServer();  // 默认构造函数
    ~TcpServer();  // 析构函数

    /* 接口 */
    int setListen(in_port_t, int max_port_size = 128);  // 设置监听, in_port_t <==> unsigned short int
};

// 默认构造函数
TcpServer::TcpServer()
    : m_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    
    this->m_saddr.sin_family = AF_INET;  // 地址族协议
    this->m_saddr.sin_addr.s_addr = INADDR_ANY;  // 0 = 0.0.0.0; 0 大端小端没有区别，因此不需要转换， 绑定为 0 后，会读取本地网卡实际 IP
}

// 析构函数
TcpServer::~TcpServer() {
    close(this->m_fd);
}

// 设置监听, in_port_t <==> unsigned short int
int TcpServer::setListen(in_port_t port, int max_port_size) {
    /* 配置 sockaddr */
    this->m_saddr.sin_port = htons(port);  // 设置端口

    /* 监听套接字绑定端口 */
    int bind_ret = bind(this->m_fd, (struct sockaddr*)(&this->m_saddr), sizeof(this->m_saddr));
    if (bind_ret == -1) {  // 绑定失败
        std::cerr << "bind failed" << std::endl;
        return bind_ret;
    }
    std::cout << "监听套接字绑定端口成功"
        << ", IP: " << inet_ntoa(this->m_saddr.sin_addr)  // 将网络地址转换成点分十进制
        << ", Port: " << port << std::endl;

    /* 设置监听 */
    int listen_ret = listen(this->m_fd, max_port_size);
    if (listen_ret == -1) {  // 绑定失败
        std::cerr << "listen failed" << std::endl;
        return listen_ret;
    }
    std::cout << "开始监听客户端连接" << std::endl;

    return listen_ret;
}


#endif  //  SERVER_H__