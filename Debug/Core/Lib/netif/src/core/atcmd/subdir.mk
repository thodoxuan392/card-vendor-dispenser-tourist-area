################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Lib/netif/src/core/atcmd/netif_atcmd.c \
../Core/Lib/netif/src/core/atcmd/netif_atcmd_4g.c \
../Core/Lib/netif/src/core/atcmd/netif_atcmd_ethernet.c \
../Core/Lib/netif/src/core/atcmd/netif_atcmd_http.c \
../Core/Lib/netif/src/core/atcmd/netif_atcmd_mqtt.c \
../Core/Lib/netif/src/core/atcmd/netif_atcmd_wifi.c 

OBJS += \
./Core/Lib/netif/src/core/atcmd/netif_atcmd.o \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_4g.o \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_ethernet.o \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_http.o \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_mqtt.o \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_wifi.o 

C_DEPS += \
./Core/Lib/netif/src/core/atcmd/netif_atcmd.d \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_4g.d \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_ethernet.d \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_http.d \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_mqtt.d \
./Core/Lib/netif/src/core/atcmd/netif_atcmd_wifi.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Lib/netif/src/core/atcmd/%.o Core/Lib/netif/src/core/atcmd/%.su: ../Core/Lib/netif/src/core/atcmd/%.c Core/Lib/netif/src/core/atcmd/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib" -I"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/Core/Lib/netif/inc" -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Lib-2f-netif-2f-src-2f-core-2f-atcmd

clean-Core-2f-Lib-2f-netif-2f-src-2f-core-2f-atcmd:
	-$(RM) ./Core/Lib/netif/src/core/atcmd/netif_atcmd.d ./Core/Lib/netif/src/core/atcmd/netif_atcmd.o ./Core/Lib/netif/src/core/atcmd/netif_atcmd.su ./Core/Lib/netif/src/core/atcmd/netif_atcmd_4g.d ./Core/Lib/netif/src/core/atcmd/netif_atcmd_4g.o ./Core/Lib/netif/src/core/atcmd/netif_atcmd_4g.su ./Core/Lib/netif/src/core/atcmd/netif_atcmd_ethernet.d ./Core/Lib/netif/src/core/atcmd/netif_atcmd_ethernet.o ./Core/Lib/netif/src/core/atcmd/netif_atcmd_ethernet.su ./Core/Lib/netif/src/core/atcmd/netif_atcmd_http.d ./Core/Lib/netif/src/core/atcmd/netif_atcmd_http.o ./Core/Lib/netif/src/core/atcmd/netif_atcmd_http.su ./Core/Lib/netif/src/core/atcmd/netif_atcmd_mqtt.d ./Core/Lib/netif/src/core/atcmd/netif_atcmd_mqtt.o ./Core/Lib/netif/src/core/atcmd/netif_atcmd_mqtt.su ./Core/Lib/netif/src/core/atcmd/netif_atcmd_wifi.d ./Core/Lib/netif/src/core/atcmd/netif_atcmd_wifi.o ./Core/Lib/netif/src/core/atcmd/netif_atcmd_wifi.su

.PHONY: clean-Core-2f-Lib-2f-netif-2f-src-2f-core-2f-atcmd

