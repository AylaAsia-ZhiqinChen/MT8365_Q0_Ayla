###################################################################
# flag
###################################################################

MOD_SRC  = 
MOD_OBJ  = $(PRELOADER_OUT)/obj

###################################################################
# source files
# - add your file here !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
###################################################################

MOD_SRC  := \
	blkdev.c \
	download.c \
	handshake_uart.c \
	handshake_usb.c \
	main.c \
	print.c \
	dram_buffer.c \
	
ifeq ("$(MTK_EMMC_SUPPORT)","yes")
  MOD_SRC  += partition.c
endif

ifneq ("$(MTK_EMMC_SUPPORT)","yes")
  MOD_SRC  += part.c
endif
###################################################################
# target
###################################################################

include $(COMMON_FILE_MK)
