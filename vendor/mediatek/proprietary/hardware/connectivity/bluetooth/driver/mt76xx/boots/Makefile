include ./Makefile.rules

OBJS_BOOTS = \
    boots.o \
    boots_pkt.o \
    boots_skt.o \
    boots_eth.o \
    boots_uart.o \
    boots_script.o \
    boots_stress.o \
    boots_country.o \
    boots_osi.o \
    $(NULL)

OBJS_BOOTS_SRV = \
    boots_srv.o \
    boots_btif.o \
    boots_hci.o \
    boots_pkt.o \
    boots_skt.o \
    boots_eth.o \
    boots_mt6xx.o \
    boots_uart.o \
    boots_script.o \
    boots_stress.o \
    boots_osi.o \
    $(NULL)

#---------------------------------------------------------------------------
# $(BT_VCOM_RELAYER)
#     is used to define how to OPEN VCOM port.
# If VCOM port is opened default, no need to open by boots,
#     Set BT_VCOM_RELAYER=-DVCOM_OPENED
# If open VCOM port by set property "sys.usb.config",
#     Set BT_VCOM_RELAYER=-DOPEN_VCOM_BY_SET_PROP
#---------------------------------------------------------------------------
CCFLAGS = \
    -Wall \
    $(LOCAL_CFLAGS) \
    $(BT_VCOM_RELAYER) \
    $(BOOTS_SOCKET_PATH) \
    $(NULL)

LINKFLAGS = \
    -lpthread \
    $(NULL)

#---------------------------------------------------------------------------
ifeq ($(CC),)
ifeq ($(CROSS_COMPILE),)
    CC = gcc
    STRIP = strip
else
    CC = $(CROSS_COMPILE)-gcc
    STRIP = $(CROSS_COMPILE)-strip
endif
else ifeq ($(STRIP),)
    STRIP = strip
endif
#---------------------------------------------------------------------------
all: boots boots_srv

%.o: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDES) -c -o $@ $<

boots: $(OBJS_BOOTS)
	$(CC) $(OBJS_BOOTS) $(LINKFLAGS) -o boots
	$(STRIP) boots

boots_srv: $(OBJS_BOOTS_SRV)
	$(CC) $(OBJS_BOOTS_SRV) $(LINKFLAGS) -o boots_srv
	$(STRIP) boots_srv

clean:
	rm -rf boots
	rm -rf boots_srv
	rm -rf boots_clisk
	rm -rf boots_srvsk
	rm -rf *.o

#---------------------------------------------------------------------------
