cmake_minimum_required (VERSION 3.12)

project (ChernoOpenGL)

# Setup project sources
include_directories("${CMAKE_SOURCE_DIR}/src")

file(GLOB_RECURSE HEADER_FILES "${CMAKE_SOURCE_DIR}/src/*.hpp")
file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

# Setup libraries
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
add_subdirectory("Dependencies/glfw")

set(BUILD_SHARED_LIBS          OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_EXAMPLES      OFF CACHE BOOL "" FORCE)
set(OPTION_BUILD_TOOLS         OFF CACHE BOOL "Avoid: Tool <tool> skipped: glfw3 not found" FORCE)
set(OPTION_USE_GIT_INFORMATION OFF CACHE BOOL "" FORCE)
string(REGEX REPLACE "/W3" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}") # Avoid: Command line warning D9025: overriding '/W3' with '/W4'
add_subdirectory("Dependencies/glbinding")
string(REGEX REPLACE "/W4" "/W3" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# Apply dependencies
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw glbinding::glbinding glbinding::glbinding-aux) # (last) optional auxiliary (glbinding) lib

# Deal with resource dependencies (of the executable)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}) # Not the best practice, but it works for now
