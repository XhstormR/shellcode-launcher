set(APP_NAME main)

aux_source_directory(. SOURCE)

list(APPEND SOURCE ./resources/resource.rc)

add_library(${APP_NAME} SHARED ${SOURCE})

install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
install(FILES ${PROJECT_SOURCE_DIR}/assets/start.vbs DESTINATION bin)
