cmake_minimum_required(VERSION 3.14)
project(lab3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)
enable_testing()

add_executable(lab3 main.cpp)
target_include_directories(lab3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(lab3_tests tests/test_graph.cpp)
target_include_directories(lab3_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(lab3_tests PRIVATE GTest::gtest_main)
gtest_discover_tests(lab3_tests)

find_package(Qt6 COMPONENTS Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets QUIET)
endif()

if(Qt6_FOUND)
    set(QT_LIB Qt6::Widgets)
    message(STATUS "Using Qt6")
elseif(Qt5_FOUND)
    set(QT_LIB Qt5::Widgets)
    message(STATUS "Using Qt5")
endif()

if(QT_LIB)
    add_executable(lab3_gui
        gui/main.cpp
        gui/Gui.cpp
        gui/Gui.hpp
    )
    set_target_properties(lab3_gui PROPERTIES AUTOMOC ON)
    target_include_directories(lab3_gui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(lab3_gui PRIVATE ${QT_LIB})
    message(STATUS "GUI target lab3_gui enabled")
else()
    message(WARNING "Qt not found — GUI disabled")
endif()