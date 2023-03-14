/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2022-11-10 18:17:23
 * @last_edit_time: 2023-03-14 16:09:21
 * @file_path: /Tiny-Cpp-Frame/ThreadPool/include/ThreadPool.h
 * @description: 线程池模块头文件
 */

#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__

#include <mutex>
#include <queue>
#include <functional>
#include <future>
#include <thread>
#include <vector>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <atomic>
#include <condition_variable>
#include "SafeQueue.h"


/*
***************************线程池工作模式***************************
*/

// C++11 枚举类的优势: 1、降低命名空间污染； 2、 避免发生隐式转换； 3、 可以前置声明
enum class ThreadPoolWorkMode : char {
	FIXED_THREAD,
	MUTABLE_THREAD
};


/*
***************************线程池***************************
*/

class ThreadPool {
private:
	/* 线程池相关设置 */
	static int m_threads_id;  // 线程 id，静态成员变量，用于传递给工作线程使用
	bool m_close; // 关闭线程池
	std::mutex m_mutex; // 互斥锁
	std::chrono::milliseconds m_timeout;  // 任务提交超时
	size_t m_priority_level;  // 任务优先级等级
	ThreadPoolWorkMode m_mode;  // 线程池的工作模式

	/* 任务队列 */
	SafeQueue<std::function<void()>> m_task_queue; // 函数任务队列
	std::condition_variable m_conditional_safe_queue_not_full;  // 任务已满
	std::condition_variable m_conditional_safe_queue_not_empty; // 任务为空
	size_t m_max_task;  // 最大任务量

	/* 工作线程 */
	std::unordered_map<int, std::thread> m_threads;  // 线程队列
	size_t m_max_threshold;  // 线程上限
	size_t m_min_threshold;  // 线程下限
	std::atomic_int m_thread_amount;  // 线程数量


	/* 工作线程类 */
	class Worker {
	private:
		int m_id; // 工作 id
		ThreadPool *m_pool; // 所属线程池

	public:
		Worker(ThreadPool*, const int);  // 含参构造函数
		void operator()();  // 重载()，仿函数
	};


private:
void initThreadPool();  // 初始化线程池


public:
	/* 构造函数 */
	ThreadPool();  // 默认构造函数
	explicit ThreadPool(const size_t, ThreadPoolWorkMode work_mode = ThreadPoolWorkMode::FIXED_THREAD);  // 含参构造函数，且关闭隐式转换
	ThreadPool(const ThreadPool &) = delete;  // 删除拷贝构造函数
	ThreadPool(ThreadPool &&) = delete;  // 删除移动构造函数
	ThreadPool &operator=(const ThreadPool &) = delete;  // 删除赋值构造函数
	ThreadPool &operator=(ThreadPool &&) = delete;  // 删除移动赋值运算符，避免隐式移动

	/* 析构函数 */
	~ThreadPool();  // 若没有手动关闭，则通过析构函数自动关闭线程池

	/* 成员函数 */	
	void close();  // 关闭线程池

	template <typename F, typename... Args>
	auto submitTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;  // 提交异步执行的函数

	inline size_t getThreadsAmount();  // 获取线程数量
	inline void setTaskMaxAmount(size_t);  // 设置任务量最大值
	inline size_t getTaskMaxAmount();  // 获取任务量最大值
	inline void setTaskTimeoutByMilliseconds(std::chrono::milliseconds);  // 设置超时时长
	inline void setTaskTimeoutBySeconds(std::chrono::seconds);  // 设置超时时长
	inline size_t getTaskPriority();  // 获取任务优先级
	inline void setTaskPriority(size_t);  // 设置任务优先级
};



/**
 * @description: 获取线程池线程数量
 * @return {size_t} this->m_threads.size()
 */
inline size_t ThreadPool::getThreadsAmount() {
	std::unique_lock<std::mutex> lock(this->m_mutex);
	return this->m_threads.size();
}


/**
 * @description: 获取线程池任务量最大值
 * @return {size_t} this->m_max_task
 */
inline size_t ThreadPool::getTaskMaxAmount() {
	std::unique_lock<std::mutex> lock(this->m_mutex);
	return this->m_max_task;
}


/**
 * @description: 设置线程池任务量最大值
 * @param {size_t} max: 任务量最大值
 */
inline void ThreadPool::setTaskMaxAmount(size_t max) {
	std::unique_lock<std::mutex> lock(this->m_mutex);
	this->m_max_task = max;
}


/**
 * @description: 通过 chrono::milliseconds 修改超时时长
 * @param {milliseconds} new_timeout: 新的超时时长
 */
inline void ThreadPool::setTaskTimeoutByMilliseconds(std::chrono::milliseconds new_timeout) {
	std::unique_lock<std::mutex> lock(this->m_mutex);

	this->m_timeout = new_timeout;
}


/**
 * @description: 通过 chrono::seconds 修改超时时长
 * @param {seconds} new_timeout: 新的超时时长
 */
inline void ThreadPool::setTaskTimeoutBySeconds(std::chrono::seconds new_timeout) {
	std::unique_lock<std::mutex> lock(this->m_mutex);

	this->m_timeout = std::chrono::duration_cast<std::chrono::milliseconds>(new_timeout);
}


/**
 * @description: 获取当前设置的任务优先级
 * @return {size_t} this->m_priority_level
 */
inline size_t ThreadPool::getTaskPriority() {
	std::unique_lock<std::mutex> lock(this->m_mutex);

	return this->m_priority_level;
}


/**
 * @description: 修改当前设置的任务优先级
 * @param {size_t} priority: 任务优先级
 */
inline void ThreadPool::setTaskPriority(size_t priority) {
	std::unique_lock<std::mutex> lock(this->m_mutex);

	this->m_priority_level = priority;
}



/**
 * @description: 提交异步执行的函数
 * @param {Func} &: 任务函数
 * @param {Args &&...} args: 任务函数参数
 * @return {std::future<decltype(func(args...))>} 任务函数形成的 future
 */
template <typename Func, typename... Args>
auto ThreadPool::submitTask(Func &&func, Args &&... args) -> std::future<decltype(func(args...))> {

	// typename std::result_of<Func(Args...)>::type 等同于 decltype(func(args...))
	using func_renturn_type = typename std::result_of<Func(Args...)>::type;
	
	// function<int(int, int)> new_func = bind(old_func, _1, _2);
	// 将任务函数和参数绑定，打包成 func，先转换成无参函数
	std::function<func_renturn_type()> nonparam_task_func = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);

	// 将任务函数（新打包的 func）封装进共享指针中，方便复制(被 lambda 函数值捕捉)
	auto task_ptr = std::make_shared<std::packaged_task<func_renturn_type()>>(nonparam_task_func);

	// 返回通过 packaged_task 打包的 future
	auto return_future = (*task_ptr).get_future();


	{
		// 线程池加锁
		std::unique_lock<std::mutex> lock(this->m_mutex);

		// 将打包好的无参任务函数转换成 void 函数
		std::function<void()> warpper_func = [task_ptr]() {
			(*task_ptr)();  //  (*指针变量名) (函数参数列表)
		};

		// 如果线程池已经决定关闭，则不可再提交任务
		if (this->m_close) {
			std::cout << "线程池已被关闭，无法提交新任务" << std::endl;
			throw std::runtime_error("ThreadPool is already colsed");
		}

		// 如果任务数已满，等待线程执行
		if (this->m_max_task == this->m_task_queue.safeQueueSize()) {
			std::cout << "任务队列已满, 请等待任务完成" << std::endl;

			// 用户提交任务，超过时长，否则算提交任务失败
			if (std::cv_status::timeout == this->m_conditional_safe_queue_not_full.wait_for(lock, std::chrono::milliseconds(this->m_timeout))) {
				// 表示等待一秒 条件依然没有满足
				std::cerr << "提交任务超时，请稍后重尝..." << std::endl;
				return return_future;
			}
		}

		// 任务入队
		this->m_task_queue.taskEnqueue(warpper_func, this->m_priority_level);
	}

	// 动态添加线程
	if (this->m_mode == ThreadPoolWorkMode::MUTABLE_THREAD
		&& this->getThreadsAmount() < this->m_task_queue.safeQueueSize()
		&& this->getThreadsAmount() < this->m_max_threshold
		&& this->getThreadsAmount() < std::thread::hardware_concurrency()
	) {
		this->m_threads[ThreadPool::m_threads_id] = std::thread(&Worker::operator(), Worker(this, ThreadPool::m_threads_id));  // 指定线程所执行的函数
		ThreadPool::m_threads_id++;
		this->m_thread_amount++;
		std::cout << "已动态添加新线程，当前线程数量为: " << this->getThreadsAmount() << "  ----->   " << this->m_max_threshold << std::endl;
	}

	// 唤醒一个等待中的线程
	this->m_conditional_safe_queue_not_empty.notify_one();
	
	return return_future;
}

#endif  // !THREAD_POOL_H__