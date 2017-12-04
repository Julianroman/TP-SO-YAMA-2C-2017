################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/operaciones/almacenamiento.c \
../src/operaciones/reduccionGlobal.c \
../src/operaciones/reduccionLocal.c \
../src/operaciones/transformacion.c 

OBJS += \
./src/operaciones/almacenamiento.o \
./src/operaciones/reduccionGlobal.o \
./src/operaciones/reduccionLocal.o \
./src/operaciones/transformacion.o 

C_DEPS += \
./src/operaciones/almacenamiento.d \
./src/operaciones/reduccionGlobal.d \
./src/operaciones/reduccionLocal.d \
./src/operaciones/transformacion.d 


# Each subdirectory must supply rules for building sources it contributes
src/operaciones/%.o: ../src/operaciones/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


