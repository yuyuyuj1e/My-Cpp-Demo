/**
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @date: 2022-11-10 18:17:23
 * @last_edit_time: 2022-11-16 11:46:03
 * @file_path: /Multi-Client-Communication-System-Based-on-Thread-Pool/ThreadPool/ThreadPool.h
 * @description: 带有任务优先级的多模式线程池
 */

#ifndef THREAD_POOL_H__
#define THREAD_POOL_H__ 1

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


/*
***************************线程池工作模式***************************
*/

// C++11 枚举类的优势: 1、降低命名空间污染； 2、 避免发生隐式转换； 3、 可以前置声明
enum class ThreadPoolWorkMode {
	FIXED_THREAD,
	MUTABLE_THREAD
};


/*
***************************任务队列***************************
*/

template<typename T>
class SafeQueue {
private:
	
	struct cmp {  // 优先级队列，比较函数
		bool operator()(std::pair<T, size_t>& a, std::pair<T, size_t>& b){
			return a.second < b.second;
		}
	};
	
	std::priority_queue<std::pair<T, size_t>, std::vector<std::pair<T, size_t>>, cmp> m_safe_queue;  // 任务队列

	std::mutex m_safe_queue_mutex;  // 任务队列互斥锁

public:
	/* 构造函数 */
	SafeQueue() { }


	/* 析构函数 */
	~SafeQueue() { }


	/* 成员函数 */
	bool isSafeQueueEmpty();  // 队列是否为空

	size_t safeQueueSize();  // 任务队列大小

	void taskEnqueue(T &, size_t);  // 添加任务

	bool taskDequeue(T &);  // 取出任务

};


/*
***************************任务队列实现***************************
*/

/**
 * @description: 判断任务队列是否为空
 * @return {bool} this->m_safe_queue.empty()
 */
template<typename T>
bool SafeQueue<T>::isSafeQueueEmpty() {
	std::unique_lock<std::mutex> lock(this->m_safe_queue_mutex);  // 任务队列上锁

	return this->m_safe_queue.empty();
}



/**
 * @description: 获取任务队列大小
 * @return {size_t} this->m_safe_queue.size()
 */
template<typename T>
size_t SafeQueue<T>::safeQueueSize() {
	std::unique_lock<std::mutex> lock(this->m_safe_queue_mutex);  // 任务队列上锁

	return this->m_safe_queue.size();
}


/**
 * @description: 向任务队列添加任务
 * @param {T} t: 任务函数
 * @param {size_t} priority: 任务优先级
 */
template<typename T>
void SafeQueue<T>::taskEnqueue(T &t, size_t priority) {
	std::unique_lock<std::mutex> lock(this->m_safe_queue_mutex);

	std::pair<T, int> priority_task(t, priority);

	this->m_safe_queue.emplace(priority_task);

	std::cout << "任务已提交，当前任务数量为: " << this->m_safe_queue.size() << std::endl;
}


/**
 * @description: 是否可以从任务队列取出任务，如可以取出任务
 * @param {T} t: 获取任务函数的空函数
 * @return {bool} true/false
 */
template<typename T>
bool SafeQueue<T>::taskDequeue(T &t) {
	std::unique_lock<std::mutex> lock(this->m_safe_queue_mutex); // 任务队列上锁

	if (this->m_safe_queue.empty())
		return false;

	t = std::move(this->m_safe_queue.top().first); // 取出队首元素，返回队首元素值，并进行右值引用
	this->m_safe_queue.pop(); // 弹出入队的第一个元素

	return true;
}

/*
***************************线程池***************************
*/

class ThreadPool {
private:
	/* 线程池相关设置 */
	bool m_is_close_thread_pool; // 关闭线程池

	std::mutex m_thread_pool_mutex; // 互斥锁

	std::chrono::milliseconds m_task_timeout;  // 任务提交超时

	size_t m_priority_level;  // 任务优先级等级

	static int m_threads_id;  // 线程 id

	ThreadPoolWorkMode m_thread_pool_work_mode;  // 线程池的工作模式

	/* 任务队列 */
	SafeQueue<std::function<void()>> m_task_queue; // 函数任务队列

	std::condition_variable m_conditional_safe_queue_not_full;  // 任务已满

	std::condition_variable m_conditional_safe_queue_not_empty; // 任务为空

	size_t m_max_task_amount;  // 最大任务量

	/* 工作线程 */
	// std::vector<std::thread> m_threads;  // 线程队列
	std::unordered_map<int, std::thread> m_threads;  // 线程队列

	size_t m_max_thread_threshold;  // 线程上限

	size_t m_min_thread_threshold;  // 线程下限

	std::atomic_int m_thread_amount;  // 线程数量


	/* 工作线程类 */
	class Worker {
	private:
		int m_id; // 工作 id
		ThreadPool *m_pool; // 所属线程池

	public:
		/* 构造函数 */
		Worker(ThreadPool*, const int);  // 含参构造函数

		/* 成员函数 */
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
	void closeThreadPool();  // 关闭线程池

	template <typename F, typename... Args>
	auto submitTask(F &&f, Args &&...args) -> std::future<decltype(f(args...))>;  // 提交异步执行的函数

	size_t getThreadsAmount();  // 获取线程数量

	void setTaskMaxAmount(size_t);  // 设置任务量最大值

	size_t getTaskMaxAmount();  // 获取任务量最大值

	void setTaskTimeoutByMilliseconds(std::chrono::milliseconds);  // 设置超时时长

	void setTaskTimeoutBySeconds(std::chrono::seconds);  // 设置超时时长

	size_t getTaskPriority();  // 获取任务优先级

	void setTaskPriority(size_t);  // 设置任务优先级

	void showThreadPoolWorkMode();  // 展示线程池工作模式
};


/*
***************************线程池的实现***************************
*/

// 初始线程 ID
int ThreadPool::m_threads_id = 1;

/**
 * @description: 默认构造函数，线程数量为可用硬件实现支持的并发线程数
 */
ThreadPool::ThreadPool()
	: m_is_close_thread_pool(false) 
	, m_max_task_amount(2 * std::thread::hardware_concurrency())
	, m_task_timeout(std::chrono::milliseconds(3000))
	, m_priority_level(1)
	, m_max_thread_threshold(std::thread::hardware_concurrency())
	, m_min_thread_threshold(std::thread::hardware_concurrency())
	, m_thread_pool_work_mode(ThreadPoolWorkMode::FIXED_THREAD)
	, m_thread_amount(0)
{
	std::cout << "线程池初始配置如下: " << '\n'
		<< "线程池工作模式: FIXED_THREAD"  << '\n'
		<< "线程数量: " << this->m_min_thread_threshold << '\n'
		<< "线程上限: " << this->m_max_thread_threshold << '\n'
		<< "线程下限: " << this->m_min_thread_threshold << '\n'
		<< "任务队列长度: " << this->m_max_task_amount << '\n'
		<< "任务优先级: " << this->m_priority_level << '\n'
		<< "任务提交时限: 3 秒" << '\n'
		<< std::endl;

	// 初始化线程池
	this->initThreadPool();
}


/**
 * @description: 含参构造函数
 * @param {size_t} n_threads: 最低线程数量
 * @param {ThreadPoolWorkMode} work_mode: 线程池工作模式
 */
ThreadPool::ThreadPool(const size_t n_threads, ThreadPoolWorkMode work_mode)
	: m_is_close_thread_pool(false)
	, m_max_task_amount(2 * n_threads)
	, m_task_timeout(std::chrono::milliseconds(3000))
	, m_priority_level(1)
	, m_max_thread_threshold(
		work_mode == ThreadPoolWorkMode::FIXED_THREAD ? 
		(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()) : 
		(2 * n_threads < std::thread::hardware_concurrency() ? 2 * n_threads : std::thread::hardware_concurrency()))
	, m_min_thread_threshold(
		work_mode == ThreadPoolWorkMode::FIXED_THREAD ? 
		(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()) : 
		(n_threads < std::thread::hardware_concurrency() ? n_threads : std::thread::hardware_concurrency()))
	, m_thread_pool_work_mode(work_mode)
	, m_thread_amount(0)
{
	std::cout << "线程池初始配置如下: " << std::endl;
	if (this->m_thread_pool_work_mode == ThreadPoolWorkMode::FIXED_THREAD)
		std::cout << "线程池工作模式: FIXED_THREAD"<< std::endl;
	else
		std::cout << "线程池工作模式: MUTABLE_THREAD"<< std::endl;
	std::cout << "线程数量: " << this->m_min_thread_threshold << '\n'
		<< "线程上限: " << this->m_max_thread_threshold << '\n'
		<< "线程下限: " << this->m_min_thread_threshold << '\n'
		<< "任务队列长度: " << this->m_max_task_amount << '\n'
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
	this->closeThreadPool();
}


/**
 * @description: 等待线程完成工作，然后关闭线程池
 */
void ThreadPool::closeThreadPool() {
	// 判断
	if (this->m_is_close_thread_pool) {
		return ;
	}

	{
        std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);
        this->m_is_close_thread_pool = true;
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
 * @description: 获取线程池线程数量
 * @return {size_t} this->m_threads.size()
 */
inline size_t ThreadPool::getThreadsAmount() {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);
	return this->m_threads.size();
}


/**
 * @description: 获取线程池任务量最大值
 * @return {size_t} this->m_max_task_amount
 */
inline size_t ThreadPool::getTaskMaxAmount() {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);
	return this->m_max_task_amount;
}


/**
 * @description: 设置线程池任务量最大值
 * @param {size_t} max: 任务量最大值
 */
inline void ThreadPool::setTaskMaxAmount(size_t max) {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);
	this->m_max_task_amount = max;
}


/**
 * @description: 通过 chrono::milliseconds 修改超时时长
 * @param {milliseconds} new_timeout: 新的超时时长
 */
inline void ThreadPool::setTaskTimeoutByMilliseconds(std::chrono::milliseconds new_timeout) {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);

	this->m_task_timeout = new_timeout;
}


/**
 * @description: 通过 chrono::seconds 修改超时时长
 * @param {seconds} new_timeout: 新的超时时长
 */
inline void ThreadPool::setTaskTimeoutBySeconds(std::chrono::seconds new_timeout) {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);

	this->m_task_timeout = std::chrono::duration_cast<std::chrono::milliseconds>(new_timeout);
}


/**
 * @description: 获取当前设置的任务优先级
 * @return {size_t} this->m_priority_level
 */
inline size_t ThreadPool::getTaskPriority() {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);

	return this->m_priority_level;
}


/**
 * @description: 修改当前设置的任务优先级
 * @param {size_t} priority: 任务优先级
 */
inline void ThreadPool::setTaskPriority(size_t priority) {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);

	this->m_priority_level = priority;
}


/**
 * @description: 展示线程池工作模式
 */
void ThreadPool::showThreadPoolWorkMode() {
	std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);

	if (this->m_thread_pool_work_mode == ThreadPoolWorkMode::FIXED_THREAD) {
		std::cout << "FIXED_THREAD" << std::endl;
	}
	else {
		std::cout << "MUTABLE_THREAD" << std::endl;
	}
		
}


/**
 * @description: 初始化线程池
 */
void ThreadPool::initThreadPool() {
	for (int i = 0; i < this->m_min_thread_threshold; ++i) {
		// std::thread 调用类的成员函数需要传递类的一个对象作为参数， 由于是 operator() 下面两种写法都可以，如果是类内部，传入 this 指针即可
		// this->m_threads[i] = std::thread(Worker(this, i));  // 分配工作线程
		this->m_threads[ThreadPool::m_threads_id] = std::thread(&Worker::operator(), Worker(this, ThreadPool::m_threads_id));  // 指定线程所执行的函数
		ThreadPool::m_threads_id++;
		this->m_thread_amount++;
	}
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
		std::unique_lock<std::mutex> lock(this->m_thread_pool_mutex);

		// 将打包好的无参任务函数转换成 void 函数
		std::function<void()> warpper_func = [task_ptr]() {
			(*task_ptr)();  //  (*指针变量名) (函数参数列表)
		};

		// 如果线程池已经决定关闭，则不可再提交任务
		if (this->m_is_close_thread_pool) {
			std::cout << "线程池已被关闭，无法提交新任务" << std::endl;
			throw std::runtime_error("ThreadPool is already colsed");
		}

		// 如果任务数已满，等待线程执行
		if (this->m_max_task_amount == this->m_task_queue.safeQueueSize()) {
			std::cout << "任务队列已满, 请等待任务完成" << std::endl;

			// 用户提交任务，超过时长，否则算提交任务失败
			if (std::cv_status::timeout == this->m_conditional_safe_queue_not_full.wait_for(lock, std::chrono::milliseconds(this->m_task_timeout))) {
				// 表示等待一秒 条件依然没有满足
				std::cerr << "提交任务超时，请稍后重尝..." << std::endl;
				return return_future;
			}
		}

		// 任务入队
		this->m_task_queue.taskEnqueue(warpper_func, this->m_priority_level);
	}

	// 动态添加线程
	if (this->m_thread_pool_work_mode == ThreadPoolWorkMode::MUTABLE_THREAD
		&& this->getThreadsAmount() < this->m_task_queue.safeQueueSize()
		&& this->getThreadsAmount() < this->m_max_thread_threshold
		&& this->getThreadsAmount() < std::thread::hardware_concurrency()
	) {
		this->m_threads[ThreadPool::m_threads_id] = std::thread(&Worker::operator(), Worker(this, ThreadPool::m_threads_id));  // 指定线程所执行的函数
		ThreadPool::m_threads_id++;
		this->m_thread_amount++;
		std::cout << "已动态添加新线程，当前线程数量为: " << this->getThreadsAmount() << "  ----->   " << this->m_max_thread_threshold << std::endl;
	}

	// 唤醒一个等待中的线程
	this->m_conditional_safe_queue_not_empty.notify_one();
	
	return return_future;
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

	while (!this->m_pool->m_is_close_thread_pool || !this->m_pool->m_task_queue.isSafeQueueEmpty()) {
		{
			// 线程池加锁
			std::unique_lock<std::mutex> lock(this->m_pool->m_thread_pool_mutex);

			std::cout << "tid: " << std::this_thread::get_id() << " 正在尝试获取任务" << std::endl;

			// 如果任务队列为空，阻塞当前线程
			if (this->m_pool->m_task_queue.isSafeQueueEmpty()) {
				std::cout << "任务队列空，等待任务..." << std::endl;
				if (this->m_pool->m_thread_pool_work_mode == ThreadPoolWorkMode::FIXED_THREAD) {
					this->m_pool->m_conditional_safe_queue_not_empty.wait(lock);  // 等待任务
				}
				else if (this->m_pool->m_thread_pool_work_mode == ThreadPoolWorkMode::MUTABLE_THREAD) {
					
					if (std::cv_status::timeout == this->m_pool->m_conditional_safe_queue_not_empty.wait_for(
						lock, std::chrono::milliseconds(this->m_pool->m_task_timeout))) {
						
						if (this->m_pool->m_thread_amount > this->m_pool->m_min_thread_threshold) {
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
				<< "  ----->   " << this->m_pool->m_max_task_amount << std::endl;
			func();
		}
		else {
			std::cout << "tid: " << std::this_thread::get_id() << " 取出任务失败" << std::endl;
		}
			
	}

}
#endif  // !THREAD_POOL_H__