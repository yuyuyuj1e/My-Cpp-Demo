/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-13 09:58:13
 * @last_edit_time: 2023-03-14 14:10:16
 * @file_path: /Tiny-Cpp-Frame/ThreadPool/src/ThreadPool.cpp
 * @description: 线程池模块源文件
 */

#include "ThreadPool.h"

/*
***************************线程池的实现***************************
*/

// 初始线程 ID
int ThreadPool::m_threads_id = 1;

/**
 * @description: 默认构造函数，线程数量为可用硬件实现支持的并发线程数
 * @description: 通过委托构造函数
 */
ThreadPool::ThreadPool() : ThreadPool(std::thread::hardware_concurrency(), ThreadPoolWorkMode::FIXED_THREAD)
{ }


/**
 * @description: 含参构造函数
 * @param {size_t} n_threads: 最低线程数量
 * @param {ThreadPoolWorkMode} work_mode: 线程池工作模式
 */
ThreadPool::ThreadPool(const size_t n_threads, ThreadPoolWorkMode work_mode)
	: m_close(false)
	, m_max_task(2 * n_threads)
	, m_timeout(std::chrono::milliseconds(3000))
	, m_priority_level(1)
	, m_max_threshold(
		work_mode == ThreadPoolWorkMode::FIXED_THREAD ? 
		(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()) : 
		(2 * n_threads < std::thread::hardware_concurrency() ? 2 * n_threads : std::thread::hardware_concurrency()))
	, m_min_threshold(
		work_mode == ThreadPoolWorkMode::FIXED_THREAD ? 
		(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()) : 
		(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()))
	, m_mode(work_mode)
	, m_thread_amount(0)
{
	std::cout << "线程池初始配置如下: " << std::endl;
	if (this->m_mode == ThreadPoolWorkMode::FIXED_THREAD)
		std::cout << "线程池工作模式: FIXED_THREAD"<< std::endl;
	else
		std::cout << "线程池工作模式: MUTABLE_THREAD"<< std::endl;
	std::cout << "线程数量: " << this->m_min_threshold << '\n'
		<< "线程上限: " << this->m_max_threshold << '\n'
		<< "线程下限: " << this->m_min_threshold << '\n'
		<< "任务队列长度: " << this->m_max_task << '\n'
		<< "任务优先级: " << this->m_priority_level << '\n'
		<< "任务提交时限: 3 秒\n"
		<< std::endl;

	// 初始化线程池
	this->initThreadPool();
}


/**
 * @description: 析构函数，关闭线程池
 */
ThreadPool::~ThreadPool() {
	this->close();
}


/**
 * @description: 等待线程完成工作，然后关闭线程池
 */
void ThreadPool::close() {
	// 判断
	if (this->m_close) {
		return ;
	}

	{
        std::unique_lock<std::mutex> lock(this->m_mutex);
        this->m_close = true;
		std::cout << "线程池已准备关闭，请勿继续提交任务" << std::endl;
    }

	// 唤醒所有被当前条件变量阻塞的线程
	this->m_conditional_safe_queue_not_empty.notify_all();

	// 等待所有线程结束工作
	for(std::unordered_map<int, std::thread>::iterator it = this->m_threads.begin(); it != this->m_threads.end(); ++it) {
		it->second.join();
	}

	std::cout << "线程池已关闭" << std::endl;
}



/**
 * @description: 初始化线程池
 */
void ThreadPool::initThreadPool() {
	for (int i = 0; i < this->m_min_threshold; ++i) {
		// std::thread 调用类的成员函数需要传递类的一个对象作为参数， 由于是 operator() 下面两种写法都可以，如果是类内部，传入 this 指针即可
		// this->m_threads[i] = std::thread(Worker(this, i));  // 分配工作线程
		this->m_threads[ThreadPool::m_threads_id] = std::thread(&Worker::operator(), Worker(this, ThreadPool::m_threads_id));  // 指定线程所执行的函数
		ThreadPool::m_threads_id++;
		this->m_thread_amount++;
	}
}




/*
***************************线程池内部类的实现***************************
*/

/**
 * @description: 工作线程构造函数
 * @param {ThreadPool} *pool: 工作线程所属线程池
 * @param {int} id: 工作线程 ID
 */
ThreadPool::Worker::Worker(ThreadPool *pool, const int id) 
	: m_pool(pool)
	, m_id(id)
{ }


/**
 * @description: 重载 ()，这里是工作线程的工作函数，提交的函数会在这里执行
 */
void ThreadPool::Worker::operator()() {
	std::function<void()> func;  // 存放真正执行的函数
	bool dequeued = false;  // 是否取出任务

	while (!this->m_pool->m_close || !this->m_pool->m_task_queue.empty()) {
		{
			// 线程池加锁
			std::unique_lock<std::mutex> lock(this->m_pool->m_mutex);

			std::cout << "tid: " << std::this_thread::get_id() << " 正在尝试获取任务" << std::endl;

			// 如果任务队列为空，阻塞当前线程
			if (this->m_pool->m_task_queue.empty()) {
				std::cout << "任务队列空，等待任务..." << std::endl;
				if (this->m_pool->m_mode == ThreadPoolWorkMode::FIXED_THREAD) {
					this->m_pool->m_conditional_safe_queue_not_empty.wait(lock);  // 等待任务
				}
				else if (this->m_pool->m_mode == ThreadPoolWorkMode::MUTABLE_THREAD) {
					
					if (std::cv_status::timeout == this->m_pool->m_conditional_safe_queue_not_empty.wait_for(
						lock, std::chrono::milliseconds(this->m_pool->m_timeout))) {
						
						if (this->m_pool->m_thread_amount > this->m_pool->m_min_threshold) {
							std::cout << "tid:" << std::this_thread::get_id() << " 退出! ---- ";
							this->m_pool->m_threads[this->m_id].detach();
							this->m_pool->m_threads.erase(this->m_id);
							this->m_pool->m_thread_amount--;
							std::cout << "剩余线程: " << this->m_pool->m_thread_amount << std::endl;
							return ;
						}
						else {
							this->m_pool->m_conditional_safe_queue_not_empty.wait(lock);  // 等待任务
						}
					}
				}
			}

			// 取出任务队列中的元素
			dequeued = this->m_pool->m_task_queue.taskDequeue(func);
		}

		// 如果成功取出，执行工作函数
		if (dequeued) {
			// 取出一个任务进行通知 通知可以继续提交任务
			this->m_pool->m_conditional_safe_queue_not_full.notify_all();
			std::cout << "tid: " << std::this_thread::get_id() << " 已领取任务，当前任务数量为: " << this->m_pool->m_task_queue.safeQueueSize() 
				<< "  ----->   " << this->m_pool->m_max_task << std::endl;
			func();
		}
		else {
			std::cout << "tid: " << std::this_thread::get_id() << " 取出任务失败" << std::endl;
		}
	}
}