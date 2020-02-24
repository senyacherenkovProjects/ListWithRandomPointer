cmake_minimum_required(VERSION 2.8)

project(saber)
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

set(CMAKE_BUILD_TYPE DEBUG)
include_directories(${Boost_INCLUDE_DIR})

add_executable(${PROJECT_NAME} "main.cpp")

set_target_properties(${PROJECT_NAME} PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS -Wpedantic -Wall -Wextra
)

add_compile_options(lboost_system)

target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK
    INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR}
)

enable_testing()

add_test(test_version_valid ${PROJECT_NAME})

