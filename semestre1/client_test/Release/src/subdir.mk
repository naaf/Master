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
../src/test.c \
../src/visualisation.c 

OBJS += \
./src/SDLS.o \
./src/communication.o \
./src/ihm.o \
./src/main.o \
./src/ricochet_robot.o \
./src/test.o \
./src/visualisation.o 

C_DEPS += \
./src/SDLS.d \
./src/communication.d \
./src/ihm.d \
./src/main.d \
./src/ricochet_robot.d \
./src/test.d \
./src/visualisation.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I"/home/naaf/git/Master/semestre1/client_test/lib" -I"/home/naaf/git/Master/semestre1/client_test/headers" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


