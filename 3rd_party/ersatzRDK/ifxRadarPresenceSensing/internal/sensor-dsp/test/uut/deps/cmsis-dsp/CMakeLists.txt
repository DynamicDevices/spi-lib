cmake_minimum_required (VERSION 3.14)

# Define the project
project (testcmsisdsp VERSION 0.1)

# ROOT set here because I was not able to set it from the call to cmake in the build section in project.yml
set(ROOT $ENV{CMSIS_DIR})

# Define the path to CMSIS-DSP (ROOT is defined on command line when using cmake)
set(DSP ${ROOT})

# Add DSP folder to module path
#list(APPEND CMAKE_MODULE_PATH ${DSP})

########### 
#
# CMSIS DSP
#

# Load CMSIS-DSP definitions. Libraries will be built in bin_dsp
add_subdirectory(${DSP}/Source bin_dsp)
