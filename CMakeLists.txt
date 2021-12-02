cmake_minimum_required(VERSION 3.12)

SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(CMAKE_C_COMPILER "/usr/bin/clang-13")
SET(CMAKE_CXX_COMPILER "/usr/bin/clang++-13")

project(imgui_test VERSION 0.0.1 DESCRIPTION "imgui test app")

set(CMAKE_BUILD_TYPE Debug)

# wpilib
find_package(wpilib REQUIRED)

# imgui
include(${CMAKE_CURRENT_LIST_DIR}/imgui/CMakeLists.txt)

# OpenCV
find_package(OpenCV REQUIRED)

add_executable(imgui_test
	src/main.cpp
)

target_include_directories(imgui_test PRIVATE inc)

set(CPP_FLAGS -Wall -Wpedantic -Wextra -Wshadow -pthread -g)
target_compile_options(imgui_test PUBLIC
	$<$<COMPILE_LANGUAGE:CXX>: ${CPP_FLAGS}>
)

target_link_libraries(imgui_test
	${OpenCV_LIBS}
	-lpthread
	cameraserver
	ntcore
	cscore
	wpiutil
	# wpigui
	imgui
	-lGL
	# -lglfw3
	-lglfw
)
