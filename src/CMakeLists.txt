set(HDRS
  network_access.h
  basic_request.h

  stream_request_builder.h
  copy_request_builder.h

  http_stream.h
  http_stream_error_category.h

  response.h
  response_reader.h

  utils.h
  constants.h
)

add_custom_target(Headers SOURCES ${HDRS})
