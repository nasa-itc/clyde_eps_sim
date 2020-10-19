project(eps_sim CXX)

# boost
find_package(Boost REQUIRED QUIET COMPONENTS program_options system)

# nos
find_package(ITC_Common REQUIRED QUIET COMPONENTS itc_logger)
find_package(NOSENGINE REQUIRED QUIET COMPONENTS common transport client i2c)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_RPATH}:$ORIGIN/../lib") # Pick up .so in install directory

# fltk (ui toolkit)
find_package(FLTK QUIET) # issues with this (only static libs, etc.) so only using for fluid
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
#fltk_wrap_ui(eps_sim src/eps_win.fl)
#set_source_files_properties(${eps_sim_FLTK_UI_SRCS} PROPERTIES COMPILE_FLAGS "-Wno-old-style-cast")

# eps sim
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
include_directories(inc
                    ${libeps_SOURCE_DIR}/inc
                    ${ITC_Common_INCLUDE_DIRS}
                    ${NOSENGINE_INCLUDE_DIRS})

file(GLOB eps_sim_h inc/*.hpp)

set(eps_sim_src src/config.cpp
                src/eps_win.cpp
                src/eps_sim.cpp
                src/main.cpp)

set(eps_sim_libs ${FLTK_LIBRARIES}
                 ${Boost_LIBRARIES}
                 ${ITC_Common_itc_logger_LIBRARY}
                 ${NOSENGINE_LIBRARIES}
                 eps)

add_executable(nos3-eps-simulator ${eps_sim_h} ${eps_sim_src})
target_link_libraries(nos3-eps-simulator ${eps_sim_libs})

install(TARGETS nos3-eps-simulator RUNTIME DESTINATION bin)
install(FILES cfg/eps.json DESTINATION bin)

