cmake_minimum_required(VERSION 3.8)

project(ofshash LANGUAGES CXX)

if (NOT TARGET ${PROJECT_NAME})
  add_library(${PROJECT_NAME}
    base64.cpp
    ofshash.cpp
    sha1.c
  )

  target_include_directories(${PROJECT_NAME}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}
  )
endif()
