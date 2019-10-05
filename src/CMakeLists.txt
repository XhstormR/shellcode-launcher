set(APP_NAME main)

aux_source_directory(. SOURCE)

list(APPEND SOURCE ./resources/resource.rc)

add_executable(${APP_NAME} ${SOURCE})

install(TARGETS ${APP_NAME} RUNTIME DESTINATION bin)
