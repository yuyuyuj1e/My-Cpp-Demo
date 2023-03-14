/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-11-20 10:57:36
 * @last_edit_time: 2023-03-14 21:19:51
 * @file_path: /Tiny-Cpp-Frame/Communication/include/Server.h
 * @description: 封装服务器类头文件
 */

#ifndef TCP_SERVER_H__
#define TCP_SERVER_H__

#include "Socket.h"

class TcpServer {
private:
    /* 私有成员变量 */
    int m_fd;  // 监听套接字
    struct sockaddr_in m_saddr;  // sockaddr 端口(2字节) + IP地址(4字节) + 填充(8字节)
public:
    /* 构造函数与析构函数 */
    TcpServer();  // 默认构造函数
    ~TcpServer();  // 析构函数

    /* 接口 */
    int setListen(in_port_t, int max_port_size = 128);  // 设置监听, in_port_t <==> unsigned short int
    TcpSocket* acceptConnection(sockaddr_in*);  // 接受客户端连接请求
    void closeConnection();  // 关闭监听套接字
};

#endif  //  TCP_SERVER_H__