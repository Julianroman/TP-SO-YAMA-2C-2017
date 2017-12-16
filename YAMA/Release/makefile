################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/serverYAMA/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: YAMA

dependents:
	-cd /home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades/Release && $(MAKE) clean all

# Tool invocations
YAMA: $(OBJS) $(USER_OBJS) /home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades/Release/libUtilidades.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades/Release" -o "YAMA" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) YAMA
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades/Release/libUtilidades.so:

-include ../makefile.targets
