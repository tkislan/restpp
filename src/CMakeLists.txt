add_definitions(-DASIO_STANDALONE=1)

set(HDRS
  network_access.h
  basic_request.h

  stream_request_builder.h
  copy_request_builder.h
)

add_custom_target(Headers SOURCES ${HDRS})
