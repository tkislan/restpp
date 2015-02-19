enable_testing()

include_directories(${CMAKE_SOURCE_DIR}/3rd/gtest-1.7.0/include)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(test_network_access test_network_access.cc)
target_link_libraries(test_network_access gtest_main ${OPENSSL_LIBRARIES})
add_test(TestNetworkAccess test_network_access)
