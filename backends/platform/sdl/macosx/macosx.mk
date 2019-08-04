# Special target to create a application wrapper for Mac OS X

ifdef USE_DOCKTILEPLUGIN

# The NsDockTilePlugIn needs to be compiled in both 32 and 64 bits irrespective of how ScummVM itself is compiled.
# Therefore do not use $(CXXFLAGS) and $(LDFLAGS).

ScummVMDockTilePlugin32.o:
	$(CXX) -mmacosx-version-min=10.6 -arch i386 -O2 -c $(srcdir)/backends/taskbar/macosx/dockplugin/dockplugin.m -o ScummVMDockTilePlugin32.o

ScummVMDockTilePlugin32: ScummVMDockTilePlugin32.o
	$(CXX) -mmacosx-version-min=10.6 -arch i386 -bundle -framework Foundation -framework AppKit -fobjc-link-runtime ScummVMDockTilePlugin32.o -o ScummVMDockTilePlugin32

ScummVMDockTilePlugin64.o:
	$(CXX) -mmacosx-version-min=10.6 -arch x86_64 -O2 -c $(srcdir)/backends/taskbar/macosx/dockplugin/dockplugin.m -o ScummVMDockTilePlugin64.o

ScummVMDockTilePlugin64: ScummVMDockTilePlugin64.o
	$(CXX) -mmacosx-version-min=10.6 -arch x86_64 -bundle -framework Foundation -framework AppKit -fobjc-link-runtime ScummVMDockTilePlugin64.o -o ScummVMDockTilePlugin64

ScummVMDockTilePlugin: ScummVMDockTilePlugin32 ScummVMDockTilePlugin64
	lipo -create ScummVMDockTilePlugin32 ScummVMDockTilePlugin64 -output ScummVMDockTilePlugin

scummvm.docktileplugin: ScummVMDockTilePlugin
	mkdir -p scummvm.docktileplugin/Contents
	cp $(srcdir)/dists/macosx/dockplugin/Info.plist scummvm.docktileplugin/Contents
	mkdir -p scummvm.docktileplugin/Contents/MacOS
	cp ScummVMDockTilePlugin scummvm.docktileplugin/Contents/MacOS/
	chmod 644 scummvm.docktileplugin/Contents/MacOS/ScummVMDockTilePlugin

endif

bundle_name = ScummVM.app

bundle: all
ifdef USE_DOCKTILEPLUGIN
bundle: scummvm.docktileplugin
endif
bundle:
	mkdir -p $(bundle_name)/Contents/MacOS
	mkdir -p $(bundle_name)/Contents/Resources
	echo "APPL????" > $(bundle_name)/Contents/PkgInfo
	sed -e 's/$$(PRODUCT_BUNDLE_IDENTIFIER)/org.scummvm.scummvm/' $(srcdir)/dists/macosx/Info.plist >$(bundle_name)/Contents/Info.plist
ifdef USE_SPARKLE
	mkdir -p $(bundle_name)/Contents/Frameworks
	cp $(srcdir)/dists/macosx/dsa_pub.pem $(bundle_name)/Contents/Resources/
	rm -rf $(bundle_name)/Contents/Frameworks/Sparkle.framework
	cp -R $(SPARKLEPATH)/Sparkle.framework $(bundle_name)/Contents/Frameworks/
endif
	cp $(srcdir)/icons/scummvm.icns $(bundle_name)/Contents/Resources/
	cp $(DIST_FILES_DOCS) $(bundle_name)/Contents/Resources/
	cp $(DIST_FILES_THEMES) $(bundle_name)/Contents/Resources/
ifdef DIST_FILES_NETWORKING
	cp $(DIST_FILES_NETWORKING) $(bundle_name)/Contents/Resources/
endif
ifdef DIST_FILES_ENGINEDATA
	cp $(DIST_FILES_ENGINEDATA) $(bundle_name)/Contents/Resources/
endif
ifdef DIST_FILES_VKEYBD
	cp $(DIST_FILES_VKEYBD) $(bundle_name)/Contents/Resources/
endif
	$(srcdir)/devtools/credits.pl --rtf > $(bundle_name)/Contents/Resources/AUTHORS.rtf
	rm $(bundle_name)/Contents/Resources/AUTHORS
	cp $(bundle_name)/Contents/Resources/COPYING.LGPL $(bundle_name)/Contents/Resources/COPYING-LGPL
	cp $(bundle_name)/Contents/Resources/COPYING.FREEFONT $(bundle_name)/Contents/Resources/COPYING-FREEFONT
	cp $(bundle_name)/Contents/Resources/COPYING.OFL $(bundle_name)/Contents/Resources/COPYING-OFL
	cp $(bundle_name)/Contents/Resources/COPYING.BSD $(bundle_name)/Contents/Resources/COPYING-BSD
	chmod 644 $(bundle_name)/Contents/Resources/*
	cp scummvm $(bundle_name)/Contents/MacOS/scummvm
	chmod 755 $(bundle_name)/Contents/MacOS/scummvm
	$(STRIP) $(bundle_name)/Contents/MacOS/scummvm
ifdef USE_DOCKTILEPLUGIN
	mkdir -p $(bundle_name)/Contents/PlugIns
	cp -r scummvm.docktileplugin $(bundle_name)/Contents/PlugIns/
endif

ifdef USE_SPARKLE
ifneq ($(SPARKLEPATH),)
LDFLAGS += -F$(SPARKLEPATH)
endif
LDFLAGS += -framework Sparkle -Wl,-rpath,@loader_path/../Frameworks
endif

# Special target to create a snapshot disk image for Mac OS X
# TODO: Replace AUTHORS by Credits.rtf
osxsnap: bundle
	mkdir ScummVM-snapshot
	cp $(DIST_FILES_DOCS) ./ScummVM-snapshot/
	mv ./ScummVM-snapshot/COPYING ./ScummVM-snapshot/License\ \(GPL\)
	mv ./ScummVM-snapshot/COPYING.LGPL ./ScummVM-snapshot/License\ \(LGPL\)
	mv ./ScummVM-snapshot/COPYING.FREEFONT ./ScummVM-snapshot/License\ \(FREEFONT\)
	mv ./ScummVM-snapshot/COPYING.OFL ./ScummVM-snapshot/License\ \(OFL\)
	mv ./ScummVM-snapshot/COPYING.BSD ./ScummVM-snapshot/License\ \(BSD\)
	$(XCODETOOLSPATH)/SetFile -t ttro -c ttxt ./ScummVM-snapshot/*
	mkdir ScummVM-snapshot/doc
	cp $(srcdir)/doc/QuickStart ./ScummVM-snapshot/doc/QuickStart
	mkdir ScummVM-snapshot/doc/cz
	cp $(DIST_FILES_DOCS_cz) ./ScummVM-snapshot/doc/cz/
	mkdir ScummVM-snapshot/doc/da
	cp $(DIST_FILES_DOCS_da) ./ScummVM-snapshot/doc/da/
	mkdir ScummVM-snapshot/doc/de
	cp $(DIST_FILES_DOCS_de) ./ScummVM-snapshot/doc/de/
	mkdir ScummVM-snapshot/doc/es
	cp $(DIST_FILES_DOCS_es) ./ScummVM-snapshot/doc/es/
	mkdir ScummVM-snapshot/doc/fr
	cp $(DIST_FILES_DOCS_fr) ./ScummVM-snapshot/doc/fr/
	mkdir ScummVM-snapshot/doc/it
	cp $(DIST_FILES_DOCS_it) ./ScummVM-snapshot/doc/it/
	mkdir ScummVM-snapshot/doc/no-nb
	cp $(DIST_FILES_DOCS_no-nb) ./ScummVM-snapshot/doc/no-nb/
	mkdir ScummVM-snapshot/doc/se
	cp $(DIST_FILES_DOCS_se) ./ScummVM-snapshot/doc/se/
	$(XCODETOOLSPATH)/SetFile -t ttro -c ttxt ./ScummVM-snapshot/doc/QuickStart
	$(XCODETOOLSPATH)/SetFile -t ttro -c ttxt ./ScummVM-snapshot/doc/*/*
	xattr -w "com.apple.TextEncoding" "utf-8;134217984" ./ScummVM-snapshot/doc/*/*
	$(XCODETOOLSPATH)/CpMac -r $(bundle_name) ./ScummVM-snapshot/
	cp $(srcdir)/dists/macosx/DS_Store ./ScummVM-snapshot/.DS_Store
	cp $(srcdir)/dists/macosx/background.jpg ./ScummVM-snapshot/background.jpg
	$(XCODETOOLSPATH)/SetFile -a V ./ScummVM-snapshot/.DS_Store
	$(XCODETOOLSPATH)/SetFile -a V ./ScummVM-snapshot/background.jpg
	hdiutil create -ov -format UDZO -imagekey zlib-level=9 -fs HFS+ \
					-srcfolder ScummVM-snapshot \
					-volname "ScummVM" \
					ScummVM-snapshot.dmg
	rm -rf ScummVM-snapshot

# Mark special targets as phony
.PHONY: bundle osxsnap
