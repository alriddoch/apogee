SUBDIRS = common visual gui app

confdir = $(sysconfdir)/apogee
conf_DATA = apogee.vconf

apogee: all
	cd app && $(MAKE) apogee

EXTRA_DIST = apogee.vconf
