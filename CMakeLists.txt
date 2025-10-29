# 最低版本
cmake_minimum_required(VERSION 3.4)

# 工程名称
project (MetaNN C CXX)

# 编译类型, 如果没有指定则默认为Debug
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Debug")
endif()


# 如果CMAKE_CXX_STANDARD_REQUIRED设置为True，则必须使用CMAKE_CXX_STANDARD指定的版本，
# 如果CMAKE_CXX_STANDARD_REQUIRED设置为OFF则CMAKE_CXX_STANDARD指定版本的为首选版本，如果没有会使用上一版本
set(CMAKE_CXX_STANDARD_REQUIRED True)
# 设置C++版本, gcc或者clang的c++17版本可以编译通过; VC必须设置到20才能通过
set(CMAKE_CXX_STANDARD 20)

# 设置工程目录
set(MetaNN_PROJECT ${CMAKE_CURRENT_SOURCE_DIR})

# 如果是MSVC开启项目分组
if (MSVC)
  set_property (GLOBAL PROPERTY USE_FOLDERS ON)
endif()

# 添加MetaNN项目, 编译MetaNN静态库
add_subdirectory(MetaNN)
add_subdirectory(Test)