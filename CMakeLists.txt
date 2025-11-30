cmake_minimum_required(VERSION 3.14)

project(lfmq VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

set(TARGET lfmq)

if (BUILD_SHARED_LIBS)
    message("Building as a shared library - yes")
else ()
    message("Building as a static library - yes")
endif()

set(HEADER_FILES
   include/lfmq/message.hpp
   include/lfmq/lock_free_queue.hpp
)

add_library(${TARGET}
   src/message.cpp
   ${HEADER_FILES}
)
add_library(${TARGET}::${TARGET} ALIAS ${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
    CMAKE_CXX_STANDARD 20
    CMAKE_CXX_STANDARD_REQUIRED 20
)

target_include_directories(${TARGET}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lfmq>
)

get_target_property(TARGET_INCLUDE_DIR ${TARGET} INCLUDE_DIRECTORIES)

# Include useful directory helpers for target installation
include(GNUInstallDirs)

# Configure installation destinations
install(
    TARGETS ${TARGET}
    EXPORT ${TARGET}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Configure include header install destination and files
install(
    FILES ${HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${TARGET}
)

# Actually install the Targets file
install(
    EXPORT ${TARGET}Targets
    NAMESPACE ${TARGET}::
    FILE ${TARGET}Targets.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
)

# Include the package configuration helpers for config installation
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}Config.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${TARGET}
)
