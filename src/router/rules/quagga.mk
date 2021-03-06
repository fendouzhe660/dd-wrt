quagga-configure: ncurses
ifeq ($(CONFIG_QUAGGA_STABLE),y)
	cd quagga-stable/readline && ./configure --host=$(ARCH)-uclibc-linux --prefix=/usr CFLAGS="$(COPTS) $(MIPS16_OPT) -fPIC -fno-strict-aliasing"
	$(MAKE) -C quagga-stable/readline clean all
	cd quagga-stable && ./configure --host=$(ARCH)-uclibc-linux $(CONFIG_QUAGGA_EXTRA) --libdir=/usr/lib --enable-opaque-lsa --enable-ospf-te --disable-ospfclient --enable-multipath=32  --enable-ipv6 --prefix=/usr --disable-ospf6d  --enable-vtysh --enable-user=root --enable-group=root --disable-ospfapi --with-libreadline=$(TOP)/quagga-stable/readline CFLAGS="-fno-strict-aliasing -I$(TOP)/quagga-stable -Drpl_malloc=malloc -Drpl_realloc=realloc $(COPTS)  $(MIPS16_OPT)" LDFLAGS="-L$(TOP)/quagga-stable/readline -L$(TOP)/ncurses/lib -lncurses" 
else
	cd quagga/readline && ./configure --host=$(ARCH)-uclibc-linux --prefix=/usr  CFLAGS="$(COPTS) $(MIPS16_OPT) -fPIC -fno-strict-aliasing"
	$(MAKE) -C quagga/readline clean all
	cd quagga && ./configure --host=$(ARCH)-uclibc-linux $(CONFIG_QUAGGA_EXTRA) --localstatedir=/var/run  --libdir=/usr/lib --enable-opaque-lsa --enable-ospf-te --disable-ospfclient --enable-multipath=32  --enable-ipv6 --prefix=/usr --sysconfdir=/tmp --disable-ospf6d  --enable-vtysh --enable-user=root --enable-group=root --disable-ospfapi --disable-isisd --enable-pie=no --with-libreadline=$(TOP)/quagga/readline CFLAGS="-fno-strict-aliasing -I$(TOP)/quagga -Drpl_malloc=malloc -Drpl_realloc=realloc $(COPTS)  $(MIPS16_OPT)" LDFLAGS="-L$(TOP)/quagga/readline -L$(TOP)/ncurses/lib -lncurses" 
endif

quagga: ncurses
ifeq ($(CONFIG_QUAGGA_STABLE),y)
	$(MAKE) -j 4 -C quagga-stable
else
	$(MAKE) -j 4 -C quagga
endif

quagga-install:
ifeq ($(CONFIG_QUAGGA_STABLE),y)
	make -C quagga-stable DESTDIR=$(INSTALLDIR)/quagga install
else
	make -C quagga DESTDIR=$(INSTALLDIR)/quagga install
endif
	rm -rf $(INSTALLDIR)/quagga/usr/info
	rm -rf $(INSTALLDIR)/quagga/usr/share
	rm -rf $(INSTALLDIR)/quagga/usr/include
	rm -rf $(INSTALLDIR)/quagga/usr/etc
	rm -rf $(INSTALLDIR)/quagga/usr/man
	rm -f $(INSTALLDIR)/quagga/usr/lib/*.a
	rm -f $(INSTALLDIR)/quagga/usr/lib/*.la


quagga-clean:
ifeq ($(CONFIG_QUAGGA_STABLE),y)
	if test -e "quagga-stable/Makefile"; then $(MAKE) -C quagga-stable clean; fi
else
	if test -e "quagga/Makefile"; then $(MAKE) -C quagga clean; fi
endif
