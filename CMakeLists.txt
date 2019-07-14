##EQ10Q TopLevel CMake
cmake_minimum_required(VERSION 2.8)
PROJECT(eq10q)

##ADD_DEFINITIONS(-Wall -O3 -fPIC -finline-functions -finline-functions-called-once  -msse -mfpmath=sse -std=c99)
set(CMAKE_C_FLAGS "-Wall -O3 -fPIC -finline-functions -finline-functions-called-once  -msse -mfpmath=sse -std=c99")
#set(CMAKE_C_FLAGS "-Wall -O0 -g -fPIC -finline-functions -finline-functions-called-once  -msse -mfpmath=sse -std=c99")

set(CMAKE_CXX_FLAGS "-Wall -fPIC -std=c++11")
#set(CMAKE_CXX_FLAGS "-Wall -O0 -g -fPIC -DPIC -std=c++11") ##Debuging GUI

##ADD_DEFINITIONS(-Wall -O0 -g -fPIC -DPIC) ###USED FOR DEBUG

#Set Install prefix
set (CMAKE_INSTALL_PREFIX "/usr/local/lib/lv2" CACHE STRING "install prefix for LV2 bundle") 
#set (CMAKE_INSTALL_PREFIX  "$ENV{HOME}/.lv2") #Install prefix
set (LV2_BUNDLE "sapistaEQv2.lv2") #LV2 Bundel dir
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(gui) 
add_subdirectory(dsp)

#===========================BUILD EQ10Q ALL DSP FILES=============================================

#Build EQ1QM DSP
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq1qm.c")
add_library(eq1qm SHARED eq1qm.c)
target_link_libraries( eq1qm  lib_filter lib_vumeter m fftw3)

#Build EQ1QS DSP
set( Eq_Bands_Count 1)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq1qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq1qs.c")
add_library(eq1qs SHARED eq1qs.c)
target_link_libraries( eq1qs  lib_filter lib_vumeter m fftw3)

#Build EQ4QM DSP
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq4qm.c")
add_library(eq4qm SHARED eq4qm.c)
target_link_libraries( eq4qm  lib_filter lib_vumeter m fftw3)

#Build EQ4QS DSP
set( Eq_Bands_Count 4)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq4qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq4qs.c")
add_library(eq4qs SHARED eq4qs.c)
target_link_libraries( eq4qs  lib_filter lib_vumeter m fftw3)

#Build EQ6QM DSP
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq6qm.c")
add_library(eq6qm SHARED eq6qm.c)
target_link_libraries( eq6qm  lib_filter lib_vumeter m fftw3)

#Build EQ6QS DSP
set( Eq_Bands_Count 6)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq6qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq6qs.c")
add_library(eq6qs SHARED eq6qs.c)
target_link_libraries( eq6qs  lib_filter lib_vumeter m fftw3)

#Build EQ10QM DSP
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 1)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qm\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq10qm.c")
add_library(eq10qm SHARED eq10qm.c)
target_link_libraries( eq10qm  lib_filter lib_vumeter m fftw3)

#Build EQ10QS DSP
set( Eq_Bands_Count 10)
set( Eq_Channels_Count 2)
set( Eq_Uri  "\"http://eq10q.sourceforge.net/eq/eq10qs\"")
configure_file("${PROJECT_SOURCE_DIR}/eq.c" "${PROJECT_SOURCE_DIR}/eq10qs.c")
add_library(eq10qs SHARED eq10qs.c)
target_link_libraries( eq10qs  lib_filter lib_vumeter m fftw3)

#Build GATE DSP Mono
set( Plugin_Is_Dynamics_Compressor "PLUGIN_IS_GATE")
set( Dyn_Uri  "\"http://eq10q.sourceforge.net/gate\"")
set( Dyn_Channels_Count 1)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${PROJECT_SOURCE_DIR}/gate_mono.c")
add_library(gate SHARED gate_mono.c)
target_link_libraries( gate lib_vumeter lib_filter m)

#Build Compressor DSP Mono
set( Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR")
set( Dyn_Uri  "\"http://eq10q.sourceforge.net/compressor\"")
set( Dyn_Channels_Count 1)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${PROJECT_SOURCE_DIR}/compressor_mono.c")
add_library(compressor SHARED compressor_mono.c)
target_link_libraries( compressor lib_vumeter lib_filter m)

#Build Compressor Side-Chain DSP Mono
set( Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR_WITH_SC")
set( Dyn_Uri  "\"http://eq10q.sourceforge.net/compressor_sc\"")
set( Dyn_Channels_Count 1)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${PROJECT_SOURCE_DIR}/compressor_mono_sc.c")
add_library(compressor_sc SHARED compressor_mono_sc.c)
target_link_libraries( compressor_sc lib_vumeter lib_filter m)

#Build GATE DSP Stereo
set( Plugin_Is_Dynamics_Compressor "PLUGIN_IS_GATE")
set( Dyn_Uri  "\"http://eq10q.sourceforge.net/gate_stereo\"")
set( Dyn_Channels_Count 2)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${PROJECT_SOURCE_DIR}/gate_stereo.c")
add_library(gate_stereo SHARED gate_stereo.c)
target_link_libraries( gate_stereo lib_vumeter lib_filter m)

#Build Compressor DSP Stereo
set( Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR")
set( Dyn_Uri  "\"http://eq10q.sourceforge.net/compressor_stereo\"")
set( Dyn_Channels_Count 2)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${PROJECT_SOURCE_DIR}/compressor_stereo.c")
add_library(compressor_stereo SHARED compressor_stereo.c)
target_link_libraries( compressor_stereo lib_vumeter lib_filter m)

#Build Compressor Side-Chain DSP Mono
set( Plugin_Is_Dynamics_Compressor "PLUGIN_IS_COMPRESSOR_WITH_SC")
set( Dyn_Uri  "\"http://eq10q.sourceforge.net/compressor_stereo_sc\"")
set( Dyn_Channels_Count 2)
configure_file("${PROJECT_SOURCE_DIR}/dynamics.c" "${PROJECT_SOURCE_DIR}/compressor_stereo_sc.c")
add_library(compressor_stereo_sc SHARED compressor_stereo_sc.c)
target_link_libraries( compressor_stereo_sc lib_vumeter lib_filter m)

#Build BassUp DSP
add_library(bassup SHARED bassup.c)
target_link_libraries( bassup lib_filter m)

#Build LR2MS DSP Stereo
set( Matrix_Is_LR2MS 1)
set( Matrix_Uri  "\"http://eq10q.sourceforge.net/matrix_lr2ms\"")
configure_file("${PROJECT_SOURCE_DIR}/midside_matrix.c" "${PROJECT_SOURCE_DIR}/matrix_lr2ms.c")
add_library(matrix_lr2ms SHARED matrix_lr2ms.c)
target_link_libraries( matrix_lr2ms lib_vumeter lib_filter m)

#Build MS2LR DSP Stereo
set( Matrix_Is_LR2MS 0)
set( Matrix_Uri  "\"http://eq10q.sourceforge.net/matrix_ms2lr\"")
configure_file("${PROJECT_SOURCE_DIR}/midside_matrix.c" "${PROJECT_SOURCE_DIR}/matrix_ms2lr.c")
add_library(matrix_ms2lr SHARED matrix_ms2lr.c)
target_link_libraries( matrix_ms2lr lib_vumeter lib_filter m)

#remove the lib prefix to all generated .so files
set_target_properties(eq1qm eq1qs eq4qm eq4qs eq6qm eq6qs eq10qm eq10qs gate compressor compressor_sc gate_stereo compressor_stereo compressor_stereo_sc bassup matrix_lr2ms matrix_ms2lr PROPERTIES PREFIX "")

#CLEAN generated .c files on run 'make clean'
SET(generatedEqConfigFiles 
      ${PROJECT_SOURCE_DIR}/eq1qm.c;${PROJECT_SOURCE_DIR}/eq1qs.c
      ${PROJECT_SOURCE_DIR}/eq4qm.c;${PROJECT_SOURCE_DIR}/eq4qs.c
      ${PROJECT_SOURCE_DIR}/eq6qm.c;${PROJECT_SOURCE_DIR}/eq6qs.c
      ${PROJECT_SOURCE_DIR}/eq10qm.c;${PROJECT_SOURCE_DIR}/eq10qs.c
      ${PROJECT_SOURCE_DIR}/gate_mono.c;${PROJECT_SOURCE_DIR}/compressor_mono.c;${PROJECT_SOURCE_DIR}/compressor_mono_sc.c
      ${PROJECT_SOURCE_DIR}/gate_stereo.c;${PROJECT_SOURCE_DIR}/compressor_stereo.c;${PROJECT_SOURCE_DIR}/compressor_stereo_sc.c
      ${PROJECT_SOURCE_DIR}/matrix_lr2ms.c;${PROJECT_SOURCE_DIR}/matrix_ms2lr.c
      )
SET_DIRECTORY_PROPERTIES(PROPERTIES  ADDITIONAL_MAKE_CLEAN_FILES "${generatedEqConfigFiles}")


##===========================THE LV2 INSTALLER================================================
#INSTALL( DIRECTORY "${PROJECT_SOURCE_DIR}/${LV2_BUNDLE}" DESTINATION "${LV2_BUNDLE}")
INSTALL( FILES "manifest.ttl" "eq1qm.ttl" "eq1qs.ttl" "eq4qm.ttl" "eq4qs.ttl" "eq6qm.ttl" "eq6qs.ttl" "eq10qm.ttl" "eq10qs.ttl" "gate.ttl" "compressor.ttl" "compressor_sc.ttl" "gate_stereo.ttl" "compressor_stereo.ttl" "compressor_stereo_sc.ttl" "bassup.ttl" "lr2ms.ttl" "ms2lr.ttl" DESTINATION "${LV2_BUNDLE}")
INSTALL( TARGETS eq1qm eq1qs eq4qm eq4qs eq6qm eq6qs eq10qm eq10qs gate compressor compressor_sc gate_stereo compressor_stereo compressor_stereo_sc bassup matrix_lr2ms matrix_ms2lr DESTINATION "${LV2_BUNDLE}")

INCLUDE(CPack)
# cpack -G TGZ
 
