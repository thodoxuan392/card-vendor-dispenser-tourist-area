################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/App/http.c \
../Core/Src/App/mqtt.c \
../Core/Src/App/statemachine.c 

OBJS += \
./Core/Src/App/http.o \
./Core/Src/App/mqtt.o \
./Core/Src/App/statemachine.o 

C_DEPS += \
./Core/Src/App/http.d \
./Core/Src/App/mqtt.d \
./Core/Src/App/statemachine.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/App/%.o Core/Src/App/%.su: ../Core/Src/App/%.c Core/Src/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib" -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib/netif/inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-App

clean-Core-2f-Src-2f-App:
	-$(RM) ./Core/Src/App/http.d ./Core/Src/App/http.o ./Core/Src/App/http.su ./Core/Src/App/mqtt.d ./Core/Src/App/mqtt.o ./Core/Src/App/mqtt.su ./Core/Src/App/statemachine.d ./Core/Src/App/statemachine.o ./Core/Src/App/statemachine.su

.PHONY: clean-Core-2f-Src-2f-App

