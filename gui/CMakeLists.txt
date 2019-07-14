##GUI CMake

#PROJECT(gui)

#GCC options example TODO remove -g option in release version
# TODO  segur k no e de fer un remove_definitions()???
##ADD_DEFINITIONS(-Wall -fPIC -DPIC -g)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-2.4)

add_subdirectory(widgets) 

link_directories(${GTKMM_LIBRARY_DIRS})

include_directories(${GTKMM_INCLUDE_DIRS})

#Tests GUI
set( Eq_Gui_Test_Path "\"${PROJECT_SOURCE_DIR}/gui\"" )
configure_file("${PROJECT_SOURCE_DIR}/gui/guitester.cpp" "${PROJECT_SOURCE_DIR}/gui/guitester_cfg_.cpp")
ADD_EXECUTABLE(eq10q_gui_tester  main.cpp guitester_cfg_.cpp)
target_link_libraries( eq10q_gui_tester  eqwin dynamicwin bassupwin midsidewin ${GTKMM_LIBRARIES})

#===========================BUILD EQ10Q ALL GUI FILES=============================================

#Build EQ10Q GUI
add_library(eq10q_gui SHARED eq10q_ui.cpp)
target_link_libraries( eq10q_gui  eqwin ${GTKMM_LIBRARIES})

#Build DYNAMICS GUI
add_library(dynamics_gui SHARED dyn_ui.cpp)
target_link_libraries( dynamics_gui  dynamicwin ${GTKMM_LIBRARIES})

#Build BassUp Gui_Dyn_Uri
add_library(bassup_gui SHARED bassup_ui.cpp)
target_link_libraries( bassup_gui  bassupwin ${GTKMM_LIBRARIES})

#Build MS Matrix UI
add_library(matrix_ms_gui SHARED midside_ui.cpp)
target_link_libraries( matrix_ms_gui  midsidewin ${GTKMM_LIBRARIES})

#remove the lib prefix to all generated .so files
set_target_properties(eq10q_gui dynamics_gui bassup_gui matrix_ms_gui PROPERTIES PREFIX "")

##=================================INSTALLER=======================================================================
INSTALL( TARGETS eq10q_gui dynamics_gui bassup_gui matrix_ms_gui DESTINATION "${LV2_BUNDLE}/gui")
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/gui/knobs" DESTINATION "${LV2_BUNDLE}/gui")
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/gui/combopix" DESTINATION "${LV2_BUNDLE}/gui")
INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/gui/icons" DESTINATION "${LV2_BUNDLE}/gui")

