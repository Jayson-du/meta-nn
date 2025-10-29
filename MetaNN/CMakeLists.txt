add_subdirectory(data)
add_subdirectory(facilities)

# # 设置静态库输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# # 库项目添加文件
add_library(MetaNN INTERFACE)