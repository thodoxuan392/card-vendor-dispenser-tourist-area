################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/Device/bill_acceptor.c \
../Core/Src/Device/eeprom.c \
../Core/Src/Device/keypad.c \
../Core/Src/Device/lcd.c \
../Core/Src/Device/tcd.c 

OBJS += \
./Core/Src/Device/bill_acceptor.o \
./Core/Src/Device/eeprom.o \
./Core/Src/Device/keypad.o \
./Core/Src/Device/lcd.o \
./Core/Src/Device/tcd.o 

C_DEPS += \
./Core/Src/Device/bill_acceptor.d \
./Core/Src/Device/eeprom.d \
./Core/Src/Device/keypad.d \
./Core/Src/Device/lcd.d \
./Core/Src/Device/tcd.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/Device/%.o Core/Src/Device/%.su: ../Core/Src/Device/%.c Core/Src/Device/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib" -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib/netif/inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-Device

clean-Core-2f-Src-2f-Device:
	-$(RM) ./Core/Src/Device/bill_acceptor.d ./Core/Src/Device/bill_acceptor.o ./Core/Src/Device/bill_acceptor.su ./Core/Src/Device/eeprom.d ./Core/Src/Device/eeprom.o ./Core/Src/Device/eeprom.su ./Core/Src/Device/keypad.d ./Core/Src/Device/keypad.o ./Core/Src/Device/keypad.su ./Core/Src/Device/lcd.d ./Core/Src/Device/lcd.o ./Core/Src/Device/lcd.su ./Core/Src/Device/tcd.d ./Core/Src/Device/tcd.o ./Core/Src/Device/tcd.su

.PHONY: clean-Core-2f-Src-2f-Device

