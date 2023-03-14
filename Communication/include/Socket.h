/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-11-17 19:40:14
 * @last_edit_time: 2023-03-14 21:29:39
 * @file_path: /Tiny-Cpp-Frame/Communication/include/Socket.h
 * @description: 套接字类头文件
 */

#ifndef TCP_SOCKET_H__
#define TCP_SOCKET_H__


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
    TcpSocket(int, struct sockaddr_in);  // 接收一个用于通信的建套接字
    ~TcpSocket();  // 关闭连接

    /* 接口 */
    int sendMessage(std::string);  // 发送信息
    int sendMessage(const char*, size_t);  // 发送信息
    std::string recvMessage();  // 接收信息
    void closeTcpSocket();  // 关闭套接字
    int connectToHost(std::string, unsigned short);  // 连接服务器(服务于客户端)
    struct sockaddr_in getSockaddr();  // 获取通信对方的信息
};




#endif  // !TCP_SOCKET_H__