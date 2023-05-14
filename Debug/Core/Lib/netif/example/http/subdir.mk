################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/netif/example/http/delete.c \
../Core/Lib/netif/example/http/get.c \
../Core/Lib/netif/example/http/post.c \
../Core/Lib/netif/example/http/put.c 

OBJS += \
./Core/Lib/netif/example/http/delete.o \
./Core/Lib/netif/example/http/get.o \
./Core/Lib/netif/example/http/post.o \
./Core/Lib/netif/example/http/put.o 

C_DEPS += \
./Core/Lib/netif/example/http/delete.d \
./Core/Lib/netif/example/http/get.d \
./Core/Lib/netif/example/http/post.d \
./Core/Lib/netif/example/http/put.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/netif/example/http/%.o Core/Lib/netif/example/http/%.su: ../Core/Lib/netif/example/http/%.c Core/Lib/netif/example/http/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib" -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib/netif/inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-netif-2f-example-2f-http

clean-Core-2f-Lib-2f-netif-2f-example-2f-http:
	-$(RM) ./Core/Lib/netif/example/http/delete.d ./Core/Lib/netif/example/http/delete.o ./Core/Lib/netif/example/http/delete.su ./Core/Lib/netif/example/http/get.d ./Core/Lib/netif/example/http/get.o ./Core/Lib/netif/example/http/get.su ./Core/Lib/netif/example/http/post.d ./Core/Lib/netif/example/http/post.o ./Core/Lib/netif/example/http/post.su ./Core/Lib/netif/example/http/put.d ./Core/Lib/netif/example/http/put.o ./Core/Lib/netif/example/http/put.su

.PHONY: clean-Core-2f-Lib-2f-netif-2f-example-2f-http

