add_custom_target(eps-simulator DEPENDS eps test_eps eps_sim eps_cmd)

add_subdirectory(libeps)
add_subdirectory(eps_sim)
add_subdirectory(eps_cmd)

