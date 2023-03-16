# 设置 CMake 最低版本要求
cmake_minimum_required(VERSION 3.0) 

# 设置项目名称
project(CppTinyFrame LANGUAGES CXX)

# 设置 C++11 标准
set(CMAKE_CXX_STANDARD 11)

# 添加子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/ThreadPool)
add_subdirectory(${PROJECT_SOURCE_DIR}/Communication)
add_subdirectory(${PROJECT_SOURCE_DIR}/CppLog)
