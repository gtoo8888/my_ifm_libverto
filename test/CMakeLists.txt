cmake_minimum_required(VERSION 3.10)
project(mylibverto)

set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 ")

aux_source_directory(src SRC_LIST)

# find_package(Libevent REQUIRED)

link_directories(/usr/local/lib)
include_directories(/usr/local/include)
include_directories(${CMAKE_SOURCE_DIR}/src/include)

set(test_sources
  # src/signal.cc
  # src/write.cc
  # src/read.cc
  src/timeout.cc
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# 启用测试功能
enable_testing()

message(WARNING "SRC_LIST: ${SRC_LIST}")
message(WARNING "include: ${CMAKE_SOURCE_DIR}/src/include")

# 添加测试程序的可执行目标
foreach(test_source ${test_sources})
  # 获取文件名（不含扩展名）
  get_filename_component(test_name ${test_source} NAME_WE)
  # 添加可执行目标
  # add_executable(${test_name} ${test_source}  src/main.cc src/ifm_libverto_test.h)
  add_executable(${test_name} ${test_source} src/main.cc src/ifm_verto.cc)
  target_link_libraries(${test_name} verto)
  # 添加测试
  add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()

# 添加链接库路径
# link_directories(/usr/local/lib)
# # 生成一个可执行文件
# add_executable(${PROJECT_NAME} ${SRC_LIST})
# # 链接目标库
# target_link_libraries(${PROJECT_NAME} verto)














