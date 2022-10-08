add_executable(TS_test)
target_link_libraries(TS_test PRIVATE TS)

target_include_directories(TS_test PRIVATE include/)
target_sources(TS_test PRIVATE include/ts_test.h
                               include/connector.h
                               src/ts_test.cxx)

