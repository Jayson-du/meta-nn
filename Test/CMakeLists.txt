# 设置头文件目录
include_directories("${MetaNN_PROJECT}/"
                    "${CMAKE_CURRENT_SOURCE_DIR}/")

# 项目添加文件
add_executable(test
               main.cpp
)