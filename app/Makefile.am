bin_PROGRAMS = apogee perigee

INCLUDES = $(GNOME_XML_CFLAGS)

apogee_SOURCES = \
                 Application.h \
                 IsoClient.cpp IsoClient.h \
                 apogee.cpp

apogee_LDADD = ../visual/libvisual.a ../gui/libgui.a

perigee_SOURCES = \
                 Application.h \
                 IxClient.cpp IxClient.h \
                 perigee.cpp

perigee_LDADD = ../visual/libvisual.a ../gui/libgui.a

#                 LeakTracer.cc \

