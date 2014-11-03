
function(configure_scope_files)
  foreach(_file ${ARGV})
    configure_file(
      "${_file}.in"
      "${CMAKE_CURRENT_BINARY_DIR}/${_file}"
      @ONLY
    )
  endforeach()
endfunction()

configure_scope_files(
  manifest.json
  vimeo.provider
  vimeo.service
  vimeo.application
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/manifest.json"
    "apparmor.json"
    "${CMAKE_CURRENT_BINARY_DIR}/vimeo.provider"
    "${CMAKE_CURRENT_BINARY_DIR}/vimeo.service"
    "${CMAKE_CURRENT_BINARY_DIR}/vimeo.application"
  DESTINATION ${CMAKE_INSTALL_PREFIX}
)
