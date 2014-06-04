
# Put the ini file in the build directory next to the scope
# .so file so test tools can find both easily.
configure_file(
  "${CMAKE_SOURCE_DIR}/data/${SCOPE_NAME}.ini"
  "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}.ini"
)

add_library(
  ${SCOPE_NAME} SHARED
  vimeo/api/simple-oauth.cpp
  vimeo/api/client.cpp
  vimeo/api/video.cpp
  vimeo/scope/preview.cpp
  vimeo/scope/query.cpp
  vimeo/scope/scope.cpp
)

target_link_libraries(
  ${SCOPE_NAME}
  ${SCOPE_LDFLAGS}
  ${Boost_LDFLAGS}
)

install(
  TARGETS ${SCOPE_NAME}
  LIBRARY DESTINATION ${SCOPE_INSTALL_DIR}
)
