################################################################################
# \file Makefile
#
# \brief
# Common test application makefile
#
# This Makefile can be used in multiple ways:
#
# 1) for FUT/chaitea: included by the test-specific Makefile:
# test/fut/<fut_path>/Makefile contains "include ../../Makefile"
#
# make -C test/fut/<fut_path> build
#
# 2) for SUT/coverity: used as top-level Makefile:
# (for simplicity, there is no SUT-specific Makefile)
#
# make -C test TEST=sut/<sut_path> build
#
# The path evaluation logic is different in SUT and FUT mode
# as documented below in the inline comments
#
################################################################################
# \copyright
# Copyright 2018-2021 Cypress Semiconductor Corporation
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################


################################################################################
# Basic Configuration
################################################################################

# Name of toolchain to use. Options include:
#
# GCC_ARM -- GCC provided with ModusToolbox IDE
# ARM     -- ARM Compiler (must be installed separately)
# IAR     -- IAR Compiler (must be installed separately)
#
# See also: CY_COMPILER_PATH below
TOOLCHAIN?=GCC_ARM

LIBNAME=xensiv-radar-presence

# Default build configuration. Options include:
#
# Debug -- build with minimal optimizations, focus on debugging.
# Release -- build with full optimizations
# Custom -- build with custom configuration, set the optimization flag in CFLAGS
# 
# If CONFIG is manually edited, ensure to update or regenerate launch configurations 
# for your IDE.
CONFIG?=Debug

# If set to "true" or "1", display full command-lines when building.
VERBOSE?=


################################################################################
# Advanced Configuration
################################################################################

# Enable optional code that is ordinarily disabled by default.
#
# Available components depend on the specific targeted hardware and firmware
# in use. In general, if you have
#
#    COMPONENTS=foo bar
#
# ... then code in directories named COMPONENT_foo and COMPONENT_bar will be
# added to the build
#
COMPONENTS?=

# Like COMPONENTS, but disable optional code that was enabled by default.
DISABLE_COMPONENTS?=

# By default the build system automatically looks in the Makefile's directory
# tree for source code and builds it. The SOURCES variable can be used to
# manually add source code to the build process from a location not searched
# by default, or otherwise not found by the build system.
SOURCES?=

# Like SOURCES, but for include directories. Value should be paths to
# directories (without a leading -I).
INCLUDES?=

# Add additional defines to the build process (without a leading -D).
DEFINES?=

# Select softfp or hardfp floating point. Default is softfp.
VFP_SELECT?=

# Additional / custom C compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CFLAGS?=

# Additional / custom C++ compiler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
CXXFLAGS?=

# Additional / custom assembler flags.
#
# NOTE: Includes and defines should use the INCLUDES and DEFINES variable
# above.
ASFLAGS?=

# Additional / custom linker flags.
LDFLAGS?=

# Additional / custom libraries to link in to the application.
LDLIBS?=

# Path to the linker script to use (if empty, use the default linker script).
LINKER_SCRIPT?=

# Custom pre-build commands to run.
PREBUILD?=

# Custom post-build commands to run.
POSTBUILD?=


################################################################################
# Paths
################################################################################

# Path to the top-level test directory ($(ROOT_PATH)/test)
# (evaluates to "..", "../.." or "../../..")
BUILD_ROOT_PATH:=$(dir $(lastword $(MAKEFILE_LIST)))
#$(info TEST_ROOT_PATH=$(TEST_ROOT_PATH))

# Relative path to the asset root directory (TEST_ROOT_PATH parent)
# Note: strip trailing / from TEST_ROOT_PATH
ROOT_PATH=../..

# Use separate build directory for each TOOLCHAIN/TARGET/CONFIG:
# test/fut/$(APPNAME)/build/$(TOOLCHAIN)/$(APPNAME)/$(TARGET)/$(CONFIG)
CY_BUILD_LOCATION=$(ROOT_PATH)/output/libs/$(VFP_SELECT)/$(TOOLCHAIN)
$(info CY_BUILD_LOCATION=$(CY_BUILD_LOCATION))

# Uncomment to debug the path related issues
#$(info LIBNAME=$(LIBNAME))
#$(info CY_APP_PATH=$(CY_APP_PATH))
#$(info ROOT_PATH=$(ROOT_PATH))
#$(info CY_BUILD_LOCATION=$(CY_BUILD_LOCATION))

# Relative path to the shared repo location.
#
# All .mtb files have the format, <URI>#<COMMIT>#<LOCATION>. If the <LOCATION> field 
# begins with $$ASSET_REPO$$, then the repo is deposited in the path specified by 
# the CY_GETLIBS_SHARED_PATH variable. The default location is one directory level 
# above the current app directory.
# This is used with CY_GETLIBS_SHARED_NAME variable, which specifies the directory name.
CY_GETLIBS_SHARED_PATH=$(ROOT_PATH)/output

# Directory name of the shared repo location.
#
CY_GETLIBS_SHARED_NAME=libs

# List of shared libraries (downloaded to output/libs by fetch_asset functions)
SEARCH_core-lib=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/core-lib
SEARCH_core-make=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/core-make
SEARCH_recipe-make-cat1a=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/recipe-make-cat1a
SEARCH_recipe-make-cat1b=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/recipe-make-cat1b
SEARCH_mtb-pdl-cat1=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/mtb-pdl-cat1
SEARCH_mtb-hal-cat1=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/mtb-hal-cat1
SEARCH_recipe-make-cat2=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/recipe-make-cat2
SEARCH_mtb-pdl-cat2=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/mtb-pdl-cat2
SEARCH_mtb-hal-cat2=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/mtb-hal-cat2
SEARCH_recipe-make-cat3=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/recipe-make-cat3
SEARCH_mtb-xmclib-cat3=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/mtb-xmclib-cat3
SEARCH_recipe-make-cat4=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/recipe-make-cat4
SEARCH_mtb-hal-cat4=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/mtb-hal-cat4
SEARCH_psoc6cm0p=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/psoc6cm0p
SEARCH_retarget-io=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/retarget-io
SEARCH_freertos=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/freertos
SEARCH_abstraction-rtos=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/abstraction-rtos
SEARCH_clib-support=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/clib-support
SEARCH_cmsis-dsp=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/cmsis-dsp/CMSIS_5-5.9.0/CMSIS/DSP
SEARCH_sensor-dsp=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/sensor-dsp

# Target board/hardware (BSP).
# Defined either by the test-specific Makefile, or through the command-line
ifeq (,$(TARGET))
$(error TARGET variable is not defined)
endif

# Define target BSP directory
SEARCH_TARGET_$(TARGET)=$(CY_GETLIBS_SHARED_PATH)/bsps/TARGET_$(TARGET)

# Check the BSP makefile exists
ifeq (,$(wildcard $(SEARCH_TARGET_$(TARGET))/$(TARGET).mk))
$(error BSP directory $(SEARCH_TARGET_$(TARGET)) doesn't exist)
endif

# Include BSP makefile to understand the target architecture
include $(SEARCH_TARGET_$(TARGET))/$(TARGET).mk

# Shared libraries added to build
CY_BASELIB_CORE_PATH=$(SEARCH_core-make)
INCLUDES+=$(SEARCH_cmsis-dsp)/Include
INCLUDES+=$(SEARCH_sensor-dsp)/include

# Architecture specific libraries added to build
ifneq ($(findstring CAT1A,$(COMPONENTS)),) # PSoC 6
  CY_BASELIB_PATH=$(SEARCH_recipe-make-cat1a)
  INCLUDES+=$(SEARCH_mtb-pdl-cat1)/cmsis/include
  COMPONENTS+=PSOC6HAL
endif

# ASSET_LIB_DIR variable is defined only for GitLab CI pipeline
ifeq ($(ASSET_LIB_DIR),)
  # Developer build: add asset sources from git
  # This allows developer to skip execution of build_assets.sh
  SEARCH_asset=$(ROOT_PATH)
  SOURCES+=$(wildcard $(SEARCH_asset)/*.c)
  INCLUDES+=$(SEARCH_asset)
else
  # CI pipeline build: auto-discover asset sources in output dir
  # The location matches ASSET_LIB_DIR exported by extract_artifacts
  SEARCH_asset=$(CY_GETLIBS_SHARED_PATH)/xensiv-radar-presence
  ifeq (,$(wildcard $(SEARCH_asset)))
  $(error Directory $(SEARCH_asset) doesn't exist, run ./build/build_assets.sh or extract_artifacts)
  endif
  SEARCH+=$(SEARCH_asset)
endif

CY_IGNORE+=$(CY_GETLIBS_SHARED_PATH)

# Ensure the default target "top" defined before custom chaitea targets
top:

# Uncomment to analyze list of included libraries
#$(info SEARCH=$(SEARCH))

# Set the Python path
# $(PYTHON) is set by setenv.sh script
ifneq ($(PYTHON),)
CY_PYTHON_PATH?=$(shell which $(PYTHON))
endif

# The default GCC_DIR/ARMCC_DIR/IAR_DIR values are defined in build/setenv.sh
# For CI build, the environment variables be overwritten in .gitlab-ci.yml:
#
# test_sut:
#   variables:
#     GCC_DIR:   "C:/Program Files (x86)/GNU Tools ARM Embedded/9 2019-q4-major"
#     ARMCC_DIR: "C:/Keil_v5/ARM/ARMCLANG"
#     IAR_DIR:   "C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.2/arm"
#
# For developer build, the variables can be overwritten in the ~/.bashrc,
# or set interactively in the bash terminal window:
#
# export GCC_DIR="C:/Program Files (x86)/GNU Tools ARM Embedded/9 2019-q4-major"
# export ARMCC_DIR="C:/Keil_v5/ARM/ARMCLANG"
# export IAR_DIR="C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.2/arm"
#
OS?=$(shell uname -s)
ifeq ($(OS),Windows_NT)
  CY_DEP_DIR?=$(subst \,/,$(LOCALAPPDATA))/cydep
  GCC_DIR   ?=$(CY_DEP_DIR)/gcc-arm-none-eabi-10.3-2021.07-win32
  IAR_DIR   ?=C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.4/arm
  ARMCC_DIR ?=C:/Keil_v5/ARM/ARMCLANG
else ifeq ($(OS),Darwin)
  CY_DEP_DIR?=$(HOME)/.cache/cydep
  GCC_DIR   ?=$(CY_DEP_DIR)/gcc-arm-none-eabi-10.3-2021.07-mac
  ACLANG_LIBS_DIR?=$(CY_DEP_DIR)/aclang-libs-0.3
else ifeq ($(OS),Linux)
  CY_DEP_DIR?=$(HOME)/.cache/cydep
  GCC_DIR   ?=$(CY_DEP_DIR)/gcc-arm-none-eabi-10.3-2021.07-x86_64-linux
else
  $(error unsupported OS)
endif

# Override core-make defaults with the values defined above
CY_COMPILER_GCC_ARM_DIR?="$(GCC_DIR)"
CY_COMPILER_ARM_DIR?="$(ARMCC_DIR)"
CY_COMPILER_IAR_DIR?="$(IAR_DIR)"
CY_COMPILER_A_Clang_DIR="$(ACLANG_LIBS_DIR)"

# Set compiler/linker flags to catch important warnings
ifeq ($(TOOLCHAIN),GCC_ARM)
  CFLAGS+=-Wall -Wextra
  CXXFLAGS+=-Wall -Wextra

  # Enable stack usage analysis
  CFLAGS+=-fstack-usage
  CXXFLAGS+=-fstack-usage

else ifeq ($(TOOLCHAIN),ARM)
  CFLAGS+=-Wall -Wextra
  CXXFLAGS+=-Wall -Wextra

  # Enable stack frame debugging
  CFLAGS+=-fno-omit-frame-pointer
  CXXFLAGS+=-fno-omit-frame-pointer

  # Suppress ARM Linker warnings:
  # L6314W: No section matches pattern
  # L6329W: Pattern *(.noinit) only matches removed unused sections.
  # Treat all other linker warnings as errors
  LDFLAGS+=--diag_suppress=L6314W,L6329W --diag_error=warning

else ifeq ($(TOOLCHAIN),A_Clang)
  CFLAGS+=-Wall -Wextra
  CXXFLAGS+=-Wall -Wextra
  ifneq ($(VERBOSE),)
    $(info ACLANG_LIBS_DIR=$(ACLANG_LIBS_DIR))
  endif
  CFLAGS+=-Werror

  # Discover C runtime extracted by copy_aclang function
  SEARCH+=$(CY_GETLIBS_SHARED_PATH)/$(CY_GETLIBS_SHARED_NAME)/TOOLCHAIN_A_Clang
endif

# Treat all warnings as errors (for SUT build)
ifneq ($(WERROR),)
  ifeq ($(TOOLCHAIN),IAR)
    CFLAGS+=--warnings_are_errors
    CXXFLAGS+=--warnings_are_errors
  else # GCC/ARM/A_Clang
    CFLAGS+=-Werror
    CXXFLAGS+=-Werror
  endif

  ifeq ($(TOOLCHAIN),ARM)
    # Suppress ARM Linker warnings:
    # L6314W: No section matches pattern
    # L6329W: Pattern *(.noinit) only matches removed unused sections.
    # Treat all other linker warnings as errors
    LDFLAGS+=--diag_suppress=L6314W,L6329W --diag_error=warning
  endif
endif

# Ignore warnings reported for pragma #warning
ifneq ($(CY_IGNORE_PRAGMA_WARNINGS),)
  ifeq ($(TOOLCHAIN),GCC_ARM)
    CFLAGS+=-Wno-cpp
  else ifeq ($(TOOLCHAIN),IAR)
    CFLAGS+=--diag_suppress=Pa183
  else # ARM compiler
    # Workaround windows-specific variable escaping issue
    ifeq ($(OS),Windows_NT)
    HASHMARK:=$(shell echo \\\#)
    else
    HASHMARK:=\$(shell echo \\\#)
    endif
    CFLAGS+=-Wno-pedantic -Wno-$(HASHMARK)warnings
  endif
endif

# Enable LTO-specific build options (DRIVERS-2705)
ifneq ($(LTO),)
  ifeq ($(TOOLCHAIN),GCC_ARM)
    CFLAGS+=-flto
    CXXFLAGS+=-flto
    # Avoid linkage error related to cy_retarget_io.o weak symbols
    LDFLAGS+=-flto -u _read -u _write -u main
  else ifeq ($(TOOLCHAIN),ARM)
    CFLAGS+=-flto
    CXXFLAGS+=-flto
    LDFLAGS+=--lto --lto_level=Oz
  endif
endif

# Enable Coverity-specific build options
ifneq ($(COVERITY),)
  DEFINES+=CY_COVERITY_2012_CHECK
  ifeq ($(TOOLCHAIN),GCC_ARM)
    CFLAGS+=-Wno-unknown-pragmas
  endif
endif

# Check if FreeRTOS was enabled by FUT Makefile
ifneq ($(findstring FREERTOS,$(COMPONENTS)),)
SEARCH+=$(SEARCH_clib-support)
SEARCH+=$(SEARCH_abstraction-rtos)
SEARCH+=$(SEARCH_freertos)
COMPONENTS+=RTOS_AWARE

# Required for FreeRTOS linkage with enabled LTO
# https://stackoverflow.com/a/50172414
ifeq ($(TOOLCHAIN),GCC_ARM)
  LDFLAGS+=-Wl,--undefined=vTaskSwitchContext
else ifeq ($(TOOLCHAIN),ARM)
  LDFLAGS+=--undefined=vTaskSwitchContext
endif
endif

# Generate keys and policies for PSoC 64 secure BSPs
ifeq ($(filter build,$(MAKECMDGOALS)),build)
ifneq (,$(findstring -064,$(TARGET)))
# remove any existing files that were generated previously (possibly for the wrong device)
# Use := to override existing PREBUILD configured by chaitea-core
PREBUILD:=rm -rf keys logs packets policy prebuilt .cysecuretools;
# generate new files for the current target device
PREBUILD+=cysecuretools -t $(TARGET) init;
PREBUILD+=cysecuretools -t $(TARGET) create-keys;
endif
endif

# Prevent device-configurator-cli from updating BSP GeneratedSource
override CY_PREBUILD_GEN_CONFIG=

# Locate ModusToolbox tools folder in output/tools_2.4
# Use absolete path for configurators to work correctly
ifeq ($(OS),Windows_NT)
# Cygwin: convert /cygdrive/c/Users to C:/Users
CY_TOOLS_PATHS:=$(shell cygpath -m --absolute $(CY_GETLIBS_SHARED_PATH)/tools_2.4)
else
CY_TOOLS_PATHS:=$(abspath $(CY_GETLIBS_SHARED_PATH)/tools_2.4)
endif

# Default to the newest installed tools folder, or the users override (if it's
# found).
CY_TOOLS_DIR=$(lastword $(sort $(wildcard $(CY_TOOLS_PATHS))))

ifeq ($(CY_TOOLS_DIR),)
$(error Unable to find any of the available CY_TOOLS_PATHS -- $(CY_TOOLS_PATHS). On Windows, use forward slashes.)
endif

#$(info Tools Directory: $(CY_TOOLS_DIR))

include $(CY_TOOLS_DIR)/make/start.mk

# Enable Chaitea-specific override options for test/chaitea/*
ifneq ($(findstring test/chaitea/,$(CURDIR)),)
include $(ROOT_PATH)/test/chaitea/chaitea-core/make/override_common.mk
endif

#
# Set file-specific CFLAGS to suppress the compiler warnings
# Note: the custom rules are defined after 'include make/start.mk'
# to override standard build rules defined by core-make build.mk
# $(1) : source file name
# $(2) : custom CFLAGS
#
define CY_MACRO_CUSTOM_CFLAGS
$(filter %$(1:%.c=%.o),$(CY_BUILD_ALL_OBJ_FILES)): $(filter %$(1),$(CY_SEARCH_APP_SOURCE) $(SOURCES))
	$$(info $$(CY_INDENT)Compiling file $$(CY_COMPILE_PRINT) with $(2))
	$$(CY_NOISE)$$(CY_BUILD_COMPILE_C) $$@ $$< $(2)
endef

# -Werror mode: suppress warnings from third-party assets
ifneq ($(WERROR),)
ifneq ($(CY_SECONDSTAGE),)

ifeq ($(TOOLCHAIN), ARM)
  # cy_retarget_io.c:506:1: warning: function '_sys_exit' could be declared with attribute 'noreturn' [-Wmissing-noreturn]
  # cy_retarget_io.c:566:5: warning: 'cy_retarget_io_mutex_deinit' was marked unused but was used [-Wused-but-marked-unused]
  $(eval $(call CY_MACRO_CUSTOM_CFLAGS,cy_retarget_io.c,-Wno-missing-noreturn -Wno-used-but-marked-unused))

  # xmc_math.c:182:76: error: unknown attribute 'externally_visible' ignored [-Wunknown-attributes]
  $(eval $(call CY_MACRO_CUSTOM_CFLAGS,xmc_math.c,-Wno-unknown-attributes))
endif

endif # CY_SECONDSTAGE
endif # WERROR
