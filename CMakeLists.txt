cmake_minimum_required(VERSION 3.10)
project(mylibverto)

# set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11 ") # c++编译选项
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0") # c语言编译选项


set(test_sources
  src/main.c
  src/ifm_verto.c
  src/ifm_verto.h
  src/ifm_verto_module.h
)

add_executable(mylibverto ${test_sources} )
# target_link_libraries(mylibverto)



# # 添加测试程序的可执行目标
# foreach(test_source ${test_sources})
#   # 获取文件名（不含扩展名）
#   get_filename_component(test_name ${test_source} NAME_WE)
#   # 添加可执行目标
#   # add_executable(${test_name} ${test_source}  src/main.cc src/ifm_libverto_test.h)
#   add_executable(${test_name} ${test_source} src/main.cc src/ifm_verto.cc)
#   target_link_libraries(${test_name} verto)
#   # 添加测试
#   add_test(NAME ${test_name} COMMAND ${test_name})
# endforeach()

# 添加链接库路径
# link_directories(/usr/local/lib)
# # 生成一个可执行文件
# add_executable(${PROJECT_NAME} ${SRC_LIST})
# # 链接目标库
# target_link_libraries(${PROJECT_NAME} verto)














