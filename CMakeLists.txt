cmake_minimum_required (VERSION 3.12)

project (ChernoOpenGL VERSION       2.0
                      DESCRIPTION  "CMake implementation of TheCherno's OpenGL series"
                      HOMEPAGE_URL "https://github.com/Challanger524/ChernoOpenGL-CMake"
                      LANGUAGES     CXX)

# Setup main project
file(GLOB_RECURSE _SOURCE_FILES CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.hpp"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
## exclude files starting with uncderscore `_<file>`: https://regex101.com/r/DT6qGi/1
list(FILTER _SOURCE_FILES EXCLUDE REGEX ".*\\/_.*\\..*")

add_executable(${PROJECT_NAME} ${_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}$<0:>" # deal with resource deps
)

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

target_include_directories(${PROJECT_NAME} PRIVATE SYSTEM
  "${CMAKE_SOURCE_DIR}/Dependencies/stb"
)
