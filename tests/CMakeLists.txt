enable_testing()

set(MOCK_HDRS
  mocks/mock_request_builder.h
  mocks/mock_basic_request.h
  mocks/mock_server.h)

add_custom_target(MockHeaders SOURCES ${MOCK_HDRS})

add_custom_target(check)

add_custom_target(run-tests COMMAND ${CMAKE_CTEST_COMMAND} -V)

include_directories(${CMAKE_SOURCE_DIR}/3rd/gmock-1.7.0/include)
include_directories(${CMAKE_SOURCE_DIR}/3rd/gmock-1.7.0/gtest/include)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_definitions(-DGTEST_HAS_PTHREAD=1)

macro(restpp_add_test TEST_NAME EXECUTABLE)
  add_executable(${EXECUTABLE} ${EXECUTABLE}.cc)
  target_link_libraries(${EXECUTABLE} gmock_main ${OPENSSL_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
  if(WIN32)
    target_link_libraries(${EXECUTABLE} wsock32 ws2_32)
  endif()

  add_test(${TEST_NAME} ${EXECUTABLE})
  add_dependencies(run-tests ${EXECUTABLE})
endmacro()

restpp_add_test(TestNetworkAccess test_network_access)
restpp_add_test(TestRequestBuilder test_request_builder)
restpp_add_test(TestStreamRequestBuilder test_stream_request_builder)
restpp_add_test(TestCopyRequestBuilder test_copy_request_builder)
restpp_add_test(TestHttpStream test_http_stream)

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

