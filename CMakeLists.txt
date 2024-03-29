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

set(ONLY_LIBS               ON CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED NO CACHE BOOL "" FORCE)
add_subdirectory("deps/glew-cmake")

add_subdirectory("deps/glm")

## Setup ImGui library
set(IMGUI_SOURCE_DIR_ "${CMAKE_SOURCE_DIR}/deps/imgui")
set(IMGUI_BACKENDS_SOURCE_FILES_ "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_glfw.cpp" # :2 api-dependend includes!
                                 "${IMGUI_SOURCE_DIR_}/backends/imgui_impl_opengl3.cpp")
file(GLOB IMGUI_SOURCE_FILES_    "${IMGUI_SOURCE_DIR_}/*.cpp")
add_library               (imgui STATIC ${IMGUI_SOURCE_FILES_} ${IMGUI_BACKENDS_SOURCE_FILES_})
target_include_directories(imgui SYSTEM PUBLIC  "${IMGUI_SOURCE_DIR_}"
                                        PRIVATE "${CMAKE_SOURCE_DIR}/deps/glfw/include")
set_target_properties     (imgui PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

# Apply dependencies
target_link_libraries     (${PROJECT_NAME} OpenGL::GL glfw libglew_static glm::glm imgui)
target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE
  "${CMAKE_SOURCE_DIR}/deps"
)
