# 
# Copyright (C) 2006-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-wndr4300-usbled
PKG_RELEASE:=1.1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/wndr4300-usbled
  SUBMENU:=Other modules
  DEPENDS:=@LINUX_2_6_WNDR3700U||TARGET_wndr4300
  TITLE:=WNDR4300 USB LED driver
  FILES:= \
	$(PKG_BUILD_DIR)/wndr4300-usbled.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,20,wndr4300-usbled)
endef

define KernelPackage/wndr4300-usbled/description
 This package contains USB LED driver for Netgear WNDR4300.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

ifeq ($(BOARD),brcm-2.4)
  BUILDFLAGS := -DBROADCOM -DBCMDRIVER -I$(LINUX_DIR)/arch/mips/bcm947xx/include
else
  ifeq ($(BOARD),brcm47xx)
    BUILDFLAGS := -DBROADCOM
  endif
  ifeq ($(BOARD),dniwgt634u)
    BUILDFLAGS := -DBROADCOM
  endif
endif


define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,wndr4300-usbled))
