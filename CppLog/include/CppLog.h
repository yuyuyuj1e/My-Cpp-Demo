/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-13 09:45:56
 * @last_edit_time: 2023-03-16 12:18:02
 * @file_path: /Tiny-Cpp-Frame/CppLog/include/CppLog.h
 * @description: 日志模块头文件
 */


#ifndef CppLog_H_
#define CppLog_H_

#include <mutex>
#include <fstream>
#include <queue>
#include <thread>

/*
***************************日志文件写入模式***************************
*/
enum class LogMode {
    ADDTO = 1L << 0,  // std::ofstream::app
    WRITEONLY = 1L << 2, // std::ofstream::out
    NONE = 1L << 3
};


/*
***************************时间格式***************************
*/
enum class TimeFormat {
    FULLA = 1L << 0,  // YYYY-MM-DD HH:MM:SS
    FULLB = 1L << 1,  // YYYY/MM/DD HH:MM:SS
    YMDA = 1L << 2,  // YYYY-MM-DD
    YMDB = 1L << 3,  // YYYY/MM/DD
    TIMEONLY = 1L << 4  // HH:MM:SS
};


/*
***************************日志文件***************************
*/
class CppLog {
private:
    /* 私有成员变量 */
    std::fstream m_fp = std::fstream();  // 日志文件
    std::string m_path;  // 日志文件路径
    std::string m_name = std::string("log.txt");  // 日志文件名称
    size_t m_max_size;  // 日志文件大小
    LogMode m_mode;  // 日志文件打开方式
    TimeFormat m_time_format;  // 时间格式
    bool m_backup;  // 备份日志文件
    bool m_start = false;  // 判断日志类是否已经启动
    
    std::mutex m_mutex;  // 日志文件互斥锁
    std::queue<std::pair<std::string, int>> m_taskQ;  // 任务队列
    std::thread* m_thread;  // 日志类线程

private:
    bool backup();  // 备份日志文件
    bool open(std::string);  // 打开日志文件
    void close();  // 关闭日志文件

    std::string getCurrentTime();  // 获取当前时间
    void write(const std::string);  // 不带时间
    void writeWithTime(const std::string);  // 带时间

    void working();  // 线程工作函数

public:
    /* 构造函数与析构函数 */
    CppLog(const size_t max_size = 2, const std::string log_path = "../Log", const LogMode mode = LogMode::ADDTO, const TimeFormat tf = TimeFormat::FULLA, bool backup = true);
    ~CppLog();

    /* 接口 */
    inline void setOpenMode(LogMode);  // 设置文件打开模式
    inline void setTimeFormat(TimeFormat);  // 设置时间格式
    void addTask(std::string, int flag = 1);  // 向任务队列添加任务
};


/** 
 * @description: 设置文件打开模式
 * @param {LogMode} mode: 文件打开模式
 */
inline void CppLog::setOpenMode(LogMode mode) {
    this->m_mode = mode;
}


/**
 * @description: 设置时间格式
 * @param {TimeFormat} ft: 时间格式
 */
inline void CppLog::setTimeFormat(TimeFormat ft) {
    this->m_time_format = ft;
}

#endif  // !CppLog_H_