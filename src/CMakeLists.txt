add_definitions(-DASIO_STANDALONE=1)

set(HDRS
  network_access.h
  basic_request.h
)

set(SRCS
  network_access.cc
  basic_request.cc
)

add_library(restpp ${HDRS} ${SRCS})
target_link_libraries(restpp ${OPENSSL_LIBRARIES})