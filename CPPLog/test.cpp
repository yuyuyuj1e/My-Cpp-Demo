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
#include "../ThreadPool/ThreadPool.h"
#include <iostream>
#include <string>
#include <chrono>

CPPLog c;

void func1() {
    c.writeLogWithTIme("开始");

    for (int i = 0; i < 10000; i++) {
        std::cout << i << std::endl;
        c.writeLogWithTIme("hello world");
    }

    c.writeLogWithTIme("结束");
}


void func2() {
    c.writeLogWithTIme("开始");

    for (int i = 0; i < 10000; i++) {
        std::cout << i << std::endl;
        c.writeLogWithTIme("你好");
    }

    c.writeLogWithTIme("结束");
}

int main() {
    c.openLog("123", LogOpenMode::ADDTO);
    c.setTimeFormat(TimeFormat::FULLB);
    ThreadPool pool(2);
    pool.submitTask(func1);
    pool.submitTask(func2);

    return 0;
}