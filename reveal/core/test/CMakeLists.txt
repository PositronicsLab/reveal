add_executable( pacer-planner mockup_pacer_planner.cpp )
add_dependencies( pacer-planner
  reveal_core 
)
target_link_libraries( pacer-planner reveal_core )

add_executable( pacer-simulator mockup_pacer_simulator.cpp )
add_dependencies( pacer-simulator
  reveal_core
)
target_link_libraries( pacer-simulator reveal_core )

