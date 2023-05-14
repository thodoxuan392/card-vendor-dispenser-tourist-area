################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/netif/src/netif.c 

OBJS += \
./Core/Lib/netif/src/netif.o 

C_DEPS += \
./Core/Lib/netif/src/netif.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/netif/src/%.o Core/Lib/netif/src/%.su: ../Core/Lib/netif/src/%.c Core/Lib/netif/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib" -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib/netif/inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-netif-2f-src

clean-Core-2f-Lib-2f-netif-2f-src:
	-$(RM) ./Core/Lib/netif/src/netif.d ./Core/Lib/netif/src/netif.o ./Core/Lib/netif/src/netif.su

.PHONY: clean-Core-2f-Lib-2f-netif-2f-src

