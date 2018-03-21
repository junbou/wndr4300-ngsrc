#
# (C) Copyright 2003
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= lib$(CPU).a

ifeq ($(COMRESSED_UBOOT),1)
START	= start.o start_bootstrap.o
endif

ifeq ($(BOARD), hw29763654p16p128)
START	= start.o
endif

ifeq ($(BOARD), wndr3700v1h2)
START	= start.o
endif

ifeq ($(BOARD), hw29763847p16p64)
START	= start.o
endif

ifeq ($(BOARD), hw29763904p4p32)
START	= start_$(BOARD).o
endif

ifeq ($(BOARD), hw29763948p128p128)
START	= start_hw29763948p128p128.o
endif

ifeq ($(BOARD), hw29763948p0p128p128p2x2p3x3)
START	= start_hw29763948p0p128p128p2x2p3x3.o
endif

ifeq ($(BOARD), hw29764037p8p64)
START	= start.o
endif

ifeq ($(BOARD), wndr3700v3)
START	= start.o
endif

ifeq ($(BOARD), wnr1000v2)
START	= start.o
endif

ifeq ($(BOARD), wnr1000v4)
START	= start.o
endif

ifeq ($(BOARD), wnr1100)
START	= start.o
endif

ifeq ($(BOARD), wnr2000v3)
START	= start.o
endif

ifeq ($(BOARD), wnr2200)
START	= start.o
endif

OBJS	= interrupts.o

ifeq ($(BOARD), hw29763654p16p128)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wndr3700v1h2)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), hw29763847p16p64)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), hw29763904p4p32)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), hw29763948p128p128)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), hw29763948p0p128p128p2x2p3x3)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), hw29764037p8p64)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wndr3700v3)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wnr1000v2)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wnr1000v4)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wnr1100)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wnr2000v3)
OBJS	+= cpu.o
endif

ifeq ($(BOARD), wnr2200)
OBJS	+= cpu.o
endif

SOBJS	= cache.o

all:	.depend $(START) $(LIB)

$(LIB):	$(OBJS) $(SOBJS)
	$(AR) crv $@ $(OBJS) $(SOBJS)

#########################################################################

.depend:	Makefile $(START:.o=.S) $(OBJS:.o=.c)
		$(CC) -M $(CFLAGS) $(START:.o=.S) $(OBJS:.o=.c) > $@

sinclude .depend

#########################################################################
