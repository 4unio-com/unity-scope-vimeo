
add_executable(
  ${SCOPE_NAME}-functional-tests
  vimeo-functional-tests.cpp
)

target_link_libraries(
  ${SCOPE_NAME}-functional-tests
  ${GTEST_BOTH_LIBRARIES}
  ${GMOCK_LIBRARIES}
)

add_test(
  ${SCOPE_NAME}-functional-tests
  ${SCOPE_NAME}-functional-tests
)
