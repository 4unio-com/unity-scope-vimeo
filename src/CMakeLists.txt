
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

install(
  TARGETS ${SCOPE_NAME}
  LIBRARY DESTINATION ${SCOPE_INSTALL_DIR}
)
