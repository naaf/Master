################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Exo1_1.c \
../src/Exo1_2.c \
../src/Exo1_3.c \
../src/Exo2_1.c 

OBJS += \
./src/Exo1_1.o \
./src/Exo1_2.o \
./src/Exo1_3.o \
./src/Exo2_1.o 

C_DEPS += \
./src/Exo1_1.d \
./src/Exo1_2.d \
./src/Exo1_3.d \
./src/Exo2_1.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


