################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../socket_utils/socket_utils.c 

OBJS += \
./socket_utils/socket_utils.o 

C_DEPS += \
./socket_utils/socket_utils.d 


# Each subdirectory must supply rules for building sources it contributes
socket_utils/%.o: ../socket_utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


