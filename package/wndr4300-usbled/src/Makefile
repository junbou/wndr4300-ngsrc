# $Id$
#
# Makefile for WNDR3700 USB LED driver
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.
#

obj-m	:= wndr4300-usbled.o

ifeq ($(MAKING_MODULES),1)
export-objs := wndr4300-usbled.o

-include $(TOPDIR)/Rules.make
endif
