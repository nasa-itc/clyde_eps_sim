project(libeps CXX)

# nos3
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)

# libeps
include_directories(inc ${ITC_Common_INCLUDE_DIRS})

file(GLOB libeps_h inc/*.hpp)
set(libeps_src src/util.cpp
               src/status.cpp
               src/adc.cpp
               src/bus.cpp
               src/bcr.cpp
               src/pcm.cpp
               src/pdm.cpp
               src/eps.cpp)
set(libeps_libs ${ITC_Common_itc_logger_LIBRARY})

add_library(eps SHARED ${libeps_h} ${libeps_src})
target_link_libraries(eps ${libeps_libs})
install(TARGETS eps LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

# test

#file(GLOB test_eps_h test/*.hpp)
#set(test_eps_src test/version_test.cpp
#                 test/status_test.cpp
#                 test/bus_test.cpp
#                 test/command_test.cpp
#                 test/main.cpp)
#set(test_eps_libs ${GTEST_BOTH_LIBRARIES}
#                  ${libeps_libs}
#                  eps)

#add_executable(test_eps ${libeps_h} ${libeps_src} ${test_eps_h} ${test_eps_src})
#set_target_properties(test_eps PROPERTIES COMPILE_FLAGS "" LINK_FLAGS "")
#target_link_libraries(test_eps ${test_eps_libs})

