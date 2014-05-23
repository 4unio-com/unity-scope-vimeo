add_executable(
  ${SCOPE_NAME}-unit-tests
  vimeo/scope/test-vimeo-scope.cpp
)

target_link_libraries(
  ${SCOPE_NAME}-unit-tests
  ${SCOPE_NAME}
  ${GTEST_BOTH_LIBRARIES}
  ${GMOCK_LIBRARIES}
)

add_test(
  ${SCOPE_NAME}-unit-tests
  ${SCOPE_NAME}-unit-tests
)
