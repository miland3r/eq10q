###DSP CMake

##Build Mono Filter lib
add_library(lib_filter STATIC filter.c)

##Build the vumeter
add_library( lib_vumeter  STATIC vu.c)
