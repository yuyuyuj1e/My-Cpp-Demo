/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-01 21:13:14
 * @last_edit_time: 2022-12-04 17:07:22
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/CPPLog/CPPLog.h
 * @description: 日志模块
 */


#ifndef CPPLOG_H_
#define CPPLOG_H_

#include <iostream>
#include <string>
#include <mutex>
#include <fstream>
#include <chrono>
#include <sys/stat.h>

/*
***************************日志文件写入模式***************************
*/
enum class LogOpenMode {
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
class CPPLog {
private:
    /* 私有成员变量 */
    std::fstream m_log_fp;  // 日志文件
    std::string m_log_path;  // 日志文件路径
    std::string m_log_name;  // 日志文件名称
    size_t m_max_log_size;  // 日志文件大小
    std::recursive_mutex m_log_mutex;  // 日志文件互斥递归锁
    LogOpenMode m_log_open_mode;  // 日志文件打开方式
    TimeFormat m_time_format;  // 时间格式
    bool m_backup;  // 备份日志文件

public:
    /* 构造函数与析构函数 */
    CPPLog(const size_t max_log_size = 2, const std::string log_path = "./Log", const TimeFormat tf = TimeFormat::FULLA, bool backup = true);
    ~CPPLog();

    /* 接口 */
    void writeLog(const std::string);  // 不带时间
    void writeLog(const char*);  // 不带时间
    void writeLogWithTIme(const std::string);  // 带时间
    void writeLogWithTIme(const char*);  // 带时间
    std::string getCurrentTime();  // 获取当前时间
    void setTimeFormat(TimeFormat);  // 设置时间格式
    bool backupLog();  // 备份日志文件
    bool openLog(std::string name, const LogOpenMode mode = LogOpenMode::ADDTO);  // 打开日志文件
    void closeLog();  // 关闭日志文件
};


/**
 * @description: 日志模块对象初始化函数
 * @param {size_t} max_log_size: 日志文件的大小，超过该大小则切换另一个文件，默认为 50M
 * @param {string} log_path: 存放日志文件的目录，默认为 ./Log
 * @param {TimeFormat} ft: 日志记录所采用的时间格式，默认为 YYYY-MM-DD HH:MM:SS
 * @param {bool} backup: 是否进行日志备份
 */
CPPLog::CPPLog(const size_t max_log_size, const std::string log_path, const TimeFormat ft, bool backup) 
    : m_max_log_size(max_log_size)
    , m_log_path(log_path)
    , m_log_fp(0)
    , m_log_name("")
    , m_log_open_mode(LogOpenMode::NONE)
    , m_time_format(ft)
    , m_backup(backup)
{ }


/**
 * @description: CPPLog 对象析构函数，会调用成员函数 closeLog()
 */
CPPLog::~CPPLog() {
    this->closeLog();
}


/**
 * @description: 打开日志文件
 * @param {string} name: 日志文件名称
 * @param {LogOpenMode} mode: 日志文件打开方式
 * @return {bool}: 日志文件打开成功返回 true， 失败返回 false
 */
bool CPPLog::openLog(std::string name, const LogOpenMode mode) {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);

    /* 关闭日志文件 */
    this->closeLog();

    /* 打开日志文件 */
    this->m_log_name = name;
    this->m_log_open_mode = mode;
    std::string full_path = this->m_log_path + "/" + name;
    if (this->m_log_open_mode == LogOpenMode::ADDTO) {
        this->m_log_fp.open(full_path, std::ofstream::app);
    }
    else if (this->m_log_open_mode == LogOpenMode::WRITEONLY) {
        this->m_log_fp.open(full_path, std::ofstream::out);
    }

    // 如果打开失败
    if (!this->m_log_fp) {
        return false;
    }
    return true;
}


/**
 * @description: 关闭日志文件
 */
void CPPLog::closeLog() {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);
    
    if (this->m_log_fp) {
        this->m_log_fp.close();
        this->m_log_name = "";
        this->m_log_open_mode = LogOpenMode::NONE;
    }
}



/**
 * @description: 设置时间格式
 * @param {TimeFormat} ft: 时间格式
 */
void CPPLog::setTimeFormat(TimeFormat ft) {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);

    this->m_time_format = ft;
}


/**
 * @description: 获取指定时间格式的当前时间字符串
 * @return {std::string}: 指定时间格式的字符串
 */
std::string CPPLog::getCurrentTime() {
    /* 获取当前时间戳 */
    std::chrono::time_point now = std::chrono::system_clock::now();
    std::time_t now_t = std::chrono::system_clock::to_time_t(now);

    /* 将当前时间戳转换为 string */
    struct tm now_st = *localtime (&now_t);
    now_st.tm_year = now_st.tm_year + 1900;
    ++now_st.tm_mon;
    char ctime[20];

   
    if (this->m_time_format == TimeFormat::FULLB) {
        snprintf(ctime, 20, "%04d/%02d/%02d %02d:%02d:%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
            , now_st.tm_hour, now_st.tm_min, now_st.tm_sec
        );
    }
    else if (this->m_time_format == TimeFormat::YMDA) {
        snprintf(ctime, 11, "%04d-%02d-%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
        );
    }
    else if (this->m_time_format == TimeFormat::YMDB) {
        snprintf(ctime, 11, "%04d/%02d/%02d"
            , now_st.tm_year, now_st.tm_mon, now_st.tm_mday
        );
    }
    else if (this->m_time_format == TimeFormat::TIMEONLY) {
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


/**
 * @description: 带时间写入日志
 * @param {string} str: 写入日志的内容
 * @return {*}
 */
void CPPLog::writeLogWithTIme(const std::string str) {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);

    if (!this->m_log_fp) {
        return ;
    }

    this->backupLog();

    std::string now_t = this->getCurrentTime();
    while (now_t.length() != 20) now_t += " ";
    now_t += " --->  ";
    
    this->m_log_fp << now_t;
    this->m_log_fp << str << std::endl;
}


/**
 * @description: 带时间写入日志
 * @param {char*} str: 写入日志的内容
 */
void CPPLog::writeLogWithTIme(const char* str) {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);

    if (!this->m_log_fp) {
        return ;
    }

    this->backupLog();

    std::string now_t = this->getCurrentTime();
    while (now_t.length() != 20) now_t += " ";
    now_t += " --->  ";
    this->m_log_fp << now_t;
    this->m_log_fp << str << std::endl;
}


/**
 * @description: 不带时间写入日志
 * @param {string} str: 写入日志的内容
 */
void CPPLog::writeLog(const std::string str) {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);

    if (!this->m_log_fp) {
        return ;
    }

    this->backupLog();

    this->m_log_fp << str << std::endl;
}


/**
 * @description: 不带时间写入日志
 * @param {char*} str: 写入日志的内容
 */
void CPPLog::writeLog(const char* str) {
    std::unique_lock<std::recursive_mutex> lock(this->m_log_mutex);

    if (!this->m_log_fp) {
        return ;
    }

    this->backupLog();

    this->m_log_fp << str << std::endl;
}


/**
 * @description: 备份日志文件
 * @return {bool}: 需要进行备份时返回 true，否则返回 false
 */
bool CPPLog::backupLog() {
    /* 判断是否需要进行备份 */
    if (this->m_backup == false) {
        return false;
    }

    /* 获取文件大小 */
	struct stat statbuf;  // 存储文件(夹)信息的结构体，有文件大小和创建时间、访问时间、修改时间等
    std::string full_path = this->m_log_path + "/" + this->m_log_name; 
	stat(full_path.c_str(), &statbuf);  // 提供文件名字符串，获得文件属性结构体
	size_t filesize = statbuf.st_size;  // 获取文件大小
    std::cout << filesize << " " << this->m_max_log_size * 1024 * 1024 << std::endl;

    /* 备份文件 */
    if (filesize >= this->m_max_log_size * 1024 * 1024) {
        /* 配置时间 */
        TimeFormat ft = this->m_time_format;
        this->setTimeFormat(TimeFormat::FULLA);
        std::string now_t = this->getCurrentTime();
        this->setTimeFormat(ft);

        /* 重命名 */
        this->m_log_fp.close();
        std::string new_log_name = full_path + " " + now_t;
        rename(full_path.c_str(), new_log_name.c_str());
    }
    else return false;

    return this->openLog(this->m_log_name, this->m_log_open_mode);;
}

#endif  // !CPPLOG_H_