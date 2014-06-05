
# Put the ini file in the build directory next to the scope
# .so file so test tools can find both easily.
configure_file(
  "${CMAKE_SOURCE_DIR}/data/${SCOPE_NAME}.ini"
  "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}.ini"
)

add_library(
  ${SCOPE_NAME} SHARED
  vimeo/api/simple-oauth.cpp
  vimeo/api/channel.cpp
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

set_target_properties(
  ${SCOPE_NAME}
  PROPERTIES
    LINK_FLAGS "-Wl,--no-undefined -Wl,--version-script,${SYMBOL_MAP}"
    LINK_DEPENDS "${SYMBOL_MAP}"
    VERSION 1.0.0
    SOVERSION 1
)

install(
  TARGETS ${SCOPE_NAME}
  LIBRARY DESTINATION ${SCOPE_INSTALL_DIR}
)
