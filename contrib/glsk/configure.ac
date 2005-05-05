#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(glsk,[0.1.0],[no-emal@nohost.net])

GLSKCORE_RELEASE=0.1.0
GLSKCORE_LIBRARY_VERSION=0:1:0
AC_SUBST(GLSKCORE_RELEASE)
AC_SUBST(GLSKCORE_LIBRARY_VERSION)

GLSKMM_RELEASE=0.1.0
GLSKMM_LIBRARY_VERSION=0:1:0
AC_SUBST(GLSKMM_RELEASE)
AC_SUBST(GLSKMM_LIBRARY_VERSION)


AC_CANONICAL_HOST
AC_CANONICAL_TARGET
## AC_CANONICAL_SYSTEM

AM_INIT_AUTOMAKE(glsk,[0.1.0])

AC_CONFIG_SRCDIR([include/glsk/glskmm.h])
AC_CONFIG_HEADER([config.h])


on_darwin=""
on_windows=""
case "${target}" in
  *-apple-darwin*)
    ## test darwin and carbon .. or just set values?
    on_darwin="yes"  
    AC_DEFINE([DARWIN_SYSTEM],[1],["is defined when building for darwin"])
    ;;
  *cygwin*)
    on_cygwin="yes" 
    ## building for X11 .. but make special tests .. 
    ;;
  *mingw32*)
    on_windows="yes" 
    AC_DEFINE([WINDOWS_SYSTEM],[1],["is defined when building for windows"])
    ;;
esac

if test "$on_windows" != "yes" -a "$on_darwin" != "yes" ; 
then
  ## test unix locations for -lGL GLU ... 
  ## allow framebuffer option? ... 
  echo "bla"
fi


AM_CONDITIONAL(DARWIN,[test "$on_darwin" = "yes"])
AM_CONDITIONAL(WINDOWS,[test "$on_windows" = "yes"])
AM_CONDITIONAL(NORMAL,[test "$on_windows" != "yes" -a "$on_darwin" != "yes"])


AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CHECK_TOOL(GPERF,gperf,,)
# Checks for libraries.

# Checks for header files.
AC_HEADER_STDBOOL

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_FUNC_MALLOC
AC_TYPE_SIZE_T


# Checks for library functions.
AC_HEADER_STDC
AC_CHECK_FUNCS([memset])

MDL_HAVE_OPENGL
PKG_CHECK_MODULES(SIGC,[sigc++-2.0],[],[])

AC_CONFIG_FILES([Makefile
                 source/Makefile
                 source/glsk/Makefile
                 source/glskcore/Makefile
                 source/glskmm/Makefile
                 include/glsk/Makefile
                 include/Makefile
                 examples/Makefile
                 examples/glsk-test/Makefile
                 examples/glskmm-test/Makefile
                 examples/gui-test/Makefile
                 ])
AC_OUTPUT
