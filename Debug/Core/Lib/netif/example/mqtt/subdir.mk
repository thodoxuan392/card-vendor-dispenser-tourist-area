################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/netif/example/mqtt/publish.c \
../Core/Lib/netif/example/mqtt/subcribe.c 

OBJS += \
./Core/Lib/netif/example/mqtt/publish.o \
./Core/Lib/netif/example/mqtt/subcribe.o 

C_DEPS += \
./Core/Lib/netif/example/mqtt/publish.d \
./Core/Lib/netif/example/mqtt/subcribe.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/netif/example/mqtt/%.o Core/Lib/netif/example/mqtt/%.su: ../Core/Lib/netif/example/mqtt/%.c Core/Lib/netif/example/mqtt/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib" -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib/netif/inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-netif-2f-example-2f-mqtt

clean-Core-2f-Lib-2f-netif-2f-example-2f-mqtt:
	-$(RM) ./Core/Lib/netif/example/mqtt/publish.d ./Core/Lib/netif/example/mqtt/publish.o ./Core/Lib/netif/example/mqtt/publish.su ./Core/Lib/netif/example/mqtt/subcribe.d ./Core/Lib/netif/example/mqtt/subcribe.o ./Core/Lib/netif/example/mqtt/subcribe.su

.PHONY: clean-Core-2f-Lib-2f-netif-2f-example-2f-mqtt

