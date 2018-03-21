include $(TOPDIR)/config.mk

LIB	= libhw29763847p16p64.a

OBJS	= hw29763847p16p64.o flash.o ar9331gpio.o ../common/ar7240_flash.o ../common/ar7240_s26_phy.o
SOBJS	= ../common/lowlevel_init.o

SOBJS	+= hornet_pll_init.o

$(LIB):	.depend $(OBJS) $(SOBJS)
	$(AR) crv $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(SOBJS:.o=.S) $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(SOBJS:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
