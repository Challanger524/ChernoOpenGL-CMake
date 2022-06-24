cmake_minimum_required (VERSION 3.21)

project (ChernoOpenGL VERSION       2.0
                      DESCRIPTION  "Cross-platform restoration of TheCherno's OpenGL series"
                      HOMEPAGE_URL "https://github.com/Challanger524/ChernoOpenGL-CMake"
                      LANGUAGES     CXX)

# Setup main project
file(GLOB_RECURSE _SOURCE_FILES CONFIGURE_DEPENDS
  "${PROJECT_SOURCE_DIR}/src/*.hpp"
  "${PROJECT_SOURCE_DIR}/src/*.cpp"
)
## exclude files starting with uncderscore `_<file>`: https://regex101.com/r/DT6qGi/1
list(FILTER _SOURCE_FILES EXCLUDE REGEX ".*\\/_.*\\..*")

add_executable            (${PROJECT_NAME} ${_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "${PROJECT_SOURCE_DIR}/src")
set_target_properties     (${PROJECT_NAME} PROPERTIES
  CXX_STANDARD          17
  CXX_STANDARD_REQUIRED ON
  RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}$<0:>" # deal with resource deps
)

# Setup libraries
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_EXAMPLES NO CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    NO CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     NO CACHE BOOL "" FORCE)
add_subdirectory("deps/glfw")

# Apply dependencies
target_link_libraries(${PROJECT_NAME} OpenGL::GL glfw)
