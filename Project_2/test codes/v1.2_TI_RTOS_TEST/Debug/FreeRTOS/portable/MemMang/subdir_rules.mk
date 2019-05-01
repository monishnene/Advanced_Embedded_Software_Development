################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
FreeRTOS/portable/MemMang/heap_2.obj: C:/Users/monis/Documents/Academics/AESD/TM4C129_FreeRTOS_Demo-master/TM4C129_FreeRTOS_Demo-master/FreeRTOS/portable/MemMang/heap_2.c $(GEN_OPTS) | $(GEN_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccs900/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/Users/monis/workspace_v9/v1.2" --include_path="C:/Users/monis/workspace_v9/v1.2" --include_path="C:/ti/tirtos_tivac_2_16_00_08/products/TivaWare_C_Series-2.1.1.71b" --include_path="C:/ti/tirtos_tivac_2_16_00_08/products/bios_6_45_01_29/packages/ti/sysbios/posix" --include_path="C:/Users/monis/Documents/Academics/AESD/TM4C129_FreeRTOS_Demo-master/TM4C129_FreeRTOS_Demo-master" --include_path="C:/Users/monis/Documents/Academics/AESD/TM4C129_FreeRTOS_Demo-master/TM4C129_FreeRTOS_Demo-master/FreeRTOS/include" --include_path="C:/Users/monis/Documents/Academics/AESD/TM4C129_FreeRTOS_Demo-master/TM4C129_FreeRTOS_Demo-master/FreeRTOS/portable/CCS/ARM_CM4F" --include_path="C:/Users/monis/Documents/Academics/AESD/TM4C129_FreeRTOS_Demo-master/TM4C129_FreeRTOS_Demo-master/drivers" --include_path="C:/Users/monis/Documents/Academics/AESD/TM4C129_FreeRTOS_Demo-master/TM4C129_FreeRTOS_Demo-master/utils" --include_path="C:/ti/ccs900/ccs/tools/compiler/ti-cgt-arm_18.12.1.LTS/include" --define=ccs="ccs" --define=PART_TM4C1294NCPDT --define=ccs --define=TIVAWARE -g --gcc --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="FreeRTOS/portable/MemMang/$(basename $(<F)).d_raw" --obj_directory="FreeRTOS/portable/MemMang" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


