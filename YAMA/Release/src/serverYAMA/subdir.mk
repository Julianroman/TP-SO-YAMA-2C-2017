################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/serverYAMA/responder_SOLICITUD.c \
../src/serverYAMA/serverYAMA.c 

OBJS += \
./src/serverYAMA/responder_SOLICITUD.o \
./src/serverYAMA/serverYAMA.o 

C_DEPS += \
./src/serverYAMA/responder_SOLICITUD.d \
./src/serverYAMA/serverYAMA.d 


# Each subdirectory must supply rules for building sources it contributes
src/serverYAMA/%.o: ../src/serverYAMA/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


