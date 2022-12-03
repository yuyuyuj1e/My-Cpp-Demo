/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-01 21:13:14
 * @last_edit_time: 2022-12-03 11:59:42
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/CPPLog/CPPLog.h
 * @description: 日志类
 */


#ifndef CPPLOG_H_
#define CPPLOG_H_

#include <iostream>
#include <string>
#include <mutex>
#include <fstream>
#include <chrono>

/*
***************************日志文件写入模式***************************
*/

enum class LogOpenMode {
    ADDTO = std::ofstream::app,
    ENDOFFILE = std::ofstream::ate,
    BINARY = std::ofstream::binary,
    READONLY = std::ofstream::in,
    WRITEONLY = std::ofstream::out,
    NONE = -1,
};


/*
***************************时间格式***************************
*/

enum class TimeFormat {
    FULLA = 1L << 0,  // YYYY-MM-DD HH:MM:SS
    FULLB = 1L << 1,  // YYYY/MM/DD HH:MM:SS
    YMDA = 1L << 2,  // YYYY-MM-DD
    YMDB = 1L << 3,  // YYYY/MM/DD
    TIMEONLY = 1L << 4,  // HH:MM:SS
};


/*
***************************日志文件***************************
*/
class CPPLog {
private:
    /* 私有成员变量 */
    std::fstream m_log_fp;  // 日志文件
    std::string m_log_path;  // 日志文件路径
    std::string m_log_name;  // 日志文件名称
    size_t m_max_log_size;  // 日志文件大小
    std::mutex m_log_mutex;  // 日志文件互斥锁
    LogOpenMode m_log_open_mode;  // 日志文件打开方式


public:
    /* 构造函数与析构函数 */
    CPPLog(const size_t max_log_size = 50, const std::string log_path = "./Log");
    ~CPPLog();

    /* 接口 */
    void writeLog(const std::string);  // 不带时间
    void writeLog(const char*);  // 不带时间
    void writeLogWithTIme(const std::string);  // 带时间
    void writeLogWithTIme(const char*);  // 带时间
    std::string getCurrentTime(TimeFormat tf = TimeFormat::FULLA);  // 获取当前时间
    bool openLog(std::string name, const LogOpenMode mode = LogOpenMode::ADDTO);  // 打开日志文件
    void closeLog();  // 关闭日志文件
};



CPPLog::CPPLog(const size_t max_log_size, const std::string log_path) 
    : m_max_log_size(max_log_size)
    , m_log_path(log_path)
    , m_log_fp(0)
    , m_log_name("")
    , m_log_open_mode(LogOpenMode::NONE)
{ }


/**
 * @description: CPPLog 对象析构函数，会调用成员函数 closeLog()
 */
CPPLog::~CPPLog() {
    this->closeLog();
}


// 打开日志文件
bool CPPLog::openLog(std::string name, const LogOpenMode mode) {
    /* 关闭日志文件 */
    this->closeLog();

    /* 打开日志文件 */
    this->m_log_name = name;
    this->m_log_open_mode = mode;
    std::string full_path = this->m_log_path + "/" + name;
    this->m_log_fp.open(full_path, std::ofstream::app);
    // 如果打开失败
    if (!this->m_log_fp) {
        return false;
    }

    return true;
}


// 关闭日志文件
void CPPLog::closeLog() {
    if (this->m_log_fp) {
        this->m_log_fp.close();
        this->m_log_name = "";
        this->m_log_open_mode = LogOpenMode::NONE;
    }
}


// 获取当前时间
/**
 * @description: 获取指定时间格式的当前时间字符串，默认返回格式为 YYYY-MM-DD HH:MM:SS
 * @param {TimeFormat} tf: 时间格式
 * @return {std::string}: 指定时间格式的字符串
 */
std::string CPPLog::getCurrentTime(TimeFormat tf) {
    /* 获取当前时间戳 */
    std::chrono::time_point now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);

    /* 将当前时间戳转换为 string */
    struct tm now_st = *localtime (&now_t);
    now_st.tm_year = now_st.tm_year + 1900;
    ++now_st.tm_mon;
    char ctime[20];

   
    if (tf == TimeFormat::FULLB) {
        snprintf(ctime, 20, "%04d/%02d/%02d %02d:%02d:%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }
    else if (tf == TimeFormat::YMDA) {
        snprintf(ctime, 11, "%04d-%02d-%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
        );
    }
    else if (tf == TimeFormat::YMDB) {
        snprintf(ctime, 11, "%04d/%02d/%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
        );
    }
    else if (tf == TimeFormat::TIMEONLY) {
        snprintf(ctime, 9, "%02d:%02d:%02d"
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }
    else {
        snprintf(ctime, 20, "%04d-%02d-%02d %02d:%02d:%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }

    std::string time(ctime);
    return time;
}


void CPPLog::writeLogWithTIme(const std::string str) {
    if (!this->m_log_fp) {
        return ;
    }

    std::string now_t = this->getCurrentTime(TimeFormat::FULLA);
    while (now_t.length() != 20) now_t += " ";
    now_t += " --->  ";
    
    this->m_log_fp << now_t;
    this->writeLog(str);

}


void CPPLog::writeLogWithTIme(const char* str) {
    if (!this->m_log_fp) {
        return ;
    }

    std::string now_t = this->getCurrentTime(TimeFormat::FULLA);
    while (now_t.length() != 20) now_t += " ";
    now_t += " --->  ";
    this->m_log_fp << now_t;
    this->writeLog(str);
}


void CPPLog::writeLog(const std::string str) {
    if (!this->m_log_fp) {
        return ;
    }

    this->m_log_fp << str << std::endl;
}

void CPPLog::writeLog(const char* str) {
    if (!this->m_log_fp) {
        return ;
    }

    this->m_log_fp << str << std::endl;
}

#endif  // !CPPLOG_H_