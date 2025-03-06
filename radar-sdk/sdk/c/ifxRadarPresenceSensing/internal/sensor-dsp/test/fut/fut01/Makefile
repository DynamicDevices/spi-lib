# Configure default target
TARGET=CYSBSYSKIT-DEV-01

# Include common test makefile
include ../../Makefile

# Suppress compiler warnings related to strlen argument sign
ifeq ($(TOOLCHAIN), GCC_ARM)
  $(eval $(call CY_MACRO_CUSTOM_CFLAGS,main.c,-Wno-pointer-sign))
endif
