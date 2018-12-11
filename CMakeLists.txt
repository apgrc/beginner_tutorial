# %Tag(FULLTEXT)%
cmake_minimum_required(VERSION 2.8.3)
project(beginner_tutorials)
add_compile_options(-std=c++17)
## Find catkin and any catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs genmsg)

## Declare ROS messages and services
add_message_files(FILES Num.msg ps3.msg)
add_service_files(FILES AddTwoInts.srv)

## Generate added messages and services
generate_messages(DEPENDENCIES std_msgs)

## Declare a catkin package
catkin_package()

## Build talker and listener
# include_directories(include ${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS} /usr/include/opencv4/opencv2/)
include_directories(include ${catkin_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

add_executable(talker src/talker.cpp)
target_link_libraries(talker ${catkin_LIBRARIES})
add_dependencies(talker beginner_tutorials_generate_messages_cpp)

add_executable(listener src/listener.cpp)
target_link_libraries(listener ${catkin_LIBRARIES})
add_dependencies(listener beginner_tutorials_generate_messages_cpp)

find_package(Threads)
add_executable(serialCandy src/serialCandy.cpp)
target_link_libraries(serialCandy libserial.a  ${CMAKE_THREAD_LIBS_INIT} ${catkin_LIBRARIES})
add_dependencies(serialCandy beginner_tutorials_generate_messages_cpp)

add_executable(serialTest src/serialTest.cpp)
target_link_libraries(serialTest libserial.a  ${CMAKE_THREAD_LIBS_INIT} ${catkin_LIBRARIES})
add_dependencies(serialTest beginner_tutorials_generate_messages_cpp)

add_executable(serialRobo src/serialRobo.cpp)
target_link_libraries(serialRobo libserial.a  ${CMAKE_THREAD_LIBS_INIT} ${catkin_LIBRARIES})
add_dependencies(serialRobo beginner_tutorials_generate_messages_cpp)

find_package(OpenCV REQUIRED)
add_executable(cv_stream src/cv_stream.cpp)
target_link_libraries(cv_stream ${OpenCV_LIBS})

add_executable(ps3_joy src/ps3_joy.cpp)
target_link_libraries(ps3_joy ${CMAKE_THREAD_LIBS_INIT} ${catkin_LIBRARIES})
add_dependencies(ps3_joy beginner_tutorials_generate_messages_cpp)
# %EndTag(FULLTEXT)%
