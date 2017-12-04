################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/serverWorker/serverChild.c \
../src/serverWorker/serverWorker.c 

OBJS += \
./src/serverWorker/serverChild.o \
./src/serverWorker/serverWorker.o 

C_DEPS += \
./src/serverWorker/serverChild.d \
./src/serverWorker/serverWorker.d 


# Each subdirectory must supply rules for building sources it contributes
src/serverWorker/%.o: ../src/serverWorker/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


