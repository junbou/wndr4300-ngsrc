include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=dni-dmapool
PKG_VERSION:=
PKG_RELEASE:=

#PKG_SOURCE:=
#PKG_SOURCE_URL:=
#PKG_MD5SUM:=
#PKG_CAT:=zcat

PKG_GIT_TREEISH=877466ae28d7706e35a73b75a11931d6adfaaf9f
PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/dni-dmapool

include $(INCLUDE_DIR)/package.mk

define KernelPackage/dni-dmapool
  SUBMENU:= 
  DEPENDS:=@TARGET_WNDR3700U||TARGET_wndr4300
  TITLE:=Module to pre-allocated dma pool
  VERSION:=$(LINUX_VERSION)+$(PKG_VERSION)-$(BOARD)-$(PKG_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/dni-dmapool.$(LINUX_KMOD_SUFFIX)
  #AUTOLOAD:=$(call AutoLoad,40,dni-dmapool)
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/dni-dmapool.git/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
endef

define Build/InstallDev
endef

define Build/UninstallDev
endef

define KernelPackage/dni-dmapool/install
	mkdir -p $(1)/etc/init.d
	$(INSTALL_BIN) ./files/dni-dmapool.init $(1)/etc/init.d/dni-dmapool
endef

$(eval $(call KernelPackage,dni-dmapool))
