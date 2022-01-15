cmake_minimum_required(VERSION 3.8)

project(ofs LANGUAGES CXX)

if (NOT TARGET ${PROJECT_NAME})
  add_library(${PROJECT_NAME}
    mutex.cpp
    mutexlocker.cpp
  )

  target_include_directories(${PROJECT_NAME}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}
  )
endif()
