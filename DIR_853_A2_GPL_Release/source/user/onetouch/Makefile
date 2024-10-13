
all:
	make -C onetouch
	make -C onetouch/easyroaming
	make -C onetouch/device
	
romfs:	
	install -m 755 onetouch/goahead $(ROOTDIR)/romfs/usr/sbin/goahead
	install -m 755 onetouch/onetouch $(ROOTDIR)/romfs/usr/sbin/onetouch
	install -m 755 onetouch/easyroaming/easyroaming $(ROOTDIR)/romfs/usr/sbin/easyroaming
	install -m 755 onetouch/dxml $(ROOTDIR)/romfs/usr/sbin/dxml
	ln -sf ./dxml $(ROOTDIR)/romfs/usr/sbin/dxmlc
	ln -sf ./dxml $(ROOTDIR)/romfs/usr/sbin/xmldb
	ln -sf ./dxml $(ROOTDIR)/romfs/usr/sbin/xmldbc
	install onetouch/easyroaming/dlink_nl_ctrl/dlink_nl_ctrl.ko $(ROOTDIR)/romfs/lib/modules/dlink_nl_ctrl.ko
#	install onetouch/easyroaming/dlink_nl_ctrl/mt_wifi.ko $(ROOTDIR)/romfs/lib/modules/mt_wifi.ko
#	install $(ROOTDIR)/$(LINUXDIR)/drivers/net/wireless/MTK7615/mt_wifi_ap/mt_wifi.ko $(ROOTDIR)/romfs/lib/modules/mt_wifi.ko
	install onetouch/easyroaming/libaes.so $(ROOTDIR)/romfs/lib
	install onetouch/libstarter.so $(ROOTDIR)/romfs/lib
	install onetouch/libdxmlapi.so $(ROOTDIR)/romfs/lib
	install -d $(ROOTDIR)/romfs/etc_ro/onetouch
	cp -rf onetouch/scripts $(ROOTDIR)/romfs/etc_ro/onetouch/scripts
	cp -rf onetouch/www $(ROOTDIR)/romfs/www
	ln -sf /var/tmp $(ROOTDIR)/romfs/www/tmp	
	make -C onetouch/device install

clean:
	make -C onetouch clean
	make -C onetouch/easyroaming clean
	make -C onetouch/device clean