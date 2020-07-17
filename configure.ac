AC_PREREQ([2.68])
AC_INIT([libCamiProdRead], [1.0.0], [emmanuel.paviot-adet@parisdescartes.fr])

m4_esyscmd([./bin/generate-files])

AM_INIT_AUTOMAKE([foreign subdir-objects])

AC_CONFIG_SRCDIR([src/IO.cpp])

AC_PROG_CXX
AC_PROG_RANLIB

AC_CONFIG_SUBDIRS(resources/tinyxml2-master)

#CPPFLAGS="-Iresources/tinyxml2-master $CPPFLAGS"
#LDFLAGS="-Lresources/tinyxml2-master $LDFLAGS"

#AC_CHECK_LIB(tinyxml2,tinyxml2::myVisitor::VisitEnter)

AC_CHECK_HEADER_STDBOOL
AC_FUNC_MALLOC

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
