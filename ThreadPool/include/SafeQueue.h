/** 
 * @author: yuyuyuj1e 807152541@qq.com
 * @github: https://github.com/yuyuyuj1e
 * @csdn: https://blog.csdn.net/yuyuyuj1e
 * @date: 2023-03-14 10:19:29
 * @last_edit_time: 2023-03-14 13:50:55
 * @file_path: /Tiny-Cpp-Frame/ThreadPool/include/SafeQueue.h
 * @description: 线程池任务队列头文件
 */

#pragma once
#include <queue>
#include <mutex>

template<typename T>
class SafeQueue {
private:
	// 优先级队列，比较函数
	struct cmp {  
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
	bool empty();  // 队列是否为空
	size_t safeQueueSize();  // 任务队列大小
	void taskEnqueue(T &, size_t);  // 添加任务
	bool taskDequeue(T &);  // 取出任务
};


/**
 * @description: 判断任务队列是否为空
 * @return {bool} this->m_safe_queue.empty()
 */
template<typename T>
bool SafeQueue<T>::empty() {
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
	std::unique_lock<std::mutex> lock(this->m_safe_queue_mutex);  // 任务队列上锁

	if (this->m_safe_queue.empty())
		return false;

	t = std::move(this->m_safe_queue.top().first);  // 取出队首元素，返回队首元素值，并进行右值引用
	this->m_safe_queue.pop();  // 弹出入队的第一个元素

	return true;
}