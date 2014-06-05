
install(
  FILES "${SCOPE_NAME}.ini"
  DESTINATION ${SCOPE_INSTALL_DIR}
)

install(
  DIRECTORY "accounts"
  DESTINATION "share"
)
