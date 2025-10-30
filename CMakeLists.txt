cmake_minimum_required(VERSION 3.5)

# Указываем что это embedded система без ОС
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Disable compiler checking
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(stm32_template C)

# Отключаем автоматические флаги для Windows
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")
set(CMAKE_EXE_LINK_DEFAULT_FLAGS "")

set(TARGET_NAME ${PROJECT_NAME})

# Явно указываем пути к компиляторам
set(ARM_TOOLCHAIN_PATH "C:/Users/Wolfo/CLionProjects/tools/xpack-arm-none-eabi-gcc-13.3.1-1.1/bin/")
set(CMAKE_C_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc.exe)
set(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-c++.exe)
set(CMAKE_ASM_COMPILER ${ARM_TOOLCHAIN_PATH}arm-none-eabi-gcc.exe)
set(OBJCOPY ${ARM_TOOLCHAIN_PATH}arm-none-eabi-objcopy.exe)
set(OBJDUMP ${ARM_TOOLCHAIN_PATH}arm-none-eabi-objdump.exe)
set(SIZE ${ARM_TOOLCHAIN_PATH}arm-none-eabi-size.exe)
set(STFLASH st-flash)

SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/build)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# Флаги компиляции
set(CMAKE_C_FLAGS "-mcpu=cortex-m3 -mthumb -std=c99")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdata-sections -ffunction-sections -mlittle-endian -g3")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-move-loop-invariants -fsigned-char -ffreestanding")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")

# Флаги линковки - УБИРАЕМ дублирование specs
set(LINKER_FLAGS "-nostdlib -nostartfiles")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -Map -Xlinker ${TARGET_NAME}.map")
set(LINKER_FLAGS "${LINKER_FLAGS} -Xlinker -T ${CMAKE_CURRENT_SOURCE_DIR}/stm32f103c8.ld")
set(LINKER_FLAGS "${LINKER_FLAGS} -Wl,--gc-sections")
# УБРАНО: -specs=nosys.specs -specs=nano.specs

message("DIRECTORY ${EXECUTABLE_OUTPUT_PATH}")

# Preprocessor definitions
add_definitions(-DSTM32F103C8)
add_definitions(-DSTM32F10X_MD)

include_directories(cmsis)
include_directories(stm32)

set(SOURCE_FILES
        stm32/startup/startup_stm32f10x_md.c
        stm32/system_stm32f10x.c
        syscalls/syscalls.c
        main.c)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit DESTINATION ${EXECUTABLE_OUTPUT_PATH})

# Создаем executable с явным указанием флагов линковки
add_executable(${TARGET_NAME} ${SOURCE_FILES})

# Устанавливаем флаги линковки через свойство цели
set_target_properties(${TARGET_NAME} PROPERTIES
        LINK_FLAGS "${LINKER_FLAGS}"
        SUFFIX ".elf"
)

# Добавляем specs через отдельное свойство
target_link_options(${TARGET_NAME} PRIVATE
        "SHELL:-specs=nosys.specs"
        "SHELL:-specs=nano.specs"
)

# Targets for make firmware in different formats
set(BIN_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.bin")
set(HEX_TARGET "${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME}.hex")

# Команда для генерации .bin файла
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}> ${BIN_TARGET}
        COMMAND ${SIZE} $<TARGET_FILE:${TARGET_NAME}>
        COMMENT "Generating BIN file"
)

add_custom_target(bin DEPENDS ${TARGET_NAME})

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${OBJCOPY} -O ihex $<TARGET_FILE:${TARGET_NAME}> ${HEX_TARGET}
        COMMENT "Generating HEX file"
)

add_custom_target(hex DEPENDS ${TARGET_NAME})

# Command and target to flash firmware
add_custom_target(flash
        COMMAND ${STFLASH} write ${BIN_TARGET} 0x8000000
        DEPENDS ${TARGET_NAME}
        COMMENT "Flashing firmware to STM32"
)

# Показываем информацию о компиляторе
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "Objcopy: ${OBJCOPY}")