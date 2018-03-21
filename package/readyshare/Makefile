#
# ReadyShare
#

include $(TOPDIR)/rules.mk

PKG_NAME:=readyshare
PKG_RELEASE:=0.0.13

PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/readyshare
	SECTION:=net
	CATEGORY:=Remote
	TITLE:=ReadyShare
#	DESCRIPTION:=ReadyShare Management
endef

define Build/Prepare
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Package/readyshare/install
	install -d -m0755 $(1)/opt/remote/

	mkdir -p $(1)/etc/init.d/
	mkdir -p $(1)/etc/crontabs/
	$(CP) $(PKG_BUILD_DIR)/opt/remote/etc/init.d/*.sh $(1)/etc/init.d/
	$(CP) $(PKG_BUILD_DIR)/opt/remote/etc/cron.d/*    $(1)/etc/crontabs/

	$(CP) -r $(PKG_BUILD_DIR)/opt/remote/bin/ $(1)/opt/remote/
	#$(CP) -r $(PKG_BUILD_DIR)/opt/remote/plugin/ $(1)/opt/remote/
	ln -sf /bin/busybox $(1)/opt/remote/bin/tail
endef

$(eval $(call BuildPackage,readyshare))
