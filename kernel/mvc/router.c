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

#include "kernel/mvc/router.h"

/*{{{ All ARGINFO FOR CLASS Router */
ZEND_BEGIN_ARG_INFO_EX(arginfo_router_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_router_add, 0, 0, 2)
    ZEND_ARG_INFO(0, url)
    ZEND_ARG_INFO(0, mvc)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(Router, __construct)/*{{{ proto Router::__construct()*/
{
    zval all_routines;
    array_init(&all_routines);
    zend_update_property(cspeed_router_ce, getThis(), CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), &all_routines);
}/*}}}*/

CSPEED_METHOD(Router, add)/*{{{ proto Router::add($url, $matcher)*/
{
    zend_string *url;
    zval *mvc;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sa", &url, &mvc) == FAILURE){
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(url))){
        /* Do the PCRE replace */
        zend_string *var_key;
        zval *var_value;
        zval faker_pattern, real_pattern;
        array_init(&faker_pattern);
        add_next_index_string(&faker_pattern, "#{module}#");
        add_next_index_string(&faker_pattern, "#{controller}#");
        add_next_index_string(&faker_pattern, "#{action}#");

        array_init(&real_pattern);
        add_next_index_string(&real_pattern, "([^0-9-][a-zA-Z0-9-]*)");
        add_next_index_string(&real_pattern, "([^0-9-][a-zA-Z0-9-]*)");
        add_next_index_string(&real_pattern, "([^0-9-][a-zA-Z0-9-]*)");

        /* preg_replace */
        zval subject;
        ZVAL_STRING(&subject, ZSTR_VAL(strpprintf(0, "#%s#", ZSTR_VAL(url))));

        zval real_subject, preg_function_name;
        ZVAL_STRING(&preg_function_name, "preg_replace");
        zval params[] = { faker_pattern, real_pattern, subject };
        uint32_t param_count = 3;
        call_user_function(CG(function_table), NULL, &preg_function_name, &real_subject, param_count, params);

        zval *all_routines = zend_read_property(cspeed_router_ce, getThis(), CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), 1, NULL);
        add_assoc_zval(all_routines, Z_STRVAL(real_subject), mvc);
        RETURN_TRUE
    }
    RETURN_FALSE
}/*}}}*/

static const zend_function_entry cspeed_router_functions[] = {  /*{{{ All methods for the class Router*/
    CSPEED_ME(Router, __construct, arginfo_router_construct,    ZEND_ACC_PUBLIC)
    CSPEED_ME(Router, add,         arginfo_router_add,          ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(router) /*{{{ proto void router_init() */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\mvc", "Router", cspeed_router_functions);
    cspeed_router_ce = zend_register_internal_class(&ce);

    /* ALL static properties */
    zend_declare_property_null(cspeed_router_ce, CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), ZEND_ACC_PUBLIC);
}/*}}}*/