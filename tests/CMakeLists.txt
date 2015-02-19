enable_testing()

set(MOCK_HDRS
  mocks/mock_request_builder.h)

add_custom_target(MockHeaders SOURCES ${MOCK_HDRS})

include_directories(${CMAKE_SOURCE_DIR}/3rd/gmock-1.7.0/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd/gmock-1.7.0/gtest/include)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test_network_access test_network_access.cc)
target_link_libraries(test_network_access gmock_main ${OPENSSL_LIBRARIES})
add_test(TestNetworkAccess test_network_access)

add_executable(test_request_builder test_request_builder.cc)
target_link_libraries(test_request_builder gmock_main ${OPENSSL_LIBRARIES})
add_test(TestRequestBuilder test_request_builder)
