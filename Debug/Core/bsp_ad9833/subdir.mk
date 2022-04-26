################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/bsp_ad9833/ad9833.c 

OBJS += \
./Core/bsp_ad9833/ad9833.o 

C_DEPS += \
./Core/bsp_ad9833/ad9833.d 


# Each subdirectory must supply rules for building sources it contributes
Core/bsp_ad9833/ad9833.o: ../Core/bsp_ad9833/ad9833.c Core/bsp_ad9833/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32CUBE-IDE/STM32CubeIDEworkspace/F407-2019D/Core/bsp_ad9833" -I"D:/STM32CUBE-IDE/STM32CubeIDEworkspace/F407-2019D/Core/bsp_hmi" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Core/bsp_ad9833/ad9833.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

