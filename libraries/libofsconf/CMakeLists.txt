cmake_minimum_required(VERSION 3.8)

project(ofsconf LANGUAGES CXX)

if (NOT TARGET ${PROJECT_NAME})
  add_library(${PROJECT_NAME}
    ofsconf.cpp
  )

  target_compile_definitions(${PROJECT_NAME}
    PUBLIC
      OFS_STATE_DIR="${CMAKE_INSTALL_PREFIX}/var/ofs"
  )

  target_include_directories(${PROJECT_NAME}
    PUBLIC
      ${CMAKE_CURRENT_LIST_DIR}
  )

  add_subdirectory(../libofs libofs)

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      ofs  
  )
endif()
