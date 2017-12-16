################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/consola/interfaces/_interface_table.c \
../src/consola/interfaces/cat_interface.c \
../src/consola/interfaces/cpblock_interface.c \
../src/consola/interfaces/cpfrom_interface.c \
../src/consola/interfaces/cpto_interface.c \
../src/consola/interfaces/format_interface.c \
../src/consola/interfaces/info_interface.c \
../src/consola/interfaces/ls_interface.c \
../src/consola/interfaces/md5_interface.c \
../src/consola/interfaces/mkdir_interface.c \
../src/consola/interfaces/mv_interface.c \
../src/consola/interfaces/rename_interface.c \
../src/consola/interfaces/rm_interface.c 

OBJS += \
./src/consola/interfaces/_interface_table.o \
./src/consola/interfaces/cat_interface.o \
./src/consola/interfaces/cpblock_interface.o \
./src/consola/interfaces/cpfrom_interface.o \
./src/consola/interfaces/cpto_interface.o \
./src/consola/interfaces/format_interface.o \
./src/consola/interfaces/info_interface.o \
./src/consola/interfaces/ls_interface.o \
./src/consola/interfaces/md5_interface.o \
./src/consola/interfaces/mkdir_interface.o \
./src/consola/interfaces/mv_interface.o \
./src/consola/interfaces/rename_interface.o \
./src/consola/interfaces/rm_interface.o 

C_DEPS += \
./src/consola/interfaces/_interface_table.d \
./src/consola/interfaces/cat_interface.d \
./src/consola/interfaces/cpblock_interface.d \
./src/consola/interfaces/cpfrom_interface.d \
./src/consola/interfaces/cpto_interface.d \
./src/consola/interfaces/format_interface.d \
./src/consola/interfaces/info_interface.d \
./src/consola/interfaces/ls_interface.d \
./src/consola/interfaces/md5_interface.d \
./src/consola/interfaces/mkdir_interface.d \
./src/consola/interfaces/mv_interface.d \
./src/consola/interfaces/rename_interface.d \
./src/consola/interfaces/rm_interface.d 


# Each subdirectory must supply rules for building sources it contributes
src/consola/interfaces/%.o: ../src/consola/interfaces/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2017-2c-Grupo-1---K3525/Utilidades" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


