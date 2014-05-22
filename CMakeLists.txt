project(unity-scope-vimeo CXX)
cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

add_definitions(
  -std=c++11
  -pedantic
  -Wall
  -Wextra
)

include(GNUInstallDirs)
include(FindPkgConfig)

pkg_check_modules(
  SCOPE
  libunity-scopes>=0.4.0
  REQUIRED
)

include_directories(
  "${CMAKE_SOURCE_DIR}/include"
  ${SCOPE_INCLUDE_DIRS}
)

set(SCOPE_NAME unity-scope-vimeo)
_pkgconfig_invoke(libunity-scopes "SCOPE" "DIR" "" --variable=scopesdir)
set(SCOPE_INSTALL_DIR "${SCOPE_DIR}/${SCOPE_NAME}")
set(SYMBOL_MAP "${CMAKE_SOURCE_DIR}/data/${SCOPE_NAME}.map")

add_subdirectory(src)
add_subdirectory(data)
enable_testing()
add_subdirectory(tests)
