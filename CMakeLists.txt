cmake_minimum_required(VERSION 3.12  FATAL_ERROR)

project(WakeUpBruh)

option (WUB_BUILD_TESTS "Build unit tests." ON)
option (WUB_BUILD_DEMO "Build demo." ON)
option (WUB_LOG_TIMER "Enable timer logs" ON)

### Header-only lib
add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} INTERFACE "${PROJECT_SOURCE_DIR}/include")
if (WUB_LOG_TIMER)
  target_compile_definitions(${PROJECT_NAME} INTERFACE WUB_LOG_TIMER)
endif ()

### Tests
if (${WUB_BUILD_TESTS})
  enable_testing ()
  add_subdirectory ("${${PROJECT_NAME}_SOURCE_DIR}/Testing")
endif ()

### Demo
if (${WUB_BUILD_DEMO})
  find_package(Threads)
  add_executable(wub)
  target_sources(wub PRIVATE "${PROJECT_SOURCE_DIR}/wub.cpp")
  target_link_libraries(wub PRIVATE ${PROJECT_NAME} Threads::Threads)
  target_compile_features(wub PRIVATE cxx_std_17)
  if (MSVC)
    target_compile_options(wub PRIVATE "/Zc:__cplusplus")
  endif ()

endif ()