cmake_minimum_required(VERSION 3.14)
project(voltage-divider-calculator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # error if compiler doesn't support c++11

add_executable(vdcalc
	main.cpp)

target_link_libraries(vdcalc
	m)
