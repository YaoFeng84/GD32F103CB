cmake_minimum_required(VERSION 3.10)
 
#=========================================== 以下工具链配置编译器信息 ==========================
 
set(ARM_TOOLCHAIN_PATH "F:/YF/MyTools/MyBin/gcc-arm-none-eabi-10.3-2021.10-win32/bin")
set(CMAKE_C_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe" CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-g++.exe" CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-gcc.exe" CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-objcopy.exe")
set(CMAKE_SIZE "${ARM_TOOLCHAIN_PATH}/arm-none-eabi-size.exe")
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_ASM_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
 
#=========================================== 以下项目配置信息 ==========================
 
set(PROJECT_NAME GD32F103CB)
project(${PROJECT_NAME} LANGUAGES C ASM)
 
#----------------------------------------------- 以下配置的编译文件信息和输出路径信息有关 --------------------------------------------------------------------------------------------------
 
 
# - - - -这是设置输出路径
set(OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/./bin")
 
 
# - - - -这是设置头文件路径
set(INC_DIR 
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommHead"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommHead/CMSIS"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommHead/Peripheral"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommHead/USB"
"${CMAKE_CURRENT_SOURCE_DIR}/../../../MyCodeLib"
)
 
 
# - - - -这是设置源文件
set(SRC_LIST 
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/system_gd32f10x.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_adc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_bkp.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_can.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_crc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_dac.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_dbg.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_dma.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_enet.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_exmc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_exti.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_fmc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_fwdgt.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_gpio.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_i2c.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_misc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_pmu.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_rcu.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_rtc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_sdio.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_spi.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_timer.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_usart.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/Peripheral/gd32f10x_wwdgt.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/USB/usbd_core.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/USB/usbd_enum.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/USB/usbd_lld_core.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/USB/usbd_lld_int.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/USB/usbd_pwr.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/CommSrc/USB/usbd_transc.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_CLK.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_ComDriver.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_DriverTest.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_Flash.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_GPIO.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_RCU.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_Systick.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_USARTn.c"
"${CMAKE_CURRENT_SOURCE_DIR}/../GD32F103CB_Code/DRI_USB.c"
)
 
 
# - - - -这是设置库文件
set(LIB_LIST 
)
 
 
# - - - - - - - - - - - - - 整理编译前执行的命令 - - - - - - - - - - - - - - - - - - - -
 
set(BUILD_BEFORE 
)
 
 
## - - - - - - - - - - - - - 整理编译后执行的命令 - - - - - - - - - - - - - - - - - - - -
 
set(BUILD_AFTER 
)
 
 
#----------------------------------------------- 以下配置和芯片平台有关 --------------------------------------------------------------------------------------------------
 
set(COMMON_COMPILE_FLAGS 
-mcpu=cortex-m3
-mthumb
-specs=nosys.specs
-Wall
-DGD32F10X_MD
-DUSE_STDPERIPH_DRIVER
)
 
set(LINK_SCRIPT 
)
set(COMMON_LINK_FLAGS 
-mcpu=cortex-m3
-mthumb
-specs=nosys.specs
-Wall
-DGD32F10X_MD
-DUSE_STDPERIPH_DRIVER
-T${LINK_SCRIPT}
-Wl,-Map=${OUTPUT_DIR}/${PROJECT_NAME}.map
)
 
#----------------------------------------------- 以下配置一般都会有 --------------------------------------------------------------------------------------------------
 
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
 
add_library(${PROJECT_NAME} STATIC ${SRC_LIST})
 
target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIR})
 
set_target_properties(${PROJECT_NAME} PROPERTIES
ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR}
PREFIX ""
)
 
target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_COMPILE_FLAGS})
 
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_LIST})
 
 
# - - - - - - - - - - - - - 开始编译前执行的命令 - - - - - - - - - - - - - - - - - - - -
 
set(STAMP_FILE "${CMAKE_CURRENT_BINARY_DIR}/before_build.stamp")
add_custom_command(
OUTPUT ${STAMP_FILE}
COMMAND ${CMAKE_COMMAND} -E touch ${STAMP_FILE}
COMMAND ${BUILD_BEFORE}
COMMAND ${CMAKE_COMMAND} -E remove -f ${STAMP_FILE}
VERBATIM
)
add_custom_target(
run_before_build
DEPENDS ${STAMP_FILE}
)
add_dependencies(${PROJECT_NAME} run_before_build)
 
# - - - - - - - - - - - - - 开始编译后执行的命令 - - - - - - - - - - - - - - - - - - - -
 
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND ${CMAKE_COMMAND} -E echo "===== Starting post-build steps ====="
COMMAND ${CMAKE_COMMAND} -E echo "Process User file... "
COMMAND "${BUILD_AFTER}"
COMMENT "Process SUCCESS !!!!!!!!!!!!!"
)
