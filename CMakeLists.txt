cmake_minimum_required(VERSION 3.25.1)
project(main VERSION 1.0)

# Set C standard
set(CMAKE_C_STANDARD 99)

#Set cJSON path
set(cJSON_INCLUDE_DIRS /home/chakety/Downloads/cJSON/include)
set(cJSON_LIBRARIES /home/chakety/Downloads/cJSON/build/libcjson.so)

#Include cJSON header
include_directories(${cJSON_INCLUDE_DIRS})

# Find required packages
find_package(CURL REQUIRED)
find_package(cJSON REQUIRED)
#include directories
include_directories(lib/dev)
include_directories(lib/EPD)
include_directories(lib/Config)
include_directories(lib/Driver)
include_directories(lib/GUI)
include_directories(lib/Fonts)
# Define an executable for the project
add_executable(main main.c func/weather.c, func/memo.c) 
# Include directories for the executable
target_include_directories(main PRIVATE ${CURL_INCLUDE_DIRS} ${cJSON_INCLUDE_DIRS} lib/Driver lib/EDP lib/COnfig)

# Link libraries
target_link_libraries(main PRIVATE ${CURL_LIBRARIES} ${cJSON_LIBRARIES})
