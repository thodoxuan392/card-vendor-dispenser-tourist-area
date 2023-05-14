################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/Hal/subdir.mk
-include Core/Src/Device/subdir.mk
-include Core/Src/App/subdir.mk
-include Core/Src/subdir.mk
-include Core/Lib/utils/subdir.mk
-include Core/Lib/netif/src/manager/subdir.mk
-include Core/Lib/netif/src/core/atcmd/subdir.mk
-include Core/Lib/netif/src/core/subdir.mk
-include Core/Lib/netif/src/apps/subdir.mk
-include Core/Lib/netif/src/subdir.mk
-include Core/Lib/netif/example/mqtt/subdir.mk
-include Core/Lib/netif/example/http/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := card-vendor-103vct6
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
card-vendor-103vct6.elf \

MAP_FILES += \
card-vendor-103vct6.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
card-vendor-103vct6.list \


# All Target
all: main-build

# Main-build Target
main-build: card-vendor-103vct6.elf secondary-outputs

# Tool invocations
card-vendor-103vct6.elf card-vendor-103vct6.map: $(OBJS) $(USER_OBJS) /home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/STM32F103VCTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "card-vendor-103vct6.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"/home/xuanthodo/STM32CubeIDE/workspace_1.10.1/card-vendor-103vct6/STM32F103VCTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="card-vendor-103vct6.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

card-vendor-103vct6.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "card-vendor-103vct6.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) card-vendor-103vct6.elf card-vendor-103vct6.list card-vendor-103vct6.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
