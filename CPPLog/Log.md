<!--
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-02 11:46:58
 * @last_edit_time: 2022-12-05 15:51:14
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/CPPLog/Log.md
 * @description: 头部注释配置模板
-->
1. this->m_log_fp.open(full_path, std::ofstream::app);  无法使用自定义的枚举类
2. 无法创建文件夹
3. 时间转换 通过 ctime 的方法输出自带换行符，且无法修改格式  std::string now_t = ctime(&now); std::cout << now_t << std::endl;
4. to_string 无法补0
5. 多个函数使用同一个日志类对象，如何保证写操作独立进行（不会乱），添加互斥锁
6. A 函数和 B 函数都获取了同一个锁，而 A 函数又会调用 B 函数。如果 Mutex 锁是个非递归锁，这个程序会立即死锁。
7. 写日志是磁盘I/O，速度慢，如果写日志这个操作直接是写入磁盘，开销大、理效率低。
8. 多个工作线程操作日志任务队列，另外会有一个专门的写日志线程将队列中的消息取出提供磁盘I/O写入文件中。但是该队列需要满足线程安全。