#ifndef TCP_CLIENT_H__
#define TCP_CLIENT_H__

#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include "Socket.h"

class TcpClient {
private:
    /* 私有成员变量 */

public:
    TcpClient();  // 默认构造函数，创建套接字
    ~TcpClient();  // 关闭连接

    /* 接口 */

};

TcpClient::TcpClient() 
{ }

TcpClient::~TcpClient() 
{ }


#endif  // !TCP_CLIENT_H__