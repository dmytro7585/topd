cmake_minimum_required(VERSION 3.10)
project(topd_v1.0)


set(CMAKE_C_STANDARD 11)


set(SOURCES
    main.c
    linux.c
)

set(HEADERS
    linux.h
)


add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})


set(CMAKE_C_FLAGS_RELEASE "-O3")
set(CMAKE_C_FLAGS_DEBUG "-g")


install(TARGETS ${PROJECT_NAME} DESTINATION /usr/bin)


if (CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake"
        IMMEDIATE @ONLY
    )
    add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/uninstall.cmake)
endif()
