/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-02 11:07:30
 * @last_edit_time: 2022-12-04 17:06:45
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/CPPLog/test.cpp
 * @description: 日志模块测试文件
 */


#include "CPPLog.h"
#include <iostream>
#include <string>
#include <chrono>

int main() {
    CPPLog c;
    c.openLog("123", LogOpenMode::ADDTO);
    c.setTimeFormat(TimeFormat::FULLB);

    c.writeLogWithTIme("开始");

    // std::string s("你好");
    // const char *buf[] = {
    //     "北凉王府龙盘虎踞于清凉山，千门万户，极土木之盛。"
    // };
    // c.writeLogWithTIme(buf[0]);

    // c.writeLog("finish");

    for (int i = 0; i < 100000; i++) {
        std::cout << i << std::endl;
        c.writeLogWithTIme("hello world");
    }

    c.writeLogWithTIme("结束");

    return 0;
}