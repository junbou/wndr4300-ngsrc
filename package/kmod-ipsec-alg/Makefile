# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-ipsec-alg
PKG_RELEASE:=1
ifeq ($(LINUX_VERSION),2.6.15)
  PKG_GIT_TREEISH:=kmod-ipsec-alg-v2.6.15-dni2
  NF=ip
else
  PKG_GIT_TREEISH:=kmod-ipsec-alg-v2.6.31-dni4
  NF=nf
endif
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define KernelPackage/ipsec-alg
  SUBMENU:=DNI Kernel Module
  DEPENDS:=@TARGET_dniwndr3700u2631||TARGET_wndr4300
  TITLE:=DNI IPSec ESP ALG
  VERSION:=$(LINUX_VERSION)-$(PKG_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/$(NF)_conntrack_proto_esp.$(LINUX_KMOD_SUFFIX)	\
	$(PKG_BUILD_DIR)/$(NF)_nat_proto_esp.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,46,$(NF)_conntrack_proto_esp $(NF)_nat_proto_esp)
endef

define Build/Prepare
	(cd $(KERNEL_BUILD_DIR); cp -a $(GIT_HOME)/$(PKG_NAME).git/* $(PKG_BUILD_DIR))
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		CROSS_COMPILE="$(TARGET_CROSS)" \
		ARCH="$(LINUX_KARCH)" \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		EXTRA_CFLAGS="$(BUILDFLAGS)" \
		modules
endef

$(eval $(call KernelPackage,ipsec-alg))
