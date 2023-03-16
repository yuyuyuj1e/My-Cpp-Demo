/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-15 09:22:13
 * @last_edit_time: 2023-03-16 12:11:21
 * @file_path: /Tiny-Cpp-Frame/CppLog/src/CppLog.cpp
 * @description: 日志模块源文件
 */

#include "CppLog.h"
#include <string>
#include <chrono>
#include <sys/stat.h>

/**
 * @description: 日志模块对象初始化函数
 * @param {size_t} max_size: 日志文件的大小，超过该大小则切换另一个文件，默认为 50M
 * @param {string} log_path: 存放日志文件的目录，默认为 ./Log
 * @param {TimeFormat} ft: 日志记录所采用的时间格式，默认为 YYYY-MM-DD HH:MM:SS
 * @param {bool} backup: 是否进行日志备份
 */
CppLog::CppLog(const size_t max_log_size, const std::string log_path, const LogMode mode, const TimeFormat tf, bool backup) 
    : m_max_size(max_log_size)
    , m_path(log_path)
    , m_mode(mode)
    , m_time_format(tf)
    , m_backup(backup) 
{ 
    m_start = true;
    m_thread = new std::thread(&CppLog::working, this);  // 构造线程
}


/**
 * @description: CppLog 对象析构函数，会调用成员函数 close()
 */
CppLog::~CppLog() {
    this->close();
}


/**
 * @description: 关闭日志文件
 */
void CppLog::close() {
    if (this->m_fp) {
        this->m_fp.close();
        this->m_name = std::string("log.txt");
        this->m_mode = LogMode::ADDTO;
        m_time_format = TimeFormat::FULLA;
    }
}


/**
 * @description: 打开日志文件
 * @param {string} name: 日志文件名称
 * @return {bool}: 日志文件打开成功返回 true， 失败返回 false
 */
bool CppLog::open(std::string name) {
    /* 关闭日志文件 */
    this->close();

    /* 打开日志文件 */
    this->m_name = name;
    std::string full_path = this->m_path + "/" + name;

    if (this->m_mode == LogMode::ADDTO) {
        this->m_fp.open(full_path, std::ofstream::app);
    }
    else if (this->m_mode == LogMode::WRITEONLY) {
        this->m_fp.open(full_path, std::ofstream::out);
    }

    if (!this->m_fp) {  // 打开失败
        return false;
    }
    return true;
}


/**
 * @description: 备份日志文件
 * @return {bool}: 需要进行备份时返回 true，否则返回 false
 */
bool CppLog::backup() {
    /* 判断是否需要进行备份 */
    if (this->m_backup == false) {
        return false;
    }

    /* 获取文件大小 */
	struct stat stat_buf;  // 存储文件(夹)信息的结构体，有文件大小和创建时间、访问时间、修改时间等
    std::string full_path = this->m_path + "/" + this->m_name; 
	stat(full_path.c_str(), &stat_buf);  // 提供文件名字符串，获得文件属性结构体
	size_t file_size = stat_buf.st_size;  // 获取文件大小

    /* 备份文件 */
    if (file_size >= this->m_max_size * 1024 * 1024) {
        /* 配置时间 */
        TimeFormat ft = this->m_time_format;
        this->setTimeFormat(TimeFormat::FULLA);
        std::string now_t = this->getCurrentTime();
        this->setTimeFormat(ft);

        /* 重命名 */
        this->m_fp.close();
        std::string new_name = full_path + " " + now_t;
        rename(full_path.c_str(), new_name.c_str());
    }
    else return false;

    return this->open(this->m_name);
}


/**
 * @description: 获取指定时间格式的当前时间字符串
 * @return {std::string}: 指定时间格式的字符串
 */
std::string CppLog::getCurrentTime() {
    /* 获取当前时间戳 */
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
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

    return std::string(ctime);
}


/**
 * @description: 带时间写入日志
 * @param {string} str: 写入日志的内容
 */
void CppLog::writeWithTime(const std::string str) {
    this->open(this->m_name);
    this->backup();
    std::string now_t = this->getCurrentTime();
    while (now_t.length() != 20) now_t += " ";
    this->m_fp << now_t << " --->  " << str << '\n';
}

/**
 * @description: 不带时间写入日志
 * @param {string} str: 写入日志的内容
 */
void CppLog::write(const std::string str) {
    this->open(this->m_name);
    this->backup();
    this->m_fp << str << '\n';
}


/**
 * @description: 日志线程工作函数
 */
void CppLog::working() {
    while (m_start || !m_taskQ.empty()) {
        while (!m_taskQ.empty()) {
            m_mutex.lock();  // 加锁，保护共享资源
            std::pair<std::string, int> pair = m_taskQ.front();
            m_taskQ.pop();
            m_mutex.unlock();  // 解锁

            if (pair.second > 0) {  // 如果标志大于 0，调用带时间的
                writeWithTime(pair.first);
            }
            else {
                write(pair.first);
            }
        }
    }
}


/**
 * @description: 外部调用，向任务队列添加任务
 * @param {string} str: 需要记录的日志内容，默认值为 1
 * @param {int} flag: 是否记录时间，当数值给定数值大于 0 时记录时间，否则不记录时间，默认记录时间
 */
void CppLog::addTask(std::string str, int flag) {
    m_mutex.lock();
    m_taskQ.push(std::make_pair(str, flag));  // 将任务加入工作队列中
    m_mutex.unlock();
}

