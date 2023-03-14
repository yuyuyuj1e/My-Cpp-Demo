# 引入头文件目录
include_directories(
    ./include
)

# 添加源文件到自定义的变量中
aux_source_directory(./src SRC_LIST)
aux_source_directory(./test TEST_LIST)

# 指定生成可执行文件
add_executable(threadpool ${SRC_LIST} ${TEST_LIST})

# 指定链接到目标文件所需的库
target_link_libraries(threadpool PRIVATE pthread)
