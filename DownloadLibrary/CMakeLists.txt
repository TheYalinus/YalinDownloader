cmake_minimum_required(VERSION 3.10)
add_library(DownloadLibrary download_library.hpp curl_request.cpp download_task.cpp)
target_include_directories(DownloadLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
find_package(CURL 8.12.0 REQUIRED)
find_package(nlohmann_json 3.12.0 REQUIRED)


target_link_libraries(DownloadLibrary PRIVATE nlohmann_json::nlohmann_json)

target_link_libraries(DownloadLibrary PRIVATE CURL::libcurl)
