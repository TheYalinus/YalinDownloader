cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
enable_testing()
include(FetchContent)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

project(
  YalinDownloader
  VERSION 1.0
  LANGUAGES CXX
)
add_subdirectory(DownloadLibrary)
#add_subdirectory(gui)
#add_executable(main main.cpp)
#target_link_libraries(main PRIVATE DownloadLibrary)
