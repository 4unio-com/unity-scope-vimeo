
include(FindGMock)

include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${GMOCK_INCLUDE_DIRS}
)

add_subdirectory(functional)
add_subdirectory(unit)
