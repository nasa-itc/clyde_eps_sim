project(eps_cmd CXX)

# boost
find_package(Boost REQUIRED QUIET COMPONENTS program_options system)

# nos
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client server i2c)

# fltk (ui toolkit)
find_package(FLTK REQUIRED QUIET) # issues with this (only static libs, etc.) so only using for fluid
if(NOT FLTK_FOUND)
    message(WARNING "fltk libraries not found. skipping eps_sim build.")
    return()
endif()
find_program(FLTK_CONFIG NAMES fltk-config)
execute_process(COMMAND ${FLTK_CONFIG} --version
                OUTPUT_VARIABLE FLTK_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
#message(STATUS "FLTK version: ${FLTK_VERSION}")
execute_process(COMMAND ${FLTK_CONFIG} --ldflags
                OUTPUT_VARIABLE FLTK_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)

# eps cmd
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(inc ${libeps_SOURCE_DIR}/inc ${ITC_Common_INCLUDE_DIRS} ${NOSENGINE_INCLUDE_DIRS})
file(GLOB eps_cmd_h inc/*.hpp)
set(eps_cmd_src src/cmd_win.cpp
                src/eps_cmd.cpp)
set(eps_cmd_libs ${FLTK_LIBRARIES}
                 ${Boost_LIBRARIES}
                 ${NOSENGINE_LIBRARIES}
                 eps)
add_executable(eps_cmd ${eps_cmd_h} ${eps_cmd_src})
target_link_libraries(eps_cmd ${eps_cmd_libs})

#install(TARGETS eps_cmd RUNTIME DESTINATION bin)

