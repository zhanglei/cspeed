dnl $Id$
dnl config.m4 for extension cspeed

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_WITH(cspeed, for cspeed support,
Make sure that the comment is aligned:
[  --with-cspeed             Include cspeed support])

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

  PHP_NEW_EXTENSION(cspeed,
    classes.c                           \
    cspeed.c                            \
    kernel/CApp.c                       \
    kernel/bootinit.c                   \
    kernel/object_factory.c             \
    kernel/di/di.c                      \
    kernel/db/pdo.c                     \
    kernel/mvc/view.c                   \
    kernel/net/request.c                \
    kernel/tool/helper.c                \
    kernel/mvc/dispatch.c               \
    kernel/tool/require.c               \
    kernel/tool/component.c             \
    kernel/net/response.c               \
    kernel/mvc/model.c                  \
    kernel/mvc/controller.c             \
    kernel/db/model_interface.c         \
    kernel/tool/configs.c               \
    kernel/rpc/server.c                 \
    kernel/rpc/client.c                 \
    kernel/mvc/router.c                 \
    kernel/console/task.c               \
    kernel/console/parse.c              \
    kernel/db/adapter.c,
  $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/db])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/di])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/rpc])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/mvc])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/net])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/tool])
  PHP_ADD_BUILD_DIR([$ext_builddir/kernel/console])

  PHP_SUBST(CURL_SHARED_LIBADD)
fi
