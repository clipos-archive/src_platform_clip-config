# SPDX-License-Identifier: LGPL-2.1-or-later
# Copyright © 2009-2018 ANSSI. All Rights Reserved.
dnl Process this file with autoconf to produce a configure script.
AC_INIT(src/clip-config.cpp)

dnl Checks for programs.
AC_PROG_CXX

[
if test "x$GCC" = "xyes"; then
  CFLAGS="$CFLAGS -Wall -Wextra \
	-Wstrict-prototypes -Wmissing-prototypes \
	-Wcast-qual -Wcast-align -Wpointer-arith \
	-Wnested-externs"
fi
]

AC_ARG_WITH(netconfd, 
	AC_HELP_STRING([--with-netconfd=path], 
		[Path to the netconf.d parent directory (default: /etc/admin/netconf.d)]),
		[netconfd_path=$withval],
		[netconfd_path=/etc/admin/netconf.d])
AC_SUBST(netconfd_path)
AC_MSG_CHECKING(path to the netconf.d parent directory)
AC_MSG_RESULT($netconfd_path)

AC_ARG_WITH(netconf, 
	AC_HELP_STRING([--with-netconf=path], 
		[Path to the netconf link to the profile currently activated (default: /etc/admin/conf.d/netconf)]),
		[netconf_link=$withval],
		[netconf_link=/etc/admin/conf.d/netconf])
AC_SUBST(netconf_link)
AC_MSG_CHECKING(path to the netconf link)
AC_MSG_RESULT($netconf_link)

AC_ARG_WITH(netd-socket, 
	AC_HELP_STRING([--with-netd-socket=path], 
		[Path to the netd socket (default: /var/run/netd)]),
		[netd_socket=$withval],
		[netd_socket=/var/run/netd])
AC_SUBST(netd_socket)
AC_MSG_CHECKING(path to the netd socket)
AC_MSG_RESULT($netd_socket)

AC_ARG_WITH(haswifi-flag, 
	AC_HELP_STRING([--with-haswifi-flag=path], 
		[Path to the file indicating the presence of a wifi interface (default: /var/run/has_wifi)]),
		[haswifi_flag=$withval],
		[haswifi_flag=/var/run/has_wifi])
AC_SUBST(haswifi_flag)
AC_MSG_CHECKING(path to the file indicating the presence of a wifi interface)
AC_MSG_RESULT($haswifi_flag)

AC_ARG_WITH(default-profile, 
	AC_HELP_STRING([--with-default-profile=path], 
		[Name of the default profile (default: default)]),
		[default_profilename=$withval],
		[default_profilename=default])
AC_SUBST(default_profilename)
AC_MSG_CHECKING(name of the default profile)
AC_MSG_RESULT($default_profilename)

AC_ARG_WITH(userd-socket, 
	AC_HELP_STRING([--with-userd-socket=path],
		[Path to the userd socket (default: /var/run/userd)]),
		[userd_socket=$withval],
		[userd_socket=/var/run/userd])
AC_SUBST(userd_socket)
AC_MSG_CHECKING(path to the userd socket)
AC_MSG_RESULT($userd_socket)

dnl Check for dependencies
if test -z "$PKG_CONFIG"; then
  PKG_CONFIG=pkg-config
fi
AC_SUBST(PKG_CONFIG)

AC_MSG_CHECKING(that pkg-config runs)
if "$PKG_CONFIG" --version > /dev/null 2>&1 ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  AC_MSG_ERROR([*** $PKG_CONFIG missing ***

\'$PKG_CONFIG --version\' failed to run - make sure it is inside one of the
directories in your PATH environment variable!

pkg-config comes with the developement packages for GTK+-2.2, available at:

    http://www.gtk.org])
fi

AC_DEFUN([BUILD_DEPENDS], [
AC_MSG_CHECKING(that $1 (version >= $2) is installed)
if "$PKG_CONFIG" --atleast-version=$2 $1 ; then
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
  if "$PKG_CONFIG" --exists $1 ; then
    ERRSTR=`"$PKG_CONFIG"  --modversion $1`
    AC_MSG_ERROR(Current version is only $ERRSTR)
  else
    AC_MSG_ERROR(Package is not installed)
  fi
fi
])

BUILD_DEPENDS(clip, 1.0.9)
BUILD_DEPENDS(ClipConfig, 2.0.0)
BUILD_DEPENDS(ClipWidgets, 2.0.0)
dnl TODO
dnl BUILD_DEPENDS(userdc, 1.0.0)

dnl Checks for header files.
AC_HEADER_STDC
dnl AC_CHECK_HEADERS(userd/userd.h)
AC_CHECK_HEADERS(clip/clip.h)


dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

dnl Checks for QT lib
PKG_CHECK_MODULES(QT4_CORE, QtCore,,
    AC_MSG_ERROR([[
***
*** Qt4 Core is required.
***]])
)

PKG_CHECK_MODULES(QT4_GUI, QtGui,,
    AC_MSG_ERROR([[
***
*** Qt4 Gui is required.
***]])
)

AC_OUTPUT(Makefile src/Makefile scripts/Makefile)
