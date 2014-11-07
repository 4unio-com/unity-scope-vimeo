project(unity-scope-vimeo CXX)
cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

# We require g++ 4.9, to avoid ABI breakage with earlier version.
# Note that this must match what is specifed in debian/control.
#set(cxx_version_required 4.9)
#if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
#    if (NOT CMAKE_CXX_COMPILER_VERSION MATCHES "^${cxx_version_required}")
#        message(FATAL_ERROR "g++ version must be ${cxx_version_required}!")
#    endif()
#endif()

add_definitions(
  -fno-permissive
  -std=c++11
  -pedantic
  -Wall
  -Wextra
  -fPIC
)

include(GNUInstallDirs)
find_package(PkgConfig)

find_package(
  Boost
  COMPONENTS
    filesystem
    system
  REQUIRED
)

pkg_check_modules(
  SCOPE
  libunity-scopes>=0.6.7
  jsoncpp
  net-cpp>=1.1.0
  process-cpp
  REQUIRED
)

include_directories(
  "${CMAKE_SOURCE_DIR}/include"
  ${Boost_INCLUDE_DIRS}
  ${SCOPE_INCLUDE_DIRS}
)

set(SCOPE_VERSION "1.0.1")
set(SCOPE_NAME "com.ubuntu.scopes.vimeo")
set(SCOPE_INSTALL_NAME "${SCOPE_NAME}_vimeo")
set(SCOPE_ACCOUNTS_NAME "${SCOPE_NAME}_accounts")
set(SYMBOL_MAP "${CMAKE_SOURCE_DIR}/data/${SCOPE_NAME}.map")
set(GETTEXT_PACKAGE "unity-scope-vimeo")

add_definitions(-DGETTEXT_PACKAGE="${GETTEXT_PACKAGE}")
add_definitions(-DSCOPE_NAME="${SCOPE_NAME}")
add_definitions(-DSCOPE_INSTALL_NAME="${SCOPE_INSTALL_NAME}")
add_definitions(-DSCOPE_ACCOUNTS_NAME="${SCOPE_ACCOUNTS_NAME}")

set(SCOPE_INSTALL_DIR "vimeo")
add_subdirectory(click)

add_subdirectory(src)
add_subdirectory(data)

if(NOT CLICK_MODE)
  enable_testing()
  add_subdirectory(tests)
  add_custom_target(
        check
        ${CMAKE_CTEST_COMMAND} --force-new-ctest-process --output-on-failure
  )
endif()

