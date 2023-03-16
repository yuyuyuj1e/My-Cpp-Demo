/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-02 11:07:30
 * @last_edit_time: 2023-03-16 11:55:14
 * @file_path: /Tiny-Cpp-Frame/CppLog/test/test.cpp
 * @description: 日志模块测试文件
 */


#include "CppLog.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unistd.h>

CppLog c;

void func1() {
    c.addTask("开始", 1);

    for (int i = 0; i < 10000; i++) {
        c.addTask("hello world", 1);
    }

    c.addTask("结束", 1);
}


void func2() {
    c.addTask("开始", 0);

    for (int i = 0; i < 10000; i++) {
        c.addTask("你好", 0);
    }

    c.addTask("结束", 0);
}

int main() {
    c.setTimeFormat(TimeFormat::FULLB);
    std::thread t1(func1), t2(func2);
    t1.join();
    t2.join();

    sleep(10);

    return 0;
}