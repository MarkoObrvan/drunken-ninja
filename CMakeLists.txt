# Declare the version of the CMake API for forward-compatibility
cmake_minimum_required(VERSION 2.8)

# Declare the name of the CMake Project
project(drunken_ninja)

find_package(PCL REQUIRED)
find_package(sensor_msgs REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  #pcl
  pcl_ros
  roscpp
  pcl_conversions
  sensor_msgs
 # pcl
)

# Declare this project as a catkin package
catkin_package()

# Find and get all the information about the roscpp package
find_package(roscpp REQUIRED)

# Add the headers from roscpp
include_directories(${roscpp_INCLUDE_DIRS})
include_directories(${catkin_INCLUDE_DIRS})
include_directories(${PCL_INCLUDE_DIRS})

link_directories(${PCL_LIBRARY_DIRS})
# Define an execuable target called socket_node 
add_executable(socket_node src/CAN_parser.cpp src/Class_SRR_track.cpp src/ClassSRRCluster.cpp src/ClassARSTarget.cpp src/ClassARSObject.cpp) 
# Link the hello_world_node target against the libraries used by roscpp
target_link_libraries(socket_node ${roscpp_LIBRARIES})
target_link_libraries(socket_node ${PCL_LIBRARIES})
