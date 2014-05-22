
# Put the ini file in the build directory next to the scope
# .so file so test tools can find both easily.
configure_file(
  "${CMAKE_SOURCE_DIR}/data/${SCOPE_NAME}.ini"
  "${CMAKE_BINARY_DIR}/${SCOPE_NAME}.ini"
)

add_library(
  ${SCOPE_NAME} SHARED
  vimeo-preview.cpp
  vimeo-query.cpp
  vimeo-scope.cpp
)

target_link_libraries(
  ${SCOPE_NAME}
  ${SCOPE_LDFLAGS}
)

set_property(
  TARGET ${SCOPE_NAME}
  PROPERTY COMPILE_FLAGS ${SCOPE_CFLAGS}
)

install(
  TARGETS ${SCOPE_NAME}
  LIBRARY DESTINATION ${SCOPE_INSTALL_DIR}
)
