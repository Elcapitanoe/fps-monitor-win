cmake_minimum_required(VERSION 3.10)
project(FPSOverlay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows-specific configuration
if(WIN32)
    set(CMAKE_SYSTEM_VERSION 6.1)  # Windows 7 minimum
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0601)  # Windows 7
endif()

# Compiler flags for optimization and compatibility
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /O2")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2 -static-libgcc -static-libstdc++")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/fps_overlay.cpp
    src/hook_manager.cpp
    src/renderer.cpp
    src/config_manager.cpp
    src/utils.cpp
)

set(HEADERS
    include/fps_overlay.h
    include/hook_manager.h
    include/renderer.h
    include/config_manager.h
    include/utils.h
    include/common.h
)
endif()

# Create executable
add_executable(FPSOverlay ${SOURCES} ${HEADERS} ${RESOURCES})

# Windows libraries
if(WIN32)
    target_link_libraries(FPSOverlay
        user32
        gdi32
        kernel32
        d3d9
        d3d11
        dxgi
        opengl32
        psapi
        shlwapi
    )
endif()

# Static linking for single executable
if(MSVC)
    # Set compiler flags for static runtime
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
    
    set_target_properties(FPSOverlay PROPERTIES
        WIN32_EXECUTABLE FALSE
        LINK_FLAGS "/SUBSYSTEM:CONSOLE /INCREMENTAL:NO"
    )
        WIN32_EXECUTABLE FALSE
    )
endif()