enable_testing()

set(MOCK_HDRS
  mocks/mock_request_builder.h)

add_custom_target(MockHeaders SOURCES ${MOCK_HDRS})

add_custom_target(check)

add_custom_target(run-tests COMMAND ${CMAKE_CTEST_COMMAND} -V)

include_directories(${CMAKE_SOURCE_DIR}/3rd/gmock-1.7.0/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd/gmock-1.7.0/gtest/include)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_definitions(-DGTEST_HAS_PTHREAD=1)

add_executable(test_network_access test_network_access.cc)
target_link_libraries(test_network_access gmock_main ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(TestNetworkAccess test_network_access)
add_dependencies(run-tests test_network_access)

add_executable(test_request_builder test_request_builder.cc)
target_link_libraries(test_request_builder gmock_main ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(TestRequestBuilder test_request_builder)
add_dependencies(run-tests test_request_builder)

add_executable(test_stream_request_builder test_stream_request_builder.cc)
target_link_libraries(test_stream_request_builder gmock_main ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(TestStreamRequestBuilder test_stream_request_builder)
add_dependencies(run-tests test_stream_request_builder)

add_executable(test_copy_request_builder test_copy_request_builder.cc)
target_link_libraries(test_copy_request_builder gmock_main ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
add_test(TestCopyRequestBuilder test_copy_request_builder)
add_dependencies(run-tests test_copy_request_builder)

if(WIN32)
  add_custom_target(start-test-server
    COMMAND start /min iojs.exe test_server/server.js
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Starting io.js test server")

  add_custom_target(stop-test-server
    COMMAND taskkill /im iojs.exe
    COMMENT "Stopping io.js test server")
endif()

add_dependencies(check stop-test-server)
add_dependencies(stop-test-server run-tests)
add_dependencies(run-tests start-test-server)

if(WIN32)
  target_link_libraries(test_network_access wsock32 ws2_32)
  target_link_libraries(test_request_builder wsock32 ws2_32)
  target_link_libraries(test_stream_request_builder wsock32 ws2_32)
endif()
