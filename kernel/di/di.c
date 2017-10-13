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

#include "kernel/di/di.h"

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_di_set, 0, 0, 2)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

/*}}}*/

CSPEED_METHOD(Di, set) /*{{{ proto Di::set($key, function(){return new stdClass();}) */
{

}/*}}}*/


static const zend_function_entry cspeed_di_functions[] = { /*{{{ All methods of the class Di */
    CSPEED_ME(Di, set, arginfo_cspeed_di_set, ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(di) /*{{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\di", "Di", cspeed_di_functions);
    cspeed_di_ce = zend_register_internal_class(&ce);
}/*}}}*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
