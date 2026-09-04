cmake_minimum_required(VERSION 3.10)
file(GLOB_RECURSE headers CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
file(GLOB_RECURSE sources CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(DownloadLibrary ${headers} ${sources})
target_include_directories(
 DownloadLibrary PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                         $<INSTALL_INTERFACE:include/${PROJECT_NAME}-${PROJECT_VERSION}>
)
find_package(CURL 8.12.0 REQUIRED)
find_package(nlohmann_json 3.12.0 REQUIRED)
target_link_libraries(DownloadLibrary PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(DownloadLibrary PRIVATE CURL::libcurl)
