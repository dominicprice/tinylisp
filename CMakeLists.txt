# CMakeList.txt : CMake project for tinylisp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("tinylisp")

option(BUILD_TESTS "Build tests" ON)

# Add source to this project's executable.
add_executable (tinylisp
	"src/tinylisp.c"
	"src/object.c"
	"src/stack.c"
	"src/parse.c" 
	"src/eval.c"
	"src/builtins.c" 
	"src/error.c"
	"src/main.c"
)

if (BUILD_TESTS)
	message(STATUS "Building tests")
	add_test(NAME simple COMMAND tinylisp simple.tl WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()