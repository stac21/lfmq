cmake_minimum_required(VERSION 3.14)

project(lfmq VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

option(BUILD_SHARED_LIBS "Build as a shared library" ON)
option(BUILD_STATIC_LIBS "Build as a static library" OFF)

set(sources
   include/message.hpp
   src/message.cpp
   include/lock_free_queue.hpp)


if (BUILD_SHARED_LIBS)
   message(STATUS "Building as a shared library - yes")
   add_library(lfmq
     ${sources})

   install(TARGETS lfmq)
   target_include_directories(lfmq PUBLIC "include")
else ()
   message(STATUS "Building as a shared library - no")
endif ()

if (BUILD_STATIC_LIBS)
   message(STATUS "Building as a static library - yes")

   add_library(lfmq-static
      ${sources})

    install(TARGETS lfmq-static)
    target_include_directories(lfmq-static PUBLIC "include")
else ()
   message(STATUS "Building as a static library - no")
endif ()
