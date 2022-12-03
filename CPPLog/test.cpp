/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-02 11:07:30
 * @last_edit_time: 2022-12-03 12:01:28
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/CPPLog/test.cpp
 * @description: 日志模块测试文件
 */


#include "CPPLog.h"
#include <iostream>
#include <string>
#include <chrono>

int main() {
    CPPLog c;
    std::string s("你好");
    c.openLog("123");
    c.writeLogWithTIme(s);

    const char *buf[] = {
        "北凉王府龙盘虎踞于清凉山，千门万户，极土木之盛。"
    };
    c.writeLogWithTIme(buf[0]);

    c.writeLog("finish");

    return 0;
}