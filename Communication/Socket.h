/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-11-17 19:40:14
 * @last_edit_time: 2022-11-20 11:40:58
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/Communication/Socket.h
 * @description: 封装通信套接字类，实现 string/char* 消息发送与接受（有处理 TCP"粘包"）等功能
 */

#ifndef TCP_SOCKET_H__
#define TCP_SOCKET_H__

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstring>

/*
***************************套接字类***************************
*/
class TcpSocket {
private:
    /* 私有成员变量 */
    int m_fd;  // 通信套接字
    struct sockaddr_in m_saddr;  // sockaddr 端口(2字节) + IP地址(4字节) + 填充(8字节)

private:
    /* 私有成员函数 */
    int readSpecLength(char*, int);  // 解决“粘包问题”
    int writeSpecLength(const char*, int);  // 解决“粘包问题”

public:
    /* 构造函数与析构函数 */
    TcpSocket();  // 默认构造函数，创建套接字
    TcpSocket(int);  // 接收一个用于通信的建套接字
    ~TcpSocket();  // 关闭连接

    /* 接口 */
    int sendMessage(std::string);  // 发送信息
    int sendMessage(const char*, size_t);  // 发送信息
    std::string recvMessage();  // 接收信息
    void closeTcpSocket();  // 关闭套接字
    int connectToHost(std::string, unsigned short);  // 连接服务器
};


/*
***************************套接字类的实现***************************
*/

/**
 * @description: 套接字默认构造函数
 */
TcpSocket::TcpSocket() 
    : m_fd(socket(AF_INET, SOCK_STREAM, 0)) {
    // int socket(int domain, int type, int protocol);
    this->m_saddr.sin_family = AF_INET;  // 地址族协议
}


/**
 * @description: 服务器创建的套接字
 * @param {int} socket_fd: 服务器传入的通信套接字描述符
 */
TcpSocket::TcpSocket(int socket_fd) 
    : m_fd(socket_fd) { }


/**
 * @description: 析构函数，断开连接
 */
TcpSocket::~TcpSocket() {
    this->closeTcpSocket();
}


/**
 * @description: 断开连接，服务器/客户端主动关闭连接
 */
void TcpSocket::closeTcpSocket() {
    // 如果连接没有关闭，断开连接
    if (this->m_fd > 0) {
        close(this->m_fd);
        std::cout << "--------------------通信套接字已关闭--------------------" << std::endl;
    }
}


/**
 * @description: 用于解决 TCP “粘包”问题，读取指定长度数据
 * @param {char*} message_buff: 
 * @param {int} length: 数据长度
 * @return {int}: 失败返回 -1, 成功返回读取的数据长度
 */
int TcpSocket::readSpecLength(char* message_buff, int length) {
    int remainder = length;
    int recv_already = 0;
    char* pbuff = message_buff;

    /* 读取数据 */
    while (remainder > 0) {
        // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
        int recv_already = recv(this->m_fd, pbuff, remainder, 0);
        if (recv_already > 0) {
            pbuff += recv_already;
            remainder -= recv_already;
        }
        else {
            return recv_already;
        }
    }

    return length;
}


/**
 * @description: 用于解决 TCP “粘包”问题，写入指定长度数据
 * @param {char*} message_buff: 写数据的地址 
 * @param {int} length: 数据长度
 * @return {int}: 失败返回 -1，断开连接返回 0，成功返回发送数据长度
 */
int TcpSocket::writeSpecLength(const char* message_buff, int length) {
    int remainder = length;
    int send_already = 0;
    const char* pbuff = message_buff;

    /* 将数据写入到缓冲区 */
    while (remainder > 0) {  // 循环直到数据全部写入到指定地址
        // ssize_t send(int fd, const void *buf, size_t len, int flags);
        int send_already = send(m_fd, message_buff, remainder, 0);
        if (send_already == -1) {
            return send_already;
        }
        else if (send_already == 0) {
            continue;
        }
        pbuff += send_already;
            remainder -= send_already;
    }
    return length;
}


/**
 * @description: 连接服务器
 * @param {string} ip: 连接的 IP 地址
 * @param {unsigned short} port: 连接 IP 的端口
 * @return {int}: 失败返回 -1, 成功返回 0
 */
int TcpSocket::connectToHost(std::string ip, unsigned short port) {
    /* 处理数据 */
    this->m_saddr.sin_port = htons(port);  // 将 IP 和端口信息转换为大端存储
    inet_pton(AF_INET, ip.data(), &this->m_saddr.sin_addr.s_addr);

    /* 建立连接 */
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int connect_ret = connect(this->m_fd, (struct sockaddr*)&this->m_saddr, sizeof(struct sockaddr));
    if (connect_ret == -1) {
        std::cerr << "connect failed" << std::endl;
        return connect_ret;
    }
    std::cout << "--------------------连接建立成功--------------------" << std::endl;
    std::cout << "服务器 IP: " << ip << " —— 端口: " << port << std::endl << std::endl;

    return connect_ret;
}

/**
 * @description: 发送信息
 * @param {string} message: 服务器/客户端发送的数据
 * @return {int}: 失败返回 -1，断开连接返回 0，成功返回发送数据长度
 */
int TcpSocket::sendMessage(std::string message) {
    /* 处理/拷贝数据 */
    char* data = new char[message.length() + sizeof(int)];  // 内存空间: 包头(存储数据长度) + 原始数据
    int data_len = htonl(message.length());
    memcpy(data, &data_len, sizeof(int));
    memcpy(data + sizeof(int), message.data(), message.length());

    /* 发送数据 */
    std::cout << "--------------------正在发送数据(数据块大小: " << message.length() << ")--------------------" << std::endl;
    int send_ret = this->writeSpecLength(data, message.length() + sizeof(int));
    if (send_ret == -1) {
        std::cout << "--------------------对方断开连接--------------------" << std::endl;
        std::cerr << "send failed" << std::endl;
        delete[] data;
        return send_ret;
    }

    /* 清理 */
    delete[] data;
    return send_ret;
}



/**
 * @description: 函数注释配置模板
 * @param {char*} message_buff: 指向服务器/客户端发送数据首地址的指针
 * @param {size_t} length: 数据长度
 * @return {*}: 失败返回 -1，断开连接返回 0，成功返回发送数据长度
 */
int TcpSocket::sendMessage(const char* message_buff, size_t length) {
    /* 申请内存 */
    char* pbuff = (char*)malloc(length + sizeof(int));
    
    /* 处理/拷贝数据 */
    int netlen = htonl(length); // 将数据包长度转换为网络字节序
    memcpy(pbuff, &netlen, sizeof(int));
    memcpy(pbuff + sizeof(int), message_buff, length);
    
    /* 发送数据 */
    std::cout << "--------------------正在发送数据(数据块大小: " << length << ")--------------------" << std::endl;
    int send_ret = this->writeSpecLength(pbuff, length + sizeof(int));
    if (send_ret == -1) {
        std::cout << "--------------------对方断开连接--------------------" << std::endl;
        std::cerr << "send failed" << std::endl;
        delete[] pbuff;
        return send_ret;
    }

    /* 清理 */
    delete[] pbuff;
    return send_ret;
}


/**
 * @description: 接收数据
 * @return {string}: 接收到的数据
 */
std::string TcpSocket::recvMessage() {
    /* 读数据包头，获取数据块大小 */
    int length = 0;
    int recv_ret = readSpecLength((char*)&length, sizeof(int));
    if (recv_ret == -1) {
        std::cerr << "recv message failed" << std::endl;
        return std::string();
    }
    else if (recv_ret == 0) {
        std::cout << "--------------------对方断开连接--------------------" << std::endl;
        return std::string();
    }

    length = ntohl(length);  // 大端转换为小端
    std::cout << "--------------------正在接受数据(数据块大小: " << length << ")--------------------"  << std::endl;

    /* 分配内存 */
    char* buf = new char[length + 1];

    /* 接受数据 */
    recv_ret = readSpecLength(buf, length);
    if (recv_ret != length) {
        delete[]buf;
        return std::string();
    }
    buf[length] = '\0';
    std::string str(buf);

    /* 清理 */
    delete[]buf;
    return str;
}

#endif  // !TCP_SOCKET_H__