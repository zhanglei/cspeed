dnl $Id$
dnl config.m4 for extension cspeed

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(cspeed, for cspeed support,
Make sure that the comment is aligned:
[  --with-cspeed             Include cspeed support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(cspeed, whether to enable cspeed support,
dnl Make sure that the comment is aligned:
dnl [  --enable-cspeed           Enable cspeed support])

if test "$PHP_CSPEED" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-cspeed -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/cspeed.h"  # you most likely want to change this
  dnl if test -r $PHP_CSPEED/$SEARCH_FOR; then # path given as parameter
  dnl   CSPEED_DIR=$PHP_CSPEED
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for cspeed files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       CSPEED_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$CSPEED_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the cspeed distribution])
  dnl fi

  dnl # --with-cspeed -> add include path
  dnl PHP_ADD_INCLUDE($CSPEED_DIR/include)

  dnl # --with-cspeed -> check for lib and symbol presence
  dnl LIBNAME=cspeed # you may want to change this
  dnl LIBSYMBOL=cspeed # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $CSPEED_DIR/$PHP_LIBDIR, CSPEED_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_CSPEEDLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong cspeed lib version or lib not found])
  dnl ],[
  dnl   -L$CSPEED_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(CSPEED_SHARED_LIBADD)

  PHP_NEW_EXTENSION(cspeed,
    cspeed.c \
    kernel/CApp.c \
    kernel/di/di.c  \
    kernel/db/pdo.c  \
    kernel/db/mysql.c  \
    kernel/mvc/view.c \
    kernel/net/request.c \
    kernel/tool/helper.c \
    kernel/mvc/dispatch.c \
    kernel/tool/require.c \
    kernel/net/response.c  \
    kernel/mvc/controller.c \
    kernel/db/model_interface.c,
  $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/db])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/di])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/mvc])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/net])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/tool])
fi
