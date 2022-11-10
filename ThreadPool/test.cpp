#include <iostream>
#include "ThreadPool.h"

// 普通函数
void multiply(const int a, const int b) {
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	const int res = a * b;
	std::cout << "tid: " << std::this_thread::get_id() << " 正在执行任务：" << a << " * " << b << " = " << res << std::endl << std::endl;
}

// 引用
void multiply_output(int &out, const int a, const int b) {
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	out = a * b;
	std::cout << a << " * " << b << " = " << out << std::endl;
}

// 结果返回
int multiply_return(const int a, const int b) {
	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
	const int res = a * b;
	std::cout << a << " * " << b << " = " << res << std::endl;
	return res;
}


int main() {
	// 创建线程池
	ThreadPool pool(3, ThreadPoolWorkMode::MUTABLE_THREAD);

	// 任务队列最大承载量
	pool.setTaskMaxAmount((size_t)4);

	// 设置提交任务超时时长
	// pool.setTaskTimeoutByMilliseconds(std::chrono::milliseconds(1000));
	// pool.setTaskTimeoutBySeconds(std::chrono::seconds(1));

	// 提交乘法操作
	for (int i = 1; i <= 3; ++i) {
		for (int j = 1; j <= 5; ++j) {
			pool.submitTask(multiply, i, j);
		}
	}

	pool.setTaskPriority(3);
	// 提交乘法操作
	for (int i = 4; i <= 6; ++i) {
		for (int j = 4; j <= 5; ++j) {
			pool.submitTask(multiply, i, j);
		}
	}

	// 如果关闭线程池，下面任务无法提交
	// pool.closeThreadPool();

	pool.setTaskPriority(4);
	// 使用ref传递的输出参数提交函数
	int output_ref;
	auto future1 = pool.submitTask(multiply_output, std::ref(output_ref), 5, 6);
	try {
		future1.get();  // 等待乘法输出完成
		std::cout << "Last operation result is equals to " << output_ref << std::endl;
	} catch(const std::exception& e) {
		std::cerr << "任务提交失败"  << std::endl;
	}


	// 使用return参数提交函数
	auto future2 = pool.submitTask(multiply_return, 5, 3);
	try {
		int res = future2.get();  // 等待乘法输出完成
		std::cout << "Last operation result is equals to " << res << std::endl;
	} catch(const std::exception& e) {
		std::cerr << "任务提交失败" << std::endl;
	}
	
	// 提交乘法操作
	for (int i = 10; i <= 16; ++i) {
		for (int j = 1; j <= 5; ++j) {
			pool.submitTask(multiply, i, j);
		}
	}
		
	// 关闭线程池，可手动关闭，也可自动关闭
	pool.closeThreadPool();
	return 0;
}
