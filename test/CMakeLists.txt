add_executable(TS_test)
target_link_libraries(TS_test PRIVATE TS)

target_include_directories(TS_test PRIVATE include/)
target_sources(TS_test PRIVATE include/cs_test.h
                               src/cs_test.cxx)
