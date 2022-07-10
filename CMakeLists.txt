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

set(ONLY_LIBS               ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED NO CACHE BOOL "" FORCE)
add_definitions(-DGLEW_STATIC)
add_subdirectory("Dependencies/glew-cmake")

# Apply dependencies
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw libglew_static)

include_directories(SYSTEM
  "${CMAKE_SOURCE_DIR}/Dependencies/stb"
)

# Deal with resource dependencies (of the executable)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}) # Not the best practice, but it works for now
