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
