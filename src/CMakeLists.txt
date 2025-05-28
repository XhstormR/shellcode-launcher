set(APP_NAME main)

aux_source_directory(. SOURCE)

list(APPEND SOURCE ./resource.rc)

list(APPEND SOURCE ./proxy.def)

add_library(${APP_NAME} SHARED ${SOURCE})

install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
install(PROGRAMS ${PROJECT_SOURCE_DIR}/assets/start.vbs TYPE BIN)
