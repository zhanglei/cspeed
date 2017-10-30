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
#include "kernel/tool/helper.h"

/*{{{ All ARGINFO FOR CLASS Router */
ZEND_BEGIN_ARG_INFO_EX(arginfo_router_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_router_add, 0, 0, 2)
    ZEND_ARG_INFO(0, url)
    ZEND_ARG_INFO(0, real_url)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_router_add_from_ini, 0, 0, 1)
    ZEND_ARG_INFO(0, ini_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_router_add_from_array, 0, 0, 1)
    ZEND_ARG_INFO(0, array)
ZEND_END_ARG_INFO()
/*}}}*/

void initialise_router_object_properties(zval *router_object)   /*{{{ Initialise the Router object properties */
{
    zval all_routines;
    array_init(&all_routines);
    zend_update_property(cspeed_router_ce, router_object, CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), &all_routines);
}/*}}}*/

CSPEED_METHOD(Router, __construct)/*{{{ proto Router::__construct()*/
{
    initialise_router_object_properties(getThis());
}/*}}}*/

void preg_faker_routine_rule(zend_string *url, zval *retval)  /*{{{ Change the URL to the real pattern */
{
    zval faker_pattern, real_pattern;
    array_init(&faker_pattern);
    add_next_index_string(&faker_pattern, "#:action:#");
    add_next_index_string(&faker_pattern, "#:any:#");
    add_next_index_string(&faker_pattern, "#:controller:#");
    add_next_index_string(&faker_pattern, "#:id:#");
    add_next_index_string(&faker_pattern, "#:module:#");

    array_init(&real_pattern);
    add_next_index_string(&real_pattern, "([^0-9-][a-zA-Z0-9-]*)");
    add_next_index_string(&real_pattern, "([^/]+)");
    add_next_index_string(&real_pattern, "([^0-9-][a-zA-Z0-9-]*)");
    add_next_index_string(&real_pattern, "([0-9]+)");
    add_next_index_string(&real_pattern, "([^0-9-][a-zA-Z0-9-]*)");

    /* preg_replace */
    zval subject;
    ZVAL_STRING(&subject, ZSTR_VAL(strpprintf(0, "#%s#", ZSTR_VAL(url))));

    zval preg_function_name;
    ZVAL_STRING(&preg_function_name, "preg_replace");
    zval params[] = { faker_pattern, real_pattern, subject };
    uint32_t param_count = 3;
    call_user_function(CG(function_table), NULL, &preg_function_name, retval, param_count, params);
}/*}}}*/

CSPEED_METHOD(Router, add)/*{{{ proto Router::add($url, $matcher)*/
{
    zend_string *url, *real_url;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &url, &real_url) == FAILURE){
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(url))){
        zval real_subject;
        preg_faker_routine_rule(url, &real_subject);
        zval *all_routines = zend_read_property(cspeed_router_ce, getThis(), CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), 1, NULL);
        add_assoc_str(all_routines, Z_STRVAL(real_subject), real_url);
        RETURN_TRUE
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(Router, addFromIni)   /*{{{ proto Router::addFromIni($ini_file)*/
{
    zend_string *ini_file_path;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &ini_file_path) == FAILURE) {
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(ini_file_path))) {
        zval routers_from_ini;
        char path[MAXPATHLEN];
        cspeed_get_cwd(path);
        zend_string *real_ini_file_path = strpprintf(0, "%s/%s", path, ZSTR_VAL(ini_file_path));
        if (check_file_exists( ZSTR_VAL(real_ini_file_path) )) {
            /* Do the parsing */
            cspeed_parse_ini_file( ZSTR_VAL(real_ini_file_path) , NULL, NULL, 1, &routers_from_ini);
            /* *** */
            if ( ZVAL_IS_NULL(&routers_from_ini) && (Z_TYPE(routers_from_ini) != IS_ARRAY ) ){
                php_error_docref(NULL, E_ERROR, "File : `%s` not valid.",  ZSTR_VAL(real_ini_file_path));
                return ;
            }
            /* *** */
            zval *all_routines = zend_read_property(cspeed_router_ce, getThis(), CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), 1, NULL);
            
            zend_string *pcre_url;zval *value;
            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(routers_from_ini), pcre_url, value){
                if ( CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(pcre_url)) && (Z_TYPE_P(value) == IS_STRING) ){
                    zval temp_real_url;
                    preg_faker_routine_rule(pcre_url, &temp_real_url);
                    Z_TRY_ADDREF_P(value);
                    add_assoc_str(all_routines, Z_STRVAL(temp_real_url), Z_STR_P(value) );
                }
            } ZEND_HASH_FOREACH_END();
        } else {
            php_error_docref(NULL, E_ERROR, "File: `%s` not exists.",  ZSTR_VAL(real_ini_file_path));
            return ;
        }
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter must be a valid file string path.");
        return ;
    }
}/*}}}*/

CSPEED_METHOD(Router, addFromArray)/*{{{ proto Router::addFromArray()*/
{
    zval *config_array;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &config_array) == FAILURE) {
        return ;
    }
    if (zend_hash_num_elements(Z_ARRVAL_P(config_array))) {
        zval *all_routines = zend_read_property(cspeed_router_ce, getThis(), CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), 1, NULL);
        zend_string *pcre_url; zval *pcre_value;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(config_array), pcre_url, pcre_value){
            if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(pcre_url)) && (Z_TYPE_P(pcre_value) == IS_STRING) ) {
                zval temp_real_url;
                preg_faker_routine_rule(pcre_url, &temp_real_url);
                Z_TRY_ADDREF_P(pcre_value);
                add_assoc_str(all_routines, Z_STRVAL(temp_real_url), Z_STR_P(pcre_value));
            }
        } ZEND_HASH_FOREACH_END();
        /*zend_hash_merge(Z_ARRVAL_P(all_routines), Z_ARRVAL_P(config_array), (copy_ctor_func_t) zval_add_ref, 0);*/
    }
}/*}}}*/

static const zend_function_entry cspeed_router_functions[] = {  /*{{{ All methods for the class Router*/
    CSPEED_ME(Router, __construct, arginfo_router_construct,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Router, add,         arginfo_router_add,              ZEND_ACC_PUBLIC)
    CSPEED_ME(Router, addFromIni,  arginfo_router_add_from_ini,     ZEND_ACC_PUBLIC)
    CSPEED_ME(Router, addFromArray,arginfo_router_add_from_array,   ZEND_ACC_PUBLIC)

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