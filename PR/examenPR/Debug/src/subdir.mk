################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/entree_sortie.c \
../src/exemple_mq.c \
../src/fork.c \
../src/ipcs.c \
../src/posix_barrier.c \
../src/resolution_nom.c \
../src/signaux_unix.c \
../src/srv_udp.c \
../src/thread.c \
../src/wait_barrier.c 

OBJS += \
./src/entree_sortie.o \
./src/exemple_mq.o \
./src/fork.o \
./src/ipcs.o \
./src/posix_barrier.o \
./src/resolution_nom.o \
./src/signaux_unix.o \
./src/srv_udp.o \
./src/thread.o \
./src/wait_barrier.o 

C_DEPS += \
./src/entree_sortie.d \
./src/exemple_mq.d \
./src/fork.d \
./src/ipcs.d \
./src/posix_barrier.d \
./src/resolution_nom.d \
./src/signaux_unix.d \
./src/srv_udp.d \
./src/thread.d \
./src/wait_barrier.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


