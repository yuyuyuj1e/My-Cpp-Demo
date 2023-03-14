# 引入头文件目录
include_directories(
    ./include
)

# 添加源文件到自定义的变量中
aux_source_directory(./src SRC_LIST)
aux_source_directory(./test CLIENT)
list(FILTER CLIENT INCLUDE REGEX "client.cpp")
aux_source_directory(./test SERVER)
list(FILTER SERVER INCLUDE REGEX "server.cpp")

# 设置可执行文件存放路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Communication/bin)

# 指定生成可执行文件
add_executable(server ${SERVER} ${SRC_LIST})
add_executable(client ${CLIENT} ${SRC_LIST})