# %Tag(FULLTEXT)%
cmake_minimum_required(VERSION 2.8.3)
project(loggerhead_bot)
add_compile_options(-std=c++17)
## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg)

# Declare ROS messages and services
add_message_files(FILES ps3.msg)
# add_service_files(FILES AddTwoInts.srv)

## Generate added messages and services
generate_messages(DEPENDENCIES std_msgs)

## Declare a catkin package
catkin_package()

## Build talker and listener
include_directories(include ${catkin_INCLUDE_DIRS})

find_package(Threads)

add_executable(serialRobo src/serialRobo.cpp)
target_link_libraries(serialRobo libserial.a  ${CMAKE_THREAD_LIBS_INIT} ${catkin_LIBRARIES})
add_dependencies(serialRobo loggerhead_bot_generate_messages_cpp)

add_executable(ps3_joy src/ps3_joy.cpp)
target_link_libraries(ps3_joy ${CMAKE_THREAD_LIBS_INIT} ${catkin_LIBRARIES})
add_dependencies(ps3_joy loggerhead_bot_generate_messages_cpp)
# %EndTag(FULLTEXT)%
