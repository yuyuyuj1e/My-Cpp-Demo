<!--
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-12-02 11:46:58
 * @last_edit_time: 2022-12-05 10:57:23
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/CPPLog/Log.md
 * @description: 头部注释配置模板
-->
1. this->m_log_fp.open(full_path, std::ofstream::app);  无法使用自定义的枚举类
2. 无法创建文件夹
3. 时间转换 通过 ctime 的方法输出自带换行符，且无法修改格式  std::string now_t = ctime(&now); std::cout << now_t << std::endl;
4. to_string 无法补0
5. 单例模式如何保证线程安全
6. A 函数和 B 函数都获取了同一个锁，而 A 函数又会调用 B 函数。如果 Mutex 锁是个非递归锁，这个程序会立即死锁。