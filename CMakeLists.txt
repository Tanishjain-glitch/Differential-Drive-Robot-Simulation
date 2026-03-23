cmake_minimum_required(VERSION 3.8)
project(rse004_robot)

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(nlohmann_json REQUIRED)

include_directories(include)

add_executable(script_a src/script_a.cpp)
ament_target_dependencies(script_a rclcpp geometry_msgs)
target_link_libraries(script_a rt pthread)

add_executable(script_b src/script_b.cpp)
target_link_libraries(script_b rt pthread nlohmann_json::nlohmann_json)

install(TARGETS script_a script_b
  DESTINATION lib/${PROJECT_NAME})

ament_package()