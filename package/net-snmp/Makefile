# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk

PKG_NAME:=net-snmp
PKG_RELEASE:=1
#PKG_BUILD_DEPENDS:=datalib
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
PKG_TARGET_DIR:=$(PKG_BUILD_DIR)/target

PKG_GIT_TREEISH:=net-snmp-5.0.9-dni7

include $(INCLUDE_DIR)/package.mk

define Package/net-snmp
  SECTION:=utils
  CATEGORY:=Network
  DEPENDS:=@TARGET_dniwndr3700u2631||TARGET_wndr4300
  TITLE:=SNMP monitors the health and welfare of network equipment.
endef

define Package/net-snmp/description
	Net-SNMP is a suite of applications used to implement SNMP v1, SNMP v2c and SNMP v3.
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/net-snmp.git/* $(PKG_BUILD_DIR)
endef

define Build/Configure
	(cd $(PKG_BUILD_DIR); \
		./configure \
                        --with-cc="$(TARGET_CC)" \
                        --with-ar=$(TARGET_CROSS)ar \
			--with-endianness=big \
                        --enable-mini-agent \
                        --enable-debugging \
                        --enable-developer \
                        --build=i386-pc-linux-gnu \
                        --host=mipsel-linux \
                        --with-persistent-directory=/tmp/var/net-snmp \
                        --enable-ucd-snmp-compatability \
                        --with-logfile=none \
                        --without-rpm \
                        --without-openssl \
                        --without-dmalloc \
                        --without-efence \
                        --without-rsaref \
                        --with-sys-contact="root" \
                        --with-sys-location="Unknown" \
                        --with-defaults \
                        --with-default-snmp-version="2" \
                        --disable-snmptrapd \
                        --with-mib-modules="agent/nsModuleTable \
                                            agent/nsTransactionTable \
                                            bridge/dot1dBridge \
                                            dot11/ieee802dot11_scalar \
                                            dot11/ieee802dot11_table \
                                            dot11ext/atherosAPdot11_scalar \
                                            mibII/at \
                                            mibII/icmp \
                                            mibII/interfaces \
                                            mibII/ipAddr \
                                            mibII/ip \
                                            mibII/tcp \
                                            mibII/udp \
					    comcast/comcast" \
                        --with-out-mib-modules="snmpv3/snmpEngine \
                                                snmpv3/snmpMPDStats \
                                                snmpv3/usmStats \
                                                snmpv3/usmUser \
                                                symbol" \
        )
endef

define Build/Compile
	! [ -f $(PKG_BUILD_DIR)/apps/Makefile.bak ] && mv $(PKG_BUILD_DIR)/apps/Makefile $(PKG_BUILD_DIR)/apps/Makefile.bak; \
        sed '100,103s/INSTALLSBINPROGS/#INSTALLSBINPROGS/' $(PKG_BUILD_DIR)/apps/Makefile.bak > $(PKG_BUILD_DIR)/apps/Makefile; \
	$(MAKE) PATH=$(TARGET_PATH) -C $(PKG_BUILD_DIR)
endef

define Package/net-snmp/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) ./files/* $(1)/
	$(CP) $(PKG_BUILD_DIR)/agent/snmpd $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,net-snmp))
