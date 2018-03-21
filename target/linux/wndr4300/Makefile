#
# Copyright (C) 2008-2009 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

ARCH:=mips
BOARD:=wndr4300
BOARDNAME:=DNI WNDR4300
CUSTOMER:=dni
FEATURES:=squashfs

LINUX_VERSION:=2.6.31
LINUX_TREEISH:=linux-v2.6.31-dni13
NETFILTER_TREEISH:=netfilter-v2.6.31-dni31
include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += \
	kmod-leds-gpio kmod-input-core kmod-input-polldev \
	kmod-input-gpio-buttons kmod-button-hotplug

define Target/Description
	Build firmware images for DNI WNDR4300 2631
endef

$(eval $(call BuildGitTarget))
