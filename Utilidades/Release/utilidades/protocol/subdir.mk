################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilidades/protocol/destroy.c \
../utilidades/protocol/enviar_paquete.c \
../utilidades/protocol/packers.c \
../utilidades/protocol/receive.c \
../utilidades/protocol/senders.c \
../utilidades/protocol/unpackers.c 

OBJS += \
./utilidades/protocol/destroy.o \
./utilidades/protocol/enviar_paquete.o \
./utilidades/protocol/packers.o \
./utilidades/protocol/receive.o \
./utilidades/protocol/senders.o \
./utilidades/protocol/unpackers.o 

C_DEPS += \
./utilidades/protocol/destroy.d \
./utilidades/protocol/enviar_paquete.d \
./utilidades/protocol/packers.d \
./utilidades/protocol/receive.d \
./utilidades/protocol/senders.d \
./utilidades/protocol/unpackers.d 


# Each subdirectory must supply rules for building sources it contributes
utilidades/protocol/%.o: ../utilidades/protocol/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


