################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/serverWorker/responses/res_ORDEN_ALMACENAMIENTO.c \
../src/serverWorker/responses/res_ORDEN_REDUCCIONGLOBAL.c \
../src/serverWorker/responses/res_ORDEN_REDUCCIONLOCAL.c \
../src/serverWorker/responses/res_ORDEN_TRANSFORMACION.c \
../src/serverWorker/responses/res_PETICION_TEMPORAL.c \
../src/serverWorker/responses/responder_ORDENES.c 

OBJS += \
./src/serverWorker/responses/res_ORDEN_ALMACENAMIENTO.o \
./src/serverWorker/responses/res_ORDEN_REDUCCIONGLOBAL.o \
./src/serverWorker/responses/res_ORDEN_REDUCCIONLOCAL.o \
./src/serverWorker/responses/res_ORDEN_TRANSFORMACION.o \
./src/serverWorker/responses/res_PETICION_TEMPORAL.o \
./src/serverWorker/responses/responder_ORDENES.o 

C_DEPS += \
./src/serverWorker/responses/res_ORDEN_ALMACENAMIENTO.d \
./src/serverWorker/responses/res_ORDEN_REDUCCIONGLOBAL.d \
./src/serverWorker/responses/res_ORDEN_REDUCCIONLOCAL.d \
./src/serverWorker/responses/res_ORDEN_TRANSFORMACION.d \
./src/serverWorker/responses/res_PETICION_TEMPORAL.d \
./src/serverWorker/responses/responder_ORDENES.d 


# Each subdirectory must supply rules for building sources it contributes
src/serverWorker/responses/%.o: ../src/serverWorker/responses/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


