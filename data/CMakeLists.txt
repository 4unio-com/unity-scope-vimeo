
configure_file(
  "${SCOPE_NAME}.ini.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}.ini"
  @ONLY
)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/${SCOPE_NAME}.ini"
  DESTINATION ${SCOPE_INSTALL_DIR}
)

install(
  DIRECTORY "accounts"
  DESTINATION "share"
)

install(
  FILES "icon.png"
  DESTINATION "${SCOPES_DATADIR}/${SCOPE_NAME}"
)

