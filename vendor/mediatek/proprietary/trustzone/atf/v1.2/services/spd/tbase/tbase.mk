

TBASE_DIR		:=	services/spd/tbase
SPD_INCLUDES		:=	-I${TBASE_DIR}

SPD_SOURCES		:=	tbase_fastcall.c	\
				tbase_setup.c		\
				tbase_pm.c		\
				tbase_helpers.S		\
				tbase_common.c

NEED_BL32       :=  no

vpath %.c ${TBASE_DIR}
vpath %.S ${TBASE_DIR}

