cmake_minimum_required(VERSION 3.8)
project(haruto_description)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)

find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(talker script/min.cpp)
ament_target_dependencies(talker rclcpp std_msgs sensor_msgs)

add_executable(auto script/auto.cpp)
ament_target_dependencies(auto rclcpp std_msgs sensor_msgs)


install(
  DIRECTORY urdf worlds script
  DESTINATION share/${PROJECT_NAME}
)

install(TARGETS
  talker
  DESTINATION lib/${PROJECT_NAME})

install(TARGETS
  auto
  DESTINATION lib/${PROJECT_NAME})


install(
  DIRECTORY rviz
  DESTINATION share/${PROJECT_NAME}
)

install(
  DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
