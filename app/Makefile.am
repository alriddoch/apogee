bin_PROGRAMS = apogee perigee

INCLUDES = $(GNOME_XML_CFLAGS)

apogee_SOURCES = \
                 Application.cpp Application.h \
                 GameClient.cpp GameClient.h \
		 WorldEntity.cpp WorldEntity.h \
                 IsoClient.cpp IsoClient.h \
                 apogee.cpp

apogee_LDADD = ../gui/libgui.a ../visual/libvisual.a

perigee_SOURCES = \
                 Application.cpp Application.h \
                 GameClient.cpp GameClient.h \
		 WorldEntity.cpp WorldEntity.h \
                 IxClient.cpp IxClient.h \
                 perigee.cpp

perigee_LDADD = ../gui/libgui.a ../visual/libvisual.a
