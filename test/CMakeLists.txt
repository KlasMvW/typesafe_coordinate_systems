add_executable(TS_test)
target_link_libraries(TS_test PRIVATE TS)

target_include_directories(TS_test PRIVATE include/)
target_sources(TS_test PRIVATE include/ts_test.h
                               src/ts_test.cxx)

target_compile_options(TS_test PRIVATE $<$<CXX_COMPILER_ID:MSVC>: $<$<CONFIG:Release>:/O2> /W4>)

add_test(tu_test TS_test)
