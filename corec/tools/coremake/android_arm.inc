#define TARGARCH arm
#define TARGARCH_ABI arm
#define TARGOPTIM release
#define ARM

CCFLAGS += -pipe -msoft-float -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -O2 -fomit-frame-pointer -fstrict-aliasing -funswitch-loops -finline-limit=300 -g
SFLAGS += -march=armv7-a
ANDROID_ABI += armeabi
ANDROID_ARCH = arch-arm
ANDROID_GCC_PREFIX = arm-eabi-
ANDROID_GCC = arm-eabi-4.4.0
ANDROID_GCC_LIB = gcc/arm-eabi/4.4.0/libgcc.a

#include "android.inc"
