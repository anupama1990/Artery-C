add_library(artery-lte INTERFACE) 
target_link_libraries(artery-lte INTERFACE artery lte)

add_opp_run(tunnel  DEPENDENCY artery-lte WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/highway-tunnel)
add_opp_run(platooning  DEPENDENCY artery-lte WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/platooning)
add_opp_run(intas DEPENDENCY artery-lte WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/InTAS/scenario)



