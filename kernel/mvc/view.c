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

#include "kernel/mvc/view.h"
#include "kernel/tool/helper.h"
#include "kernel/tool/require.h"

void render_view_file(zval *view_obj, zend_string *temp_file, zval *array_variables, zval *ret_val)
{
    /* Fix the ZTS path problem */
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);

    zval *view_dir  = zend_read_property(cspeed_view_ce, view_obj, CSPEED_STRL(CSPEED_VIEW_DIRS), 1, NULL);
    zval *suffix    = zend_read_property(cspeed_view_ce, view_obj, CSPEED_STRL(CSPEED_VIEW_SUFFIX), 1, NULL);
    
    zend_string *real_path_file = strpprintf(0, 
        "%s/%s/modules/%s/%s/%s/%s.%s", 
        path, 
        ZSTR_VAL(CSPEED_G(core_application)), 
        ZSTR_VAL(CSPEED_G(core_router_default_module)), 
        Z_STRVAL_P(view_dir), 
        ZSTR_VAL(CSPEED_G(core_router_default_controller)),
        ZSTR_VAL(temp_file), ZSTR_VAL(CSPEED_G(core_view_ext))
    );
    
    zval *view_variables = zend_read_property(cspeed_view_ce, view_obj, CSPEED_STRL(CSPEED_VIEW_VARIABLES), 1, NULL);
    if (array_variables && ( Z_TYPE_P(view_variables) == IS_ARRAY ) ) {
        zend_string *var_name;
        zval *var_value;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array_variables), var_name, var_value){
            add_assoc_zval(view_variables, ZSTR_VAL(var_name), var_value);
        } ZEND_HASH_FOREACH_END();
    }
    check_file_exists(ZSTR_VAL(real_path_file));
    
    cspeed_require_file(ZSTR_VAL(real_path_file), view_variables, view_obj, ret_val);
}

void render_file(INTERNAL_FUNCTION_PARAMETERS, zval *ret_val, zval *view_obj)/*{{{ View::render() & View::getRender() & View::partial() */
{
    zend_string *temp_file;
    zval *array_variables = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|a", &temp_file, &array_variables) == FAILURE) {
        return ;
    }
    render_view_file(view_obj, temp_file, array_variables, ret_val);
}/*}}}*/

/* {{{ ARG-INFO */

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_render, 0, 0, 1)
    ZEND_ARG_INFO(0, template)
    ZEND_ARG_INFO(0, variables)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_partial, 0, 0, 1)
    ZEND_ARG_INFO(0, template)
    ZEND_ARG_INFO(0, variables)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_set_suffix, 0, 0, 1)
    ZEND_ARG_INFO(0, suffix)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_set_var, 0, 0, 2)
    ZEND_ARG_INFO(0, var_name)
    ZEND_ARG_INFO(0, var_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_get_render, 0, 0, 2)
    ZEND_ARG_INFO(0, template)
    ZEND_ARG_INFO(0, variables)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_set_view_dir, 0, 0, 1)
    ZEND_ARG_INFO(0, view_dir)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_view_get, 0, 0, 1)
    ZEND_ARG_INFO(0, property_name)
ZEND_END_ARG_INFO()


/*}}}*/

void initialise_view_object_properties(zval *view_object)
{
    zval view_variables;
    array_init(&view_variables);
    zend_update_property(cspeed_view_ce, view_object, CSPEED_STRL(CSPEED_VIEW_VARIABLES), &view_variables);
    zval_ptr_dtor(&view_variables);
}

CSPEED_METHOD(View, __construct)    /*{{{ proto View::__construct() */
{
    initialise_view_object_properties(getThis());
}/*}}}*/

CSPEED_METHOD(View, __get)
{
    zend_string *property_name;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &property_name) == FAILURE ){
        return ;
    }

    if ( CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(property_name)) ) {
        if ( (strncmp(ZSTR_VAL(property_name), ("module_id"), strlen("module_id")) == 0) ) {
            RETURN_STR(CSPEED_G(core_router_default_module));
        } else if ( (strncmp(ZSTR_VAL(property_name), ("controller_id"), strlen("controller_id")) == 0) ) {
            RETURN_STR(CSPEED_G(core_router_default_controller));
        } else if ( (strncmp(ZSTR_VAL(property_name), ("action_id"), strlen("action_id")) == 0) ) {
            RETURN_STR(CSPEED_G(core_router_default_action));
        } else {
            HashTable *properties = Z_OBJPROP_P(getThis());
            if ( properties && zend_hash_num_elements(properties) ) {
                zval *result = zend_hash_find(properties, property_name);
                if ( result && !ZVAL_IS_NULL(result) ) {
                    RETURN_ZVAL(result, 1, NULL);
                }
                RETURN_NULL();
            }
        }
    }
    php_error_docref(NULL, E_ERROR, "__get need a valid string index.");
}

CSPEED_METHOD(View, render)         /*{{{ proto View::render($file, $variables) */
{
    render_file(INTERNAL_FUNCTION_PARAM_PASSTHRU, NULL, getThis());

}/*}}}*/

CSPEED_METHOD(View, partial)         /*{{{ proto View::partial($file, $variables) */
{
    zend_string *temp_file;
    zval *array_variables = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|a", &temp_file, &array_variables) == FAILURE) {
        return ;
    }
    /* Fix the ZTS path problem */
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);

    zval *suffix    = zend_read_property(cspeed_view_ce, getThis(), CSPEED_STRL(CSPEED_VIEW_SUFFIX), 1, NULL);
    
    zend_string *real_path_file = strpprintf(0, 
        "%s/%s/views/%s.%s", 
        path, 
        ZSTR_VAL(CSPEED_G(core_application)), 
        ZSTR_VAL(temp_file), 
        ZSTR_VAL(CSPEED_G(core_view_ext))
    );
    
    zval *view_variables = zend_read_property(cspeed_view_ce, getThis(), CSPEED_STRL(CSPEED_VIEW_VARIABLES), 1, NULL);
    if (array_variables && ( Z_TYPE_P(view_variables) == IS_ARRAY ) ) {
        zend_string *var_name;
        zval *var_value;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array_variables), var_name, var_value){
            add_assoc_zval(view_variables, ZSTR_VAL(var_name), var_value);
        } ZEND_HASH_FOREACH_END();
    }
    check_file_exists(ZSTR_VAL(real_path_file));
    
    cspeed_require_file(ZSTR_VAL(real_path_file), view_variables, getThis(), NULL);
}/*}}}*/

CSPEED_METHOD(View, getRender)      /*{{{ proto View::getRender($file, $variables)*/
{
    zval retval;
    render_file(INTERNAL_FUNCTION_PARAM_PASSTHRU, &retval, getThis());
    RETURN_ZVAL(&retval, 1, 0);
}/*}}}*/

CSPEED_METHOD(View, setSuffix)      /*{{{ proto View::setSuffix($suffix) */
{
    zend_string *suffix;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &suffix) == FAILURE) {
        return ;
    }
    zend_update_property_str(cspeed_view_ce, getThis(), CSPEED_STRL(CSPEED_VIEW_SUFFIX), suffix);
}/*}}}*/

CSPEED_METHOD(View, setVar)         /*{{{ proto View::setVar($var_name, $var_value) */
{
    zend_string *var_name;
    zval *var_value;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sz", &var_name, &var_value) == FAILURE) {
        return ;
    }
    zval *view_variables = zend_read_property(cspeed_view_ce, getThis(), CSPEED_STRL(CSPEED_VIEW_VARIABLES), 1, NULL);
    add_assoc_zval(view_variables, ZSTR_VAL(var_name), var_value );
}/*}}}*/

CSPEED_METHOD(View, setViewDir)         /*{{{ proto View::setViewDir($dirname) */
{
    zend_string *view_dir;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &view_dir) == FAILURE) {
      return ;
    }
    zend_update_property_str(cspeed_view_ce, getThis(), CSPEED_STRL(CSPEED_VIEW_DIRS), view_dir);
}/*}}}*/

static const zend_function_entry cspeed_view_functions[] = { /*{{{ All methods the View class had */
    CSPEED_ME(View, __construct,    arginfo_view_construct,        ZEND_ACC_PUBLIC)
    CSPEED_ME(View, render,         arginfo_view_render,           ZEND_ACC_PUBLIC)
    CSPEED_ME(View, setSuffix,      arginfo_view_set_suffix,       ZEND_ACC_PUBLIC)
    CSPEED_ME(View, setVar,         arginfo_view_set_var,          ZEND_ACC_PUBLIC)
    CSPEED_ME(View, getRender,      arginfo_view_get_render,       ZEND_ACC_PUBLIC)
    CSPEED_ME(View, setViewDir,     arginfo_view_set_view_dir,     ZEND_ACC_PUBLIC)
    CSPEED_ME(View, partial,        arginfo_view_partial,          ZEND_ACC_PUBLIC)
    CSPEED_ME(View, __get,          arginfo_view_get,              ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(view) /*{{{ Initialise function to initialise the view components */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\mvc", "View", cspeed_view_functions);
    cspeed_view_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(cspeed_view_ce,   CSPEED_STRL(CSPEED_VIEW_VARIABLES), ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_view_ce, CSPEED_STRL(CSPEED_VIEW_SUFFIX), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_view_ce, CSPEED_STRL(CSPEED_VIEW_DIRS), CSPEED_VIEW_DIRS_V, ZEND_ACC_PROTECTED);
}/*}}}*/










/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
