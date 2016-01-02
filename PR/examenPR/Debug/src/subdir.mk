################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/entree_sortie.c \
../src/fork.c \
../src/ipcs.c \
../src/ipcs_posix.c \
../src/posix_barrier.c \
../src/signaux_unix.c \
../src/thread.c \
../src/wait_barrier.c 

OBJS += \
./src/entree_sortie.o \
./src/fork.o \
./src/ipcs.o \
./src/ipcs_posix.o \
./src/posix_barrier.o \
./src/signaux_unix.o \
./src/thread.o \
./src/wait_barrier.o 

C_DEPS += \
./src/entree_sortie.d \
./src/fork.d \
./src/ipcs.d \
./src/ipcs_posix.d \
./src/posix_barrier.d \
./src/signaux_unix.d \
./src/thread.d \
./src/wait_barrier.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


