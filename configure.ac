dnl Initialise autoconf

AC_INIT()
AM_CONFIG_HEADER(config.h)

dnl Detect the canonical host and target environment

AC_CANONICAL_SYSTEM

dnl Initialise automake

VERSION=0.0.1

AM_INIT_AUTOMAKE(apogee, $VERSION)

dnl Need libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Test for C++ compiler

AC_PROG_CXX

CXXFLAGS="$CXXFLAGS -Wall -DATLAS_ELEMENT_TYPEDEF_PRIVATE"

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -DNDEBUG"
    ]
)

AC_LANG_CPLUSPLUS
AC_REQUIRE_CPP

dnl Test for libraries

SDL_VERSION=1.2.0
WF_CHECK_SDL_IMAGE($SDL_VERSION)

WF_CHECK_GL_LIBS

PKG_CHECK_MODULES(WF, eris-1.3 mercator-0.2 varconf-1.0 sage ,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        LIBS="$LIBS $WF_LIBS"
    ],
    [
        AC_MSG_ERROR(Couldn't find WF libs.)
    ])

AM_PATH_LIB3DS(1.2.0, , AC_MSG_ERROR(Couldn't find lib3ds))
CPPFLAGS="$CPPFLAGS $LIB3DS_CFLAGS"
LIBS="$LIBS $LIB3DS_LIBS"

AC_CHECK_LIB(cal3d, main,
[
    LIBS="$LIBS -lcal3d"
], AC_MSG_ERROR(Could not find cal3d library))

dnl add SDL flags at the end to avoid SDL_main problems
CXXFLAGS="$CXXFLAGS $WF_SDL_IMAGE_CFLAGS"
LIBS="$LIBS $WF_SDL_IMAGE_LIBS"

dnl Generate files
AC_OUTPUT([
	Makefile
	common/Makefile
	visual/Makefile
	gui/Makefile
	app/Makefile
])
