add_library(artery-simu5g INTERFACE) 
target_link_libraries(artery-simu5g INTERFACE artery simu5g)

add_opp_run(tunnel  DEPENDENCY artery-simu5g WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/highway-tunnel)
add_opp_run(platooning  DEPENDENCY artery-simu5g WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/platooning)
add_opp_run(roundabout DEPENDENCY artery-simu5g WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/roundabout)
add_opp_run(intas DEPENDENCY artery-simu5g WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/intas/scenario)



