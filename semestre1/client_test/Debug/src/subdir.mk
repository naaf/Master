################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SDLS.c \
../src/communication.c \
../src/ihm.c \
../src/main.c \
../src/ricochet_robot.c \
../src/test.c 

OBJS += \
./src/SDLS.o \
./src/communication.o \
./src/ihm.o \
./src/main.o \
./src/ricochet_robot.o \
./src/test.o 

C_DEPS += \
./src/SDLS.d \
./src/communication.d \
./src/ihm.d \
./src/main.d \
./src/ricochet_robot.d \
./src/test.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


