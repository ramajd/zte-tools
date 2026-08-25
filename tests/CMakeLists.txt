find_package(Qt6 REQUIRED COMPONENTS Test Core Network)

qt_add_executable(tst_codec tst_codec.cpp)
target_link_libraries(tst_codec PRIVATE zte-core Qt6::Test)
add_test(NAME tst_codec COMMAND tst_codec)

qt_add_executable(tst_models tst_models.cpp)
target_link_libraries(tst_models PRIVATE zte-core Qt6::Test)
add_test(NAME tst_models COMMAND tst_models)

qt_add_executable(tst_client tst_client.cpp)
target_include_directories(tst_client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tst_client PRIVATE zte-core Qt6::Test Qt6::Network)
add_test(NAME tst_client COMMAND tst_client)
