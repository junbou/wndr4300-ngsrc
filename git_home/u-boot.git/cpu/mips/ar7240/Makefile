include $(TOPDIR)/config.mk

LIB	= lib$(SOC).a

START	=
#OBJS	= ar7240_serial.o ag7240.o ar7240_flash.o meminit.o
OBJS	=
SOBJS	=

ifeq ($(BOARD), hw29763847p16p64)
OBJS	+= meminit.o hornet_serial.o ag7240.o
SOBJS	+= hornet_ddr_init.o
endif

ifeq ($(BOARD), hw29763904p4p32)
OBJS	+= meminit_$(BOARD).o ar7240_serial_$(BOARD).o ag934x_$(BOARD).o
endif

ifeq ($(BOARD), hw29763948p128p128)
OBJS	+= meminit_hw29763948p128p128.o ar7240_serial_hw29763948p128p128.o ag934x_hw29763948p128p128.o
endif

ifeq ($(BOARD), hw29763948p0p128p128p2x2p3x3)
OBJS	+= meminit_hw29763948p0p128p128p2x2p3x3.o ar7240_serial_hw29763948p0p128p128p2x2p3x3.o ag934x_hw29763948p0p128p128p2x2p3x3.o
endif

ifeq ($(BOARD), hw29764037p8p64)
OBJS	+= meminit.o ar7240_serial.o ag934x.o
endif

ifeq ($(BOARD), wndr3700v3)
OBJS	+= meminit.o ar7240_serial.o ag934x.o
endif

ifeq ($(BOARD), wnr1000v2)
OBJS	+= meminit.o ar7240_serial.o ag7240.o
endif

ifeq ($(BOARD), wnr1000v4)
OBJS	+= meminit.o hornet_serial.o ag7240.o
SOBJS	+= hornet_ddr_init.o
endif

ifeq ($(BOARD), wnr1100)
OBJS	+= meminit.o ar7240_serial.o ag7240.o
endif

ifeq ($(BOARD), wnr2000v3)
OBJS	+= meminit.o ar7240_serial.o ag7240.o
endif

ifeq ($(BOARD), wnr2200)
OBJS	+= meminit.o ar7240_serial.o ag7240.o
endif

all:	.depend $(START) $(LIB)

$(LIB):	$(OBJS) $(SOBJS) $(OBJS-0)
	$(AR) crv $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(START:.o=.S) $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(START:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
