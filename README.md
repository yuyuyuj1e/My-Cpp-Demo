# Multi-Client-Communication-System-Based-on-Thread-Pool
## 通信模块实现功能
1. 封装了用于 ```TCP``` 通信的 ```C++``` 类
    - ```TcpSocket```: 用于通信的 ```socket```
    - ```TcpServer```: 用于服务器部署，绑定服务器 ```IP``` 及监听端口，等待客户端连接
2. 在监听函数中给定端口后，无需用户操作，```TcpServer``` 内部自动绑定端口，并进行监听；在客户端连接时，返回一个用于通信的套接字类 ```TcpSocket```
3. 多种信息发送方式
    - ```C``` 风格: ```int sendMessage(const char*, size_t)```
    - ```C++``` 风格: ```int sendMessage(std::string)```
4. 统一的信息接收方式，无论对方是用 ```C``` 风格方式发送，还是 ```C++``` 风格方式发送，统一返回 ```string``` 字符串
5. 类内部自动解决 TCP "粘包"问题，用户无需进行相关设置
6. 实时反映双方连接状态

---
## 线程池实现功能
1. 多种工作模式 (```enum class```)
   - ```FIXED_THREAD```: 线程数量固定 (线程池开始时给定的参数，但是不能超过超过硬件支持的数量，超过则改为硬件支持的数量) —— 不会随任务多少而改变。
   - ```MUTABLE_THREAD```: 线程数量可变 (线程池开始时给定的参数作为下限，其二倍作为上限；但是不能超过超过硬件支持的数量，超过则改为硬件支持的数量；如果上下限全部超过，则行为等同于 ```FIXED_THREAD``` 模式) —— 当任务数量超过线程数量时，增加线程 (不超过线程上限)；当线程超过一定时间无法接到任务时，释放线程 (不低于线程下线)。
2. 任务优先级 (```priority_queue``` + ```pair```)
    - 线程优先获取更高优先级任务 （仿函数设置 ```priority_queue``` 比较函数）。
    - 任务优先级相同时，线程优先获取先进任务队列的任务 (严格弱排序)。
3. 任务提交超时时长 (```condition_variable.wait_for()```)
	- 多种设置超时时长方式。
4. 可接受任意返回类型和任意参数的任务函数 (将有返回值有参函数转换为无返回值无参函数)。
5. 有限/无限长度任务队列 (当任务队列有限时，才会出现任务提交超时的判定)。
6. 有限线程数量 (不能超过 ```std::thread::hardware_concurrency()```)。
7. 统一的工作线程入口 (仿函数的 ```operator()```)。
8. 可手动关闭线程池，也可自动关闭线程池 (析构函数会自动调用关闭线程池方法，并且会判断线程池是否已经被关闭，如已关闭，析构函数不会执行任何操作)。
9. 线程池关闭后，会等待任务队列所有任务结束 (两个条件，线程池关闭 ```bool``` 变量 + 任务队列为空)，且阻止用户继续提交任务。
10. 实时更新并反馈任务队列以及线程队列状态 (原子变量)。
11. 多种线程池配置相关接口。
	- 任务队列长度。
    	- ```void setTaskMaxAmount(size_t);```
    	- ```size_t getTaskMaxAmount();```
	- 任务提交超时时长。
    	-  ```void setTaskTimeoutByMilliseconds(std::chrono::milliseconds);```
    	-  ```void setTaskTimeoutBySeconds(std::chrono::seconds);```
	- 任务优先级。
    	- ```size_t getTaskPriority();```
    	- ```void setTaskPriority(size_t);```
	- 线程池工作模式。
    	- ```void showThreadPoolWorkMode();```