# 引入头文件目录
include_directories(
    ./include
)

# 添加源文件到自定义的变量中
aux_source_directory(./src SRC_LIST)
aux_source_directory(./test TEST_LIST)

# 设置可执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/CppLog/bin)

# 指定生成可执行文件
add_executable(log ${TEST_LIST} ${SRC_LIST})

# 指定链接到目标文件所需的库
target_link_libraries(log PRIVATE pthread)
