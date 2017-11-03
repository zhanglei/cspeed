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
  if test -r $PHP_CURL/include/curl/easy.h; then
    CURL_DIR=$PHP_CURL
  else
    AC_MSG_CHECKING(for cURL in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/curl/easy.h; then
        CURL_DIR=$i
        AC_MSG_RESULT(found in $i)
        break
      fi
    done
  fi
  if test -z "$CURL_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libcurl distribution -
    easy.h should be in <curl-dir>/include/curl/)
  fi
  CURL_CONFIG="curl-config"
  AC_MSG_CHECKING(for cURL 7.10.5 or greater)

  if ${CURL_DIR}/bin/curl-config --libs > /dev/null 2>&1; then
    CURL_CONFIG=${CURL_DIR}/bin/curl-config
  else
    if ${CURL_DIR}/curl-config --libs > /dev/null 2>&1; then
      CURL_CONFIG=${CURL_DIR}/curl-config
    fi
  fi
  curl_version_full=`$CURL_CONFIG --version`
  curl_version=`echo ${curl_version_full} | sed -e 's/libcurl //' | $AWK 'BEGIN { FS = "."; } { printf "%d", ($1 * 1000 + $2) * 1000 + $3;}'`
  if test "$curl_version" -ge 7010005; then
    AC_MSG_RESULT($curl_version_full)
    CURL_LIBS=`$CURL_CONFIG --libs`
  else
    AC_MSG_ERROR(cURL version 7.10.5 or later is required to compile php with cURL support)
  fi

  PHP_ADD_INCLUDE($CURL_DIR/include)
  PHP_EVAL_LIBLINE($CURL_LIBS, CURL_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(curl, $CURL_DIR/$PHP_LIBDIR, CURL_SHARED_LIBADD)

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
    cspeed.c         \
    kernel/CApp.c     \
    kernel/di/di.c     \
    kernel/db/pdo.c     \
    kernel/mvc/view.c    \
    kernel/net/request.c  \
    kernel/tool/helper.c   \
    kernel/mvc/dispatch.c   \
    kernel/tool/require.c    \
    kernel/net/response.c     \
    kernel/mvc/model.c         \
    kernel/mvc/controller.c     \
    kernel/db/model_interface.c  \
    kernel/tool/configs.c         \
    kernel/rpc/server.c            \
    kernel/rpc/client.c             \
    kernel/bootstrap.c               \
    kernel/mvc/router.c               \
    kernel/db/adapter.c,
  $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/db])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/di])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/rpc])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/mvc])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/net])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/tool])

  PHP_SUBST(CURL_SHARED_LIBADD)
fi
