dnl
dnl GNOME_XML_HOOK (script-if-xml-found, failflag)
dnl
dnl If failflag is "failure", script aborts due to lack of XML
dnl 
dnl Check for availability of the libxml library
dnl the XML parser uses libz if available too
dnl

AC_DEFUN([LIBXML_HOOK],[
	AC_PATH_PROG(LIBXML_CONFIG,xml-config,no)
	if test "$LIBXML_CONFIG" = no; then
		if test x$2 = xfailure; then
			AC_MSG_ERROR(Could not find xml-config)
		fi
	fi

	AC_CHECK_LIB(xml, xmlNewDoc, [
		$1
		LIBXML_CFLAGS=`$LIBXML_CONFIG --cflags`
		LIBXML_LIBS=`$LIBXML_CONFIG --libs`
		LIBXML_LIB_PATH=`$LIBXML_CONFIG --libs`
	], [
		if test x$2 = xfailure; then 
			AC_MSG_ERROR(Could not link sample xml program)
		fi
	], `$LIBXML_CONFIG --libs`)

	AC_SUBST(LIBXML_CFLAGS)
	AC_SUBST(LIBXML_LIBS)
	AC_SUBST(LIBXML_LIB_PATH)
])

AC_DEFUN([LIBXML_CHECK], [
	LIBXML_HOOK([],failure)
])

# Configure paths for Libsigc++
# Based on Gtk-- script by Erik Andersen and Tero Pulkkinen 

dnl Test for Libsigc++, and define SIGC_CFLAGS and SIGC_LIBS
dnl   to be used as follows:
dnl AM_PATH_SIGC(MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]])
dnl
AC_DEFUN(AM_PATH_SIGC,
[dnl 
dnl Get the cflags and libraries from the sigc-config script
dnl

dnl
dnl Prefix options
dnl
AC_ARG_WITH(sigc-prefix,
[  --with-sigc-prefix=PREFIX
                          Prefix where libsigc++ is installed (optional)]
, sigc_config_prefix="$withval", sigc_config_prefix="")

AC_ARG_WITH(sigc-exec-prefix,
[  --with-sigc-exec-prefix=PREFIX 
                          Exec prefix where  libsigc++ is installed (optional)]
, sigc_config_exec_prefix="$withval", sigc_config_exec_prefix="")

AC_ARG_ENABLE(sigctest, 
[  --disable-sigctest     Do not try to compile and run a test libsigc++ 
                          program],
, enable_sigctest=yes)

dnl
dnl Prefix handling
dnl
  if test x$sigc_config_exec_prefix != x ; then
     sigc_config_args="$sigc_config_args --exec-prefix=$sigc_config_exec_prefix"
     if test x${SIGC_CONFIG+set} != xset ; then
        SIGC_CONFIG=$sigc_config_exec_prefix/bin/sigc-config
     fi
  fi
  if test x$sigc_config_prefix != x ; then
     sigc_config_args="$sigc_config_args --prefix=$sigc_config_prefix"
     if test x${SIGC_CONFIG+set} != xset ; then
        SIGC_CONFIG=$sigc_config_prefix/bin/sigc-config
     fi
  fi

dnl
dnl See if sigc-config is alive
dnl
  AC_PATH_PROG(SIGC_CONFIG, sigc-config, no)
  sigc_version_min=$1

dnl
dnl  Version check
dnl
  AC_MSG_CHECKING(for libsigc++ - version >= $sigc_version_min)
  no_sigc=""
  if test "$SIGC_CONFIG" = "no" ; then
    no_sigc=yes
  else
    sigc_version=`$SIGC_CONFIG --version`

    SIGC_CFLAGS=`$SIGC_CONFIG $sigc_config_args --cflags`
    SIGC_LIBS=`$SIGC_CONFIG $sigc_config_args --libs`
    SIGC_MACROS=`$SIGC_CONFIG $sigc_config_args --macros`

    sigc_major_version=`echo $sigc_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sigc_minor_version=`echo $sigc_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sigc_micro_version=`echo $sigc_version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    sigc_major_min=`echo $sigc_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sigc_minor_min=`echo $sigc_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sigc_micro_min=`echo $sigc_version_min | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

    sigc_version_proper=`expr \
        $sigc_major_version \> $sigc_major_min \| \
        $sigc_major_version \= $sigc_major_min \& \
        $sigc_minor_version \> $sigc_minor_min \| \
        $sigc_major_version \= $sigc_major_min \& \
        $sigc_minor_version \= $sigc_minor_min \& \
        $sigc_micro_version \>= $sigc_micro_min `

    if test "$sigc_version_proper" = "1" ; then
      AC_MSG_RESULT([$sigc_major_version.$sigc_minor_version.$sigc_micro_version])
    else
      AC_MSG_RESULT(no)
      no_sigc=yes
    fi

    if test "X$no_sigc" = "Xyes" ; then
      enable_sigctest=no
    fi

    AC_LANG_SAVE
    AC_LANG_CPLUSPLUS

dnl
dnl
dnl
    if test "x$enable_sigctest" = "xyes" ; then
      AC_MSG_CHECKING(if libsigc++ sane)
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CXXFLAGS="$CXXFLAGS $SIGC_CFLAGS"
      LIBS="$LIBS $SIGC_LIBS"

      rm -f conf.sigctest
      AC_TRY_RUN([
#include <stdio.h>
#include <sigc++/signal_system.h>

#ifdef SIGC_CXX_NAMESPACES
using namespace SigC;
#endif

int foo1(int i) 
  {
   return 1;
  }

int main(int argc,char **argv)
  {
   if (sigc_major_version!=$sigc_major_version ||
       sigc_minor_version!=$sigc_minor_version ||
       sigc_micro_version!=$sigc_micro_version)
     { printf("(%d.%d.%d) ",
         sigc_major_version,sigc_minor_version,sigc_micro_version);
       return 1;
     }
   Signal1<int,int> sig1;
   sig1.connect(slot(foo1));
   sig1(1);
   return 0;
  }

],[
  AC_MSG_RESULT(yes)
],[
  AC_MSG_RESULT(no)
  no_sigc=yes
]
,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi

  dnl
  dnl
  if test "x$no_sigc" = x ; then
     ifelse([$2], , :, [$2])     
  else
     SIGC_CFLAGS=""
     SIGC_LIBS=""
     SIGC_MACROS=""
     ifelse([$3], , :, [$3])
  fi

  AC_LANG_RESTORE

  AC_SUBST(SIGC_CFLAGS)
  AC_SUBST(SIGC_LIBS)
  AC_SUBST(SIGC_MACROS)
])


dnl PKG_CHECK_MODULES(GSTUFF, gtk+-2.0 >= 1.3 glib = 1.3.4, action-if, action-not)
dnl defines GSTUFF_LIBS, GSTUFF_CFLAGS, see pkg-config man page
dnl also defines GSTUFF_PKG_ERRORS on error
AC_DEFUN(PKG_CHECK_MODULES, [
  succeeded=no

  if test -z "$PKG_CONFIG"; then
    AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  fi

  if test "$PKG_CONFIG" = "no" ; then
     echo "*** The pkg-config script could not be found. Make sure it is"
     echo "*** in your path, or set the PKG_CONFIG environment variable"
     echo "*** to the full path to pkg-config."
     echo "*** Or see http://www.freedesktop.org/software/pkgconfig to get pkg-config."
  else
     PKG_CONFIG_MIN_VERSION=0.9.0
     if $PKG_CONFIG --atleast-pkgconfig-version $PKG_CONFIG_MIN_VERSION; then
        AC_MSG_CHECKING(for $2)

        if $PKG_CONFIG --exists "$2" ; then
            AC_MSG_RESULT(yes)
            succeeded=yes

            AC_MSG_CHECKING($1_CFLAGS)
            $1_CFLAGS=`$PKG_CONFIG --cflags "$2"`
            AC_MSG_RESULT($$1_CFLAGS)

            AC_MSG_CHECKING($1_LIBS)
            $1_LIBS=`$PKG_CONFIG --libs "$2"`
            AC_MSG_RESULT($$1_LIBS)
        else
            $1_CFLAGS=""
            $1_LIBS=""
            ## If we have a custom action on failure, don't print errors, but 
            ## do set a variable so people can do so.
            $1_PKG_ERRORS=`$PKG_CONFIG --errors-to-stdout --print-errors "$2"`
            ifelse([$4], ,echo $$1_PKG_ERRORS,)
        fi

        AC_SUBST($1_CFLAGS)
        AC_SUBST($1_LIBS)
     else
        echo "*** Your version of pkg-config is too old. You need version $PKG_CONFIG_MIN_VERSION or newer."
        echo "*** See http://www.freedesktop.org/software/pkgconfig"
     fi
  fi

  if test $succeeded = yes; then
     ifelse([$3], , :, [$3])
  else
     ifelse([$4], , AC_MSG_ERROR([Library requirements ($2) not met; consider adjusting the PKG_CONFIG_PATH environment variable if your libraries are in a nonstandard prefix so pkg-config can find them.]), [$4])
  fi
])


# Configure paths for SDL
# Sam Lantinga 9/21/99
# stolen from Manish Singh
# stolen back from Frank Belew
# stolen from Manish Singh
# Shamelessly stolen from Owen Taylor

dnl AM_PATH_SDL([MINIMUM-VERSION, [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND]]])
dnl Test for SDL, and define SDL_CFLAGS and SDL_LIBS
dnl
AC_DEFUN(AM_PATH_SDL,
[dnl 
dnl Get the cflags and libraries from the sdl-config script
dnl
AC_ARG_WITH(sdl-prefix,[  --with-sdl-prefix=PFX   Prefix where SDL is installed (optional)],
            sdl_prefix="$withval", sdl_prefix="")
AC_ARG_WITH(sdl-exec-prefix,[  --with-sdl-exec-prefix=PFX Exec prefix where SDL is installed (optional)],
            sdl_exec_prefix="$withval", sdl_exec_prefix="")
AC_ARG_ENABLE(sdltest, [  --disable-sdltest       Do not try to compile and run a test SDL program],
		    , enable_sdltest=yes)

  if test x$sdl_exec_prefix != x ; then
     sdl_args="$sdl_args --exec-prefix=$sdl_exec_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_exec_prefix/bin/sdl-config
     fi
  fi
  if test x$sdl_prefix != x ; then
     sdl_args="$sdl_args --prefix=$sdl_prefix"
     if test x${SDL_CONFIG+set} != xset ; then
        SDL_CONFIG=$sdl_prefix/bin/sdl-config
     fi
  fi

  AC_REQUIRE([AC_CANONICAL_TARGET])
  AC_PATH_PROG(SDL_CONFIG, sdl-config, no)
  min_sdl_version=ifelse([$1], ,0.11.0,$1)
  AC_MSG_CHECKING(for SDL - version >= $min_sdl_version)
  no_sdl=""
  if test "$SDL_CONFIG" = "no" ; then
    no_sdl=yes
  else
    SDL_CFLAGS=`$SDL_CONFIG $sdlconf_args --cflags`
    SDL_LIBS=`$SDL_CONFIG $sdlconf_args --libs`

    sdl_major_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
    sdl_minor_version=`$SDL_CONFIG $sdl_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
    sdl_micro_version=`$SDL_CONFIG $sdl_config_args --version | \
           sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
    if test "x$enable_sdltest" = "xyes" ; then
      ac_save_CFLAGS="$CFLAGS"
      ac_save_CXXFLAGS="$CXXFLAGS"
      ac_save_LIBS="$LIBS"
      CFLAGS="$CFLAGS $SDL_CFLAGS"
      CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
      LIBS="$LIBS $SDL_LIBS"
dnl
dnl Now check if the installed SDL is sufficiently new. (Also sanity
dnl checks the results of sdl-config to some extent
dnl
      rm -f conf.sdltest
      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SDL.h"

char*
my_strdup (char *str)
{
  char *new_str;
  
  if (str)
    {
      new_str = (char *)malloc ((strlen (str) + 1) * sizeof(char));
      strcpy (new_str, str);
    }
  else
    new_str = NULL;
  
  return new_str;
}

int main (int argc, char *argv[])
{
  int major, minor, micro;
  char *tmp_version;

  /* This hangs on some systems (?)
  system ("touch conf.sdltest");
  */
  { FILE *fp = fopen("conf.sdltest", "a"); if ( fp ) fclose(fp); }

  /* HP/UX 9 (%@#!) writes to sscanf strings */
  tmp_version = my_strdup("$min_sdl_version");
  if (sscanf(tmp_version, "%d.%d.%d", &major, &minor, &micro) != 3) {
     printf("%s, bad version string\n", "$min_sdl_version");
     exit(1);
   }

   if (($sdl_major_version > major) ||
      (($sdl_major_version == major) && ($sdl_minor_version > minor)) ||
      (($sdl_major_version == major) && ($sdl_minor_version == minor) && ($sdl_micro_version >= micro)))
    {
      return 0;
    }
  else
    {
      printf("\n*** 'sdl-config --version' returned %d.%d.%d, but the minimum version\n", $sdl_major_version, $sdl_minor_version, $sdl_micro_version);
      printf("*** of SDL required is %d.%d.%d. If sdl-config is correct, then it is\n", major, minor, micro);
      printf("*** best to upgrade to the required version.\n");
      printf("*** If sdl-config was wrong, set the environment variable SDL_CONFIG\n");
      printf("*** to point to the correct copy of sdl-config, and remove the file\n");
      printf("*** config.cache before re-running configure\n");
      return 1;
    }
}

],, no_sdl=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])
       CFLAGS="$ac_save_CFLAGS"
       CXXFLAGS="$ac_save_CXXFLAGS"
       LIBS="$ac_save_LIBS"
     fi
  fi
  if test "x$no_sdl" = x ; then
     AC_MSG_RESULT(yes)
     ifelse([$2], , :, [$2])     
  else
     AC_MSG_RESULT(no)
     if test "$SDL_CONFIG" = "no" ; then
       echo "*** The sdl-config script installed by SDL could not be found"
       echo "*** If SDL was installed in PREFIX, make sure PREFIX/bin is in"
       echo "*** your path, or set the SDL_CONFIG environment variable to the"
       echo "*** full path to sdl-config."
     else
       if test -f conf.sdltest ; then
        :
       else
          echo "*** Could not run SDL test program, checking why..."
          CFLAGS="$CFLAGS $SDL_CFLAGS"
          CXXFLAGS="$CXXFLAGS $SDL_CFLAGS"
          LIBS="$LIBS $SDL_LIBS"
          AC_TRY_LINK([
#include <stdio.h>
#include "SDL.h"

int main(int argc, char *argv[])
{ return 0; }
#undef  main
#define main K_and_R_C_main
],      [ return 0; ],
        [ echo "*** The test program compiled, but did not run. This usually means"
          echo "*** that the run-time linker is not finding SDL or finding the wrong"
          echo "*** version of SDL. If it is not finding SDL, you'll need to set your"
          echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
          echo "*** to the installed location  Also, make sure you have run ldconfig if that"
          echo "*** is required on your system"
	  echo "***"
          echo "*** If you have an old version installed, it is best to remove it, although"
          echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
        [ echo "*** The test program failed to compile or link. See the file config.log for the"
          echo "*** exact error that occured. This usually means SDL was incorrectly installed"
          echo "*** or that you have moved SDL since it was installed. In the latter case, you"
          echo "*** may want to edit the sdl-config script: $SDL_CONFIG" ])
          CFLAGS="$ac_save_CFLAGS"
          CXXFLAGS="$ac_save_CXXFLAGS"
          LIBS="$ac_save_LIBS"
       fi
     fi
     SDL_CFLAGS=""
     SDL_LIBS=""
     ifelse([$3], , :, [$3])
  fi
  AC_SUBST(SDL_CFLAGS)
  AC_SUBST(SDL_LIBS)
  rm -f conf.sdltest
])

dnl worldforge check for libz, libpng, SDL and SDL_image
dnl the flags are returned in WF_SDL_IMAGE_CFLAGS and WF_SDL_IMAGE_LIBS
AC_DEFUN(WF_CHECK_SDL_IMAGE,[dnl

AC_LANG_PUSH([C])

dnl save CFLAGS and LIBS, so we can munge them when calling tests

WF_SDL_IMAGE_TMP_CFLAGS="$CFLAGS"
WF_SDL_IMAGE_TMP_LIBS="$LIBS"
WF_SDL_IMAGE_TMP_LDFLAGS="$LDFLAGS"
LDFLAGS="$LDFLAGS $LIBS"
LIBS=""

dnl check for libpng prefix option
AC_ARG_WITH(png-prefix,[  --with-png-prefix=PFX   Prefix where libpng is installed (option
al)],[dnl
	WF_SDL_IMAGE_CFLAGS="$WF_SDL_IMAGE_CFLAGS -I$withval/include"
	WF_SDL_IMAGE_LIBS="$WF_SDL_IMAGE_LIBS -L$withval/lib"
],[])

dnl check for libz prefix configure option
AC_ARG_WITH(z-prefix,[  --with-z-prefix=PFX     Prefix where libz is installed (optional)]
,[dnl
	WF_SDL_IMAGE_CFLAGS="$WF_SDL_IMAGE_CFLAGS -I$withval/include"
	WF_SDL_IMAGE_LIBS="$WF_SDL_IMAGE_LIBS -L$withval/lib"
],[])

CFLAGS="$CFLAGS $WF_SDL_IMAGE_CFLAGS"
LDFLAGS="$LDFLAGS $WF_SDL_IMAGE_LIBS"

dnl FIXME AC_CHECK_LIB() doesn't put -lfoo after the -L args
dnl we've inserted in LIBS, this may break on some platforms.

dnl check for libpng and libz

AC_CHECK_HEADER(zlib.h,[],AC_MSG_ERROR( You need libz headers for SDL_image!))
LIBS="-lz $LIBS"
AC_MSG_CHECKING(for libz)
AC_TRY_LINK([
		#include <zlib.h>
	],[
		int main()
		{
			zlibVersion();
			return 0;
		}
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR( You need libz for SDL_image!)
	])


AC_CHECK_HEADER(png.h,[],[AC_MSG_ERROR( You need libpng headers for SDL_image!)])
LIBS="-lpng $LIBS"
AC_MSG_CHECKING(for libpng)
AC_TRY_LINK([
		#include <png.h>
	],[
		int main()
		{
			png_access_version_number();
			return 0;
		}
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR( You need libpng for SDL_image!)
	])

WF_SDL_IMAGE_LIBS="$WF_SDL_IMAGE_LIBS -lpng -lz"

dnl Check for SDL and SDL_image

AM_PATH_SDL($1,,AC_MSG_ERROR([Couldn't find libSDL $1 or greater]))

dnl We pull in SDL_LIBS twice, to get the -L args before
dnl SDL_LIBS, but the -l args after as well. We pull in
dnl -lSDL_image here as well, so we can test for it
dnl in the next check.
WF_SDL_IMAGE_CFLAGS="$SDL_CFLAGS $WF_SDL_IMAGE_CFLAGS"
WF_SDL_IMAGE_LIBS="$SDL_LIBS -lSDL_image $SDL_LIBS $WF_SDL_IMAGE_LIBS"

CFLAGS="$WF_SDL_IMAGE_TMP_CFLAGS $WF_SDL_IMAGE_CFLAGS"
LIBS="$WF_SDL_IMAGE_TMP_LIBS $WF_SDL_IMAGE_LIBS"
LDFLAGS="$WF_SDL_IMAGE_TMP_LDFLAGS"

AC_CHECK_HEADER(SDL/SDL_image.h,[],[AC_MSG_ERROR(Missing SDL_image.h)])

dnl Use AC_TRY_LINK for a custom SDL_image check
AC_MSG_CHECKING(for SDL_image)
AC_TRY_LINK([
		#include <SDL/SDL_image.h>
		// use our own main(), so SDL_main links properly
		#if 0
	],[
		#endif
		int main(int argc, char** argv) {
		IMG_Load(0);
	],[
		AC_MSG_RESULT(yes)
	],[
		AC_MSG_RESULT(no)
		AC_MSG_ERROR(Couldn't link libSDL_image)
	])

dnl Got it, now clean up

CFLAGS="$WF_SDL_IMAGE_TMP_CFLAGS"
LIBS="$WF_SDL_IMAGE_TMP_LIBS"
LDFLAGS="$WF_SDL_IMAGE_TMP_LDFLAGS"

AC_LANG_POP([C])

])dnl end of WF_CHECK_SDL_IMAGE
