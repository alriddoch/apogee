EXTRA_PROGRAMS = apogee
bin_PROGRAMS = perigee

INCLUDES = -I$(top_srcdir)

apogee_SOURCES = \
                 Application.cpp Application.h \
                 GameClient.cpp GameClient.h \
                 WorldEntity.cpp WorldEntity.h \
                 IsoClient.cpp IsoClient.h \
                 apogee.cpp

apogee_LDADD = $(top_builddir)/gui/libgui.a \
               $(top_builddir)/visual/libvisual.a \
               $(top_builddir)/common/libcommon.a

perigee_SOURCES = \
                 Application.cpp Application.h \
                 GameClient.cpp GameClient.h \
                 WorldEntity.cpp WorldEntity.h \
                 IxClient.cpp IxClient.h \
                 perigee.cpp

perigee_LDADD = $(top_builddir)/gui/libgui.a \
                $(top_builddir)/visual/libvisual.a \
                $(top_builddir)/common/libcommon.a
