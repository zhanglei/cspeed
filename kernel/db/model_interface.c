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

#include "kernel/db/model_interface.h"

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_select, 0, 0, 1)
    ZEND_ARG_INFO(0, fields)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_from, 0, 0, 1)
    ZEND_ARG_INFO(0, table)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_where, 0, 0, 1)
    ZEND_ARG_INFO(0, where_conditions)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_group_by, 0, 0, 1)
    ZEND_ARG_INFO(0, group_conditions)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_having, 0, 0, 1)
    ZEND_ARG_INFO(0, having_conditions)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_order_by, 0, 0, 1)
    ZEND_ARG_INFO(0, order_by_conditions)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_limit, 0, 0, 1)
    ZEND_ARG_INFO(0, limit_conditions)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_create_command, 0, 0, 1)
    ZEND_ARG_INFO(0, raw_sql)
    ZEND_ARG_INFO(0, bind_params)
ZEND_END_ARG_INFO();

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_execute, 0, 0, 0)
ZEND_END_ARG_INFO();

/*}}}*/

CSPEED_METHOD(ModelInterface, select) /*{{{ proto ModelInterface::select() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, from) /*{{{ proto ModelInterface::from() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, where) /*{{{ proto ModelInterface::where() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, groupBy) /*{{{ proto ModelInterface::groupBy() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, having) /*{{{ proto ModelInterface::having() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, orderBy) /*{{{ proto ModelInterface::orderBy() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, limit) /*{{{ proto ModelInterface::limit() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, createCommand) /*{{{ proto ModelInterface::createCommand() */
{

}/*}}}*/

CSPEED_METHOD(ModelInterface, execute) /*{{{ proto ModelInterface::execute() */
{

}/*}}}*/

static const zend_function_entry cspeed_model_functions[] = { /*{{{*/
    ZEND_ABSTRACT_ME(ModelInterface, select, arginfo_cspeed_model_select)
    ZEND_ABSTRACT_ME(ModelInterface, from, arginfo_cspeed_model_from)
    ZEND_ABSTRACT_ME(ModelInterface, where, arginfo_cspeed_model_where)
    ZEND_ABSTRACT_ME(ModelInterface, groupBy, arginfo_cspeed_model_group_by)
    ZEND_ABSTRACT_ME(ModelInterface, having, arginfo_cspeed_model_having)
    ZEND_ABSTRACT_ME(ModelInterface, orderBy, arginfo_cspeed_model_order_by)
    ZEND_ABSTRACT_ME(ModelInterface, limit, arginfo_cspeed_model_limit)
    ZEND_ABSTRACT_ME(ModelInterface, createCommand, arginfo_cspeed_model_create_command)
    ZEND_ABSTRACT_ME(ModelInterface, execute, arginfo_cspeed_model_execute)
    PHP_FE_END
};/*}}}*/

CSPEED_INIT(model_interface) /*{{{*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\db", "ModelInterface", cspeed_model_functions);
    cspeed_model_interface_ce = zend_register_internal_interface(&ce);
}/*}}}*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
