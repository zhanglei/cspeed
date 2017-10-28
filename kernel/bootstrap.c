/*
  +----------------------------------------------------------------------+
  | CSpeed framework                                                     |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:Josin<774542602@qq.com|www.supjos.cn>                         |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cspeed.h"

#include "bootstrap.h"

/*{{{ All ARG_INFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_bootstrap_construct, 0, 0, 0)
ZEND_END_ARG_INFO()
/*}}}*/

static const zend_function_entry cspeed_bootstrap_functions[] = { /*{{{ All methods of the interface */

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(bootstrap)  /*{{{ The initialise module function */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs", "Bootstrap", NULL);
    cspeed_bootstrap_ce = zend_register_internal_interface(&ce);
}/*}}}*/