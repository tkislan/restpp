set(HDRS
  network_access.h
  request.h
  request_builder.h

  http_stream.h
  http_stream_error_category.h
  http_connector.h
  http_watcher.h

  response.h

  utils.h
  constants.h
)

add_custom_target(Headers SOURCES ${HDRS})
