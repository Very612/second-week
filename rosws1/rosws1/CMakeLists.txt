cmake_minimum_required(VERSION 3.8)
project(rosws1)
# 默认编译选项
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()
# 查找依赖包
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
# 包含头文件目录
include_directories(
  include
)
# 编译 mission2P 节点（发布者）
add_executable(mission2P
  src/mission2P.cpp
)
ament_target_dependencies(mission2P
  rclcpp
  std_msgs
)
# 编译 mission2S 节点（订阅者）
add_executable(mission2S
  src/mission2S.cpp
)
ament_target_dependencies(mission2S
  rclcpp
  std_msgs
)
# 安装可执行文件到 lib/<package_name>/ 目录
install(TARGETS
  mission2P
  mission2S
  DESTINATION lib/${PROJECT_NAME}
)
# 安装 include 目录（如果有头文件）
install(DIRECTORY
  include/
  DESTINATION include/
)
# 测试相关（可选）
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()
ament_package()
