dnl Initialise autoconf

AC_INIT()
AM_CONFIG_HEADER(config.h)

dnl Detect the canonical host and target environment

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Initialise automake

VERSION=0.0.1

AM_INIT_AUTOMAKE(apogee, $VERSION)

dnl check if user wishes maintainer-mode ("--enable-maintainer-mode = yes")
AM_MAINTAINER_MODE

dnl Need libtool

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Test for C++ compiler

AC_PROG_CXX

AC_ARG_ENABLE(debug,
    [  --enable-debug          enable debug information [default=no]],
    [
        if test "$enableval" = "yes"; then
            CXXFLAGS="$CXXFLAGS -Wall -DDEBUG"
        else
            CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
        fi
    ],[
        CXXFLAGS="$CXXFLAGS -Wall -DNDEBUG"
    ]
)

AC_LANG_CPLUSPLUS
AC_REQUIRE_CPP

dnl Test for libraries

SDL_VERSION=1.2.0
AM_PATH_SDL($SDL_VERSION)
CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
LIBS="$LIBS $SDL_LIBS"

AC_CHECK_LIB(SDL_image,IMG_Load)
AC_CHECK_LIB(GL,glViewport)
AC_CHECK_LIB(GLU,gluPerspective)

PKG_CHECK_MODULES(WF, eris-discordia-1.2 coal-blade-0.4 coal-iso-0.4 mercator-0.2 varconf-1.0 ,
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

AC_CHECK_LIB(cal3d,main)

dnl Generate files
AC_OUTPUT([
	Makefile
	common/Makefile
	visual/Makefile
	gui/Makefile
	app/Makefile
])
