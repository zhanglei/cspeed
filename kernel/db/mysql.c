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

#include "kernel/db/mysql.h"

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(MySql, __construct)/*{{{ proto MySql::__construct(array $options = [])*/
{

}/*}}}*/

static const zend_function_entry cspeed_mysql_functions[] = { /*{{{*/
    CSPEED_ME(MySql, __construct, arginfo_cspeed_mysql_construct, ZEND_ACC_PUBLIC)
    PHP_FE_END
};/*}}}*/

CSPEED_INIT(mysql)/*{{{*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\db\\pdo", "MySql", cspeed_mysql_functions)
    cspeed_mysql_ce = zend_register_internal_class(&ce);
}/*}}}*/