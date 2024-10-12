if(NOT DEFINED CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr")
endif()

file(GLOB_RECURSE files
    "${CMAKE_INSTALL_PREFIX}/bin/topd"
    "${CMAKE_INSTALL_PREFIX}/include/topd/linux.h"
)

foreach(file ${files})
    if(EXISTS "${file}")
        message(STATUS "Removing ${file}")
        file(REMOVE "${file}")
    endif()
endforeach
