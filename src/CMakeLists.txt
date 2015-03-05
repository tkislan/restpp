set(HDRS
  network_access.h
  basic_request.h

  stream_request_builder.h
  copy_request_builder.h

  response.h
  response_reader.h
)

add_custom_target(Headers SOURCES ${HDRS})
