# 
#  Copyright (C) 2012 Delta Networks Inc.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=noip
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

PKG_GIT_TREEISH:=noip-2.1.9-dni1

include $(INCLUDE_DIR)/package.mk

define Package/noip
	SECTION:=utils
	CATEGORY:=Network
	TITLE:=no-ip DDNS utility.
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/noip.git/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Package/noip/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/noip2 $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,noip))
