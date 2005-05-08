dnl Initialise autoconf

AC_INIT(apogee, 0.0.1)
AC_CONFIG_SRCDIR([app/perigee.cpp])
AC_CONFIG_HEADERS([config.h])

dnl Detect the canonical host environment

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

dnl Initialise automake

AM_INIT_AUTOMAKE([dist-bzip2])

dnl Need libtool

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

STATIC_LIBSTDCPP=
STATIC_LIBGCC=

AC_ARG_ENABLE(static_link,
  [  --enable-static-link    enable static link of some libs [default=no]],
  [
    if test "$enableval" = "yes"; then
      link_static="true"
      STATIC_LIBSTDCPP=`$CXX -print-file-name=libstdc++.a`
      STATIC_LIBGCC=`$CXX -print-file-name=libgcc.a`
    else
      link_static="false"
    fi
  ],[
    link_static="false"
  ]
)
AM_CONDITIONAL(BUILD_STATIC, test x"$link_static" = xtrue)

AC_LANG(C++)

AC_CHECK_FUNC(rint,,[AC_CHECK_LIB(m,rint)])
AC_CHECK_FUNCS([rintf lrintf lrint])

dnl Test for libraries

SDL_VERSION=1.2.0
WF_CHECK_SDL_IMAGE($SDL_VERSION)

WF_CHECK_GL_LIBS

PKG_CHECK_MODULES(WF, eris-1.3 >= 1.3.4 mercator-0.2 = 0.2.2 varconf-1.0 sage ,
    [
        CPPFLAGS="$CPPFLAGS $WF_CFLAGS"
        LIBS="$LIBS $WF_LIBS"
    ],
    [
        AC_MSG_ERROR(Couldn't find WorldForge libs.)
    ])

AM_PATH_LIB3DS(1.2.0, , AC_MSG_ERROR(Couldn't find lib3ds))
CPPFLAGS="$CPPFLAGS $LIB3DS_CFLAGS"
LIBS="$LIBS $LIB3DS_LIBS"

PKG_CHECK_MODULES(CAL3D, cal3d >= 0.10.0,
    [
        CPPFLAGS="$CPPFLAGS $CAL3D_CFLAGS"
        LIBS="$LIBS $CAL3D_LIBS"
    ],
    [
        AC_MSG_ERROR(Could not find cal3d library)
    ])

dnl AC_CHECK_LIB(cal3d, main,
dnl [
    dnl LIBS="$LIBS -lcal3d"
dnl ], AC_MSG_ERROR(Could not find cal3d library))

dnl add SDL flags at the end to avoid SDL_main problems
CXXFLAGS="$CXXFLAGS $WF_SDL_IMAGE_CFLAGS"
LIBS="$LIBS $WF_SDL_IMAGE_LIBS"

dnl Generate files
AC_CONFIG_FILES([
	Makefile
	common/Makefile
	visual/Makefile
	app/Makefile
	gui/Makefile
])
AC_OUTPUT
