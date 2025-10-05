cmake_minimum_required(VERSION 3.20)
project(Chip8 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# output dirs
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
foreach(cfg Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${cfg} CFG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CFG} ${CMAKE_BINARY_DIR}/bin/${cfg})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CFG} ${CMAKE_BINARY_DIR}/lib/${cfg})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CFG} ${CMAKE_BINARY_DIR}/lib/${cfg})
endforeach()

# SDL3
option(SDL_SHARED "Build shared SDL library" OFF)
option(SDL_STATIC "Build static SDL library" ON)
option(SDL_TEST "Build SDL test programs" OFF)
add_subdirectory(thirdparty/SDL)  # provides SDL3::SDL3-static
add_subdirectory(thirdparty/fmt)

# App sources/headers
file(GLOB_RECURSE C8_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE C8_PRIV_HDR CONFIGURE_DEPENDS "src/*.h" "src/*.hpp")
file(GLOB_RECURSE C8_HEADERS CONFIGURE_DEPENDS
  "src/*.h" "src/*.hpp"
  "include/**/*.h" "include/**/*.hpp"
)

add_executable(Chip8 ${C8_SOURCES} ${C8_PRIV_HDR} ${C8_HEADERS})

# Your include paths (public if you plan to export headers)
target_include_directories(Chip8
  PUBLIC  ${PROJECT_SOURCE_DIR}/include
  PRIVATE ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(Chip8 PRIVATE SDL3::SDL3)
target_link_libraries(Chip8 PRIVATE fmt::fmt)
set_target_properties(Chip8 PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED ON)

# Pretty groups in IDEs
source_group(TREE "${PROJECT_SOURCE_DIR}/src"     FILES ${C8_SOURCES} ${C8_PRIV_HDR})
source_group(TREE "${PROJECT_SOURCE_DIR}/include" FILES ${C8_PUB_HDR})