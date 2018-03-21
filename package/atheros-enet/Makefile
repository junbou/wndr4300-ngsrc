# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile,v 1.1.2.2 2006/11/14 08:57:31 ronger Exp $

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=atheros-enet
PKG_VERSION:=
PKG_RELEASE:=

PKG_SOURCE:=
PKG_SOURCE_URL:=
PKG_MD5SUM:=
PKG_CAT:=zcat

PKG_GIT_TREEISH:=atheros-enet-v9.2.0.1017-dni37

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/atheros-enet

include $(INCLUDE_DIR)/package.mk

ENET_AUTOLOAD:= athrs_gmac.$(LINUX_KMOD_SUFFIX) dni_enet.$(LINUX_KMOD_SUFFIX)

define KernelPackage/atheros-enet
  SUBMENU:=Ethernet Drivers
  TITLE:=Driver for Atheros ethernet
  URL:=http://www.atheros.com/
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= $(PKG_BUILD_DIR)/athrs_gmac.$(LINUX_KMOD_SUFFIX) $(PKG_BUILD_DIR)/dni_enet.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,50,$(ENET_AUTOLOAD))
endef

ENET_MAKEOPTS=  \
		PATH="$(KERNEL_STAGING_DIR)/bin:$(TARGET_PATH)" \
		ARCH="$(LINUX_KARCH)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		TOOLPREFIX="mips-linux-" \
		TOOLPATH="mips-linux-" \
		ENETDIR=$(PKG_BUILD_DIR)

ifeq ($(CONFIG_ATHEROS_ENET_BOARD),"db120-wndr3700v4")
export BOOT_FROM_NAND=1
export ETH_CONFIG=_s17
include config.db12x
else ifeq ($(CONFIG_ATHEROS_ENET_BOARD),"ap123-wnr2000v4")
export BUILD_CONFIG=_ap123
include config.db12x
export ATH_PHY_SWAP=0
else ifeq ($(CONFIG_ATHEROS_ENET_BOARD),"ap121-wnr1000v4")
endif
PLATFORM_PATCH_DIR:=./patches-$(CONFIG_ATHEROS_ENET_BOARD)

# dni related features
export DNI_ENET_BOARD=$(call qstrip,$(CONFIG_ATHEROS_ENET_BOARD))
export DNI_SIOCGMIIREG=1
export DNI_DEFAULT_MAC=1
export DNI_SWITCH_PHY=1
export DNI_ETH_LED=1
export DNI_QOS=1

define KernelPackage/atheros-enet/config
	source "$(SOURCE)/Config.in"
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/atheros-enet.git/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Build/InstallDev
endef

define Build/UninstallDev
endef

define KernelPackage/atheros-enet/install
	mkdir -p $(1)/sbin
	install -m0755 $(PKG_BUILD_DIR)/tools/ethreg $(1)/sbin/
	-[ -f ./files/$(CONFIG_ATHEROS_ENET_BOARD).vlan-set ] && install -m0755 ./files/$(CONFIG_ATHEROS_ENET_BOARD).vlan-set $(1)/sbin/vlan-set
	mkdir -p $(1)/etc/init.d
	install -m0755 ./files/init.enet $(1)/etc/init.d/
	install -m0755 ./files/db120-wndr3700v4.init.enet.vlan $(1)/etc/init.d/init.enet.vlan
	install -m0755 ./files/net-lan $(1)/etc/init.d/
	install -m0755 ./files/net-iptv $(1)/etc/init.d/
	install -m0755 ./files/net-wan $(1)/etc/init.d/
	install -m0755 $(PKG_BUILD_DIR)/tools/dni_eth $(1)/sbin/
endef

$(eval $(call KernelPackage,atheros-enet))
