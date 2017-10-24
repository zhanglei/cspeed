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

#include "kernel/CApp.h"
#include "kernel/net/request.h"

#include "kernel/tool/helper.h"
#include "kernel/tool/require.h"

#include "kernel/di/di.h"
#include "kernel/mvc/dispatch.h"

#include <string.h>

#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_string.h"

#include <unistd.h>         /* Access function */
#include <fcntl.h>          /* Access function */

int cspeed_deal_reqeust(zend_string *url, zend_fcall_info *zfi, zend_fcall_info_cache *zfic, zval *ret_val) /*{{{ Deal the REQUEST */
{
    char *path_info = cspeed_request_server_str_key_val("PATH_INFO");
    if (CSPEED_STRING_NOT_EMPTY(path_info) 
        || ( ( *(ZSTR_VAL(url)) == '/' ) && (ZSTR_LEN(url) == 1) ) ) {
        zend_string *pattern = strpprintf(0, "#%s#", ZSTR_VAL(url));

        /* The nested thing to get the preg mathces */
        zval nested_pattern, nested_pattern_string;
        array_init(&nested_pattern);
        add_next_index_string(&nested_pattern, "#{name}#");
        add_next_index_string(&nested_pattern, "#{id}#");

        array_init(&nested_pattern_string);
        add_next_index_string(&nested_pattern_string, "([^0-9][a-zA-Z0-9-]+)");
        add_next_index_string(&nested_pattern_string, "(\\d+)");

        zval url_pattern;
        ZVAL_STRING(&url_pattern, ZSTR_VAL(pattern));

        zval preg_replace_result;
        zval preg_function_name;
        ZVAL_STRING(&preg_function_name, "preg_replace");
        zval preg_params[] = {
            nested_pattern,
            nested_pattern_string,
            url_pattern
        };
        call_user_function(CG(function_table), NULL, &preg_function_name, &preg_replace_result, 3, preg_params);

        pcre_cache_entry *pce_regexp;
        if ((pce_regexp = pcre_get_compiled_regex_cache(Z_STR(preg_replace_result))) == NULL) {
            return -1;
        } else {
            zval matches, subparts;
            ZVAL_NULL(&subparts);
            php_pcre_match_impl(pce_regexp, CSPEED_STRL(path_info), &matches, &subparts, 0, 0, 0, 0);

            if (!zend_hash_num_elements(Z_ARRVAL(subparts))) {
                zval_ptr_dtor(&subparts);
                return -1;
            } else {
                zend_hash_index_del(Z_ARRVAL(subparts), 0);
                zfi->retval = ret_val;
                zfi->params = &subparts;
                zfi->param_count = 1;
                zend_call_function(zfi, zfic);

                zval_ptr_dtor(&subparts);
                return 0;
            }
        }
    } else {
        return -1;
    }
}/*}}}*/

void handle_request(INTERNAL_FUNCTION_PARAMETERS)/*{{{ Handle the user input from the PHP level*/
{
    zend_string *url;
    zend_fcall_info zfi;
    zend_fcall_info_cache zfic;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sf*", &url, &zfi, &zfic, 
        &zfi.params, &zfi.param_count) == FAILURE) {
        return ;
    }
    zval retval;
    int result = cspeed_deal_reqeust(url, &zfi, &zfic, &retval);
    if (result == 0) {
        RETURN_ZVAL(&retval, 1, 0);
    }
    RETURN_FALSE
} /*}}}*/

void handle_method_request(char *method_name, INTERNAL_FUNCTION_PARAMETERS)/*{{{ Handle the request method with the given method name */
{
    if (cspeed_request_is_method(method_name)) {
        handle_request(INTERNAL_FUNCTION_PARAM_PASSTHRU);   
    }
} /*}}}*/

void cspeed_app_load_file(zend_string *class_name_with_namespace, INTERNAL_FUNCTION_PARAMETERS, zval *app_obj) /*{{{ Load the file with the corresponding namespace */
{
    char *slash_pos = strchr(ZSTR_VAL(class_name_with_namespace), '\\');
    if (slash_pos == NULL) { /* No slash find */
        zend_string *real_file_path = strpprintf(0, "./%s", ZSTR_VAL(class_name_with_namespace));
        cspeed_require_file(ZSTR_VAL(real_file_path), NULL, NULL, NULL);
        zend_string_release(real_file_path);
    } else {                 /* find the slash */
        char *current_alias = (char *)malloc(sizeof(char) * (slash_pos - ZSTR_VAL(class_name_with_namespace) + 1));
        memset(current_alias, 0, (slash_pos - ZSTR_VAL(class_name_with_namespace) + 1));
        memcpy(current_alias, ZSTR_VAL(class_name_with_namespace), (slash_pos - ZSTR_VAL(class_name_with_namespace)));

        /* Before the steps the current_alias was `app` for example
            After we get the namespace alias, we can find the global aliases to find whether the alias is exists or not.
            if exists, replace it and plus the left character to form the correctly path
         */
        zval *all_app_aliases = zend_read_property(cspeed_app_ce, app_obj, CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 1, NULL);
        zval *has_exists = zend_hash_find(Z_ARRVAL_P(all_app_aliases), zend_string_init(CSPEED_STRL(current_alias), 0));
        if (has_exists) {   /* Exists the need alias */
            int real_file_path_size = (Z_STRLEN_P(has_exists) + ZSTR_LEN(class_name_with_namespace) - (slash_pos - ZSTR_VAL(class_name_with_namespace)) + 5);
            char *real_file_path = (char *)malloc(sizeof(char) * real_file_path_size); /* five means the .php & space*/
            memset(real_file_path, 0, real_file_path_size);

            strncat(real_file_path, Z_STRVAL_P(has_exists), Z_STRLEN_P(has_exists));
            strncat(real_file_path, ZSTR_VAL(class_name_with_namespace) + (slash_pos - ZSTR_VAL(class_name_with_namespace)), ZSTR_LEN(class_name_with_namespace) - (slash_pos - ZSTR_VAL(class_name_with_namespace)));
            strncat(real_file_path, ".php", strlen(".php"));
            /* Reslash all slash to reslash */
            cspeed_reverse_slash_char(real_file_path);
            /* check whether the file is exists or not */
            if (access(real_file_path, F_OK) == -1) {
                php_error_docref(NULL, E_ERROR, "File : %s not exists.", real_file_path);
                return ;
            }
            cspeed_require_file(real_file_path, NULL, NULL, NULL);
            free(current_alias);
            free(real_file_path);
        } else {            /* Not found the needing alias */
            php_error_docref(NULL, E_ERROR, "Namespace alias: %s not found. please set it first before use.", current_alias);
            free(current_alias);
            return ;
        }
    }
} /*}}}*/

/* ARGINFO FOR CLASS CApp */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_get, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_post, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_put, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_patch, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_delete, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_head, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_options, 0, 0, 2)
    ZEND_ARG_INFO(0, url_pattern)
    ZEND_ARG_INFO(0, closure)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_autoload, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_register_modules, 0, 0, 1)
    ZEND_ARG_INFO(0, modules)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_default_module, 0, 0, 1)
    ZEND_ARG_INFO(0, module)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_default_controller, 0, 0, 1)
    ZEND_ARG_INFO(0, controller)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_default_action, 0, 0, 1)
    ZEND_ARG_INFO(0, action)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_alias, 0, 0, 2)
    ZEND_ARG_INFO(0, alias_name)
    ZEND_ARG_INFO(0, alias_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_run, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* CApp class's methods */
CSPEED_METHOD(App, __construct) /*{{{ proto App::__construct() */
{
    /* In the constructor initialise the aliases with the default aliases app */
    zval *default_aliases = zend_read_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 1, NULL);
    zval all_aliases;
    array_init(&all_aliases);
    add_assoc_str(&all_aliases, "app", strpprintf(0, "%s/..", cspeed_get_cwd()));
    zend_update_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), &all_aliases);

    /* Setting the default loader */
    zval function_name, retval, callback;
    ZVAL_STRING(&function_name, "spl_autoload_register");
    array_init(&callback);
    add_next_index_zval(&callback, getThis());
    add_next_index_string(&callback, "autoload");
    uint32_t param_count = 1;
    zval params[] = { callback };
    call_user_function(CG(function_table), NULL, &function_name, &retval, param_count, params);
    zval_ptr_dtor(&retval);

    zval *di_object = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|o", &di_object) == FAILURE) {
        return ;
    }
    if (di_object != NULL) {
        if ( !instanceof_function(Z_OBJCE_P(di_object), cspeed_di_ce ) ){
            php_error_docref(NULL, E_ERROR, "Parameter must be instance of class derived from  Cs\\di\\Di class.");
            RETURN_FALSE
        }
        zend_update_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_DI_OBJECT), di_object);    
        /* \Cs\App::$app */
        zend_update_static_property(cspeed_app_ce, CSPEED_STRL(CSPEED_APP_PROPERTY), di_object);
    }

    /*modules*/
    zval modules;
    array_init(&modules);
    zend_update_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_MODULES), &modules);

    zval register_moduels;
    array_init(&register_moduels);
    add_assoc_string(&register_moduels, "../.", "../.");
    zend_update_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_MODULES), &register_moduels);
}/*}}}*/

CSPEED_METHOD(App, get)/*{{{ proto App::get() */
{
    handle_method_request("GET", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, post)/*{{{ proto App::post() */
{
    handle_method_request("POST", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, put)/*{{{ proto App::put() */
{
    handle_method_request("PUT", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, patch)/*{{{ proto App::patch() */
{
    handle_method_request("PATCH", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, delete)/*{{{ proto App::delete() */
{
    handle_method_request("DELETE", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, head)/*{{{ proto App::head() */
{
    handle_method_request("HEAD", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, options)/*{{{ proto App::options*/
{
    handle_method_request("OPTIONS", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, autoload)/*{{{ proto App::autoload The cspeed framework's autoload system */
{
    zend_string *class_name_with_namespace;             /* The class name which you want to load with the namespace */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &class_name_with_namespace) == FAILURE) {
        return;
    }
    /* To load the file */
    cspeed_app_load_file(class_name_with_namespace, INTERNAL_FUNCTION_PARAM_PASSTHRU, getThis());
}/*}}}*/

CSPEED_METHOD(App, setAlias)/*{{{ proto App::setAlias() */
{
    zend_string *alias_name, *alias_path;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &alias_name, &alias_path) == FAILURE) {
        return ;
    }
    if ( ( *ZSTR_VAL(alias_name) ) != '@') {
        php_error_docref(NULL, E_ERROR, "Namespace alias must be start with @.");
        RETURN_FALSE
    }
    zval *all_default_aliases = zend_read_property(cspeed_app_ce, getThis(), 
        CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 1, NULL);
    add_assoc_str(all_default_aliases, ZSTR_VAL(alias_name) + 1, alias_path);
}/*}}}*/

CSPEED_METHOD(App, registerModules)/*{{{ App::registerModules(['admin', 'backend', 'fronted'])*/
{
    zval *modules;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &modules) == FAILURE){
        return ;
    }
    zval *all_register_modules = zend_read_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_MODULES), 1, NULL);
    zend_hash_merge(Z_ARRVAL_P(all_register_modules), Z_ARRVAL_P(modules), (copy_ctor_func_t) zval_add_ref, 0);
}/*}}}*/

CSPEED_METHOD(App, run)/*{{{ proto App::run() */
{
    /* When enter into the run method, the cspeed will into the MVC logic */
    request_dispatcher_url(getThis());
}/*}}}*/

CSPEED_METHOD(App, setDefaultModule) /*{{{ proto App::setDefaultModule($moduleName)*/
{
    zend_string *default_module;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &default_module) == FAILURE){
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(default_module))){
        zend_update_property_str(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_DEFAULT_MODULE), default_module); /*"../" first*/
        RETURN_TRUE
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(App, setDefaultController) /*{{{ proto App::setDefaultController($controllerName)*/
{
    zend_string *default_controller;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &default_controller) == FAILURE){
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(default_controller))){
        zend_update_property_str(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_DEFAULT_CONTROLLER), 
            strpprintf(0, "%s", title_upper_string(ZSTR_VAL(default_controller))));
        RETURN_TRUE
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(App, setDefaultAction) /*{{{ proto App::setDefaultAction($actionName)*/
{
    zend_string *default_action;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &default_action) == FAILURE){
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(default_action))){
        zend_update_property_str(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_DEFAULT_ACTION), 
            strpprintf(0, "%sAction", ZSTR_VAL(default_action)));
        RETURN_TRUE
    }
    RETURN_FALSE
}/*}}}*/

/* The functions for the class CApp */
static const zend_function_entry cspeed_app_functions[] = {
    CSPEED_ME(App, __construct,         arginfo_cspeed_construct,                   ZEND_ACC_PUBLIC)
    CSPEED_ME(App, get,                 arginfo_cspeed_get,                         ZEND_ACC_PUBLIC)
    CSPEED_ME(App, post,                arginfo_cspeed_post,                        ZEND_ACC_PUBLIC)
    CSPEED_ME(App, put,                 arginfo_cspeed_post,                        ZEND_ACC_PUBLIC)
    CSPEED_ME(App, patch,               arginfo_cspeed_patch,                       ZEND_ACC_PUBLIC)
    CSPEED_ME(App, delete,              arginfo_cspeed_delete,                      ZEND_ACC_PUBLIC)
    CSPEED_ME(App, head,                arginfo_cspeed_head,                        ZEND_ACC_PUBLIC)
    CSPEED_ME(App, options,             arginfo_cspeed_options,                     ZEND_ACC_PUBLIC)
    CSPEED_ME(App, autoload,            arginfo_cspeed_autoload,                    ZEND_ACC_PUBLIC)
    CSPEED_ME(App, registerModules,     arginfo_cspeed_register_modules,            ZEND_ACC_PUBLIC)
    CSPEED_ME(App, setAlias,            arginfo_cspeed_set_alias,                   ZEND_ACC_PUBLIC)
    CSPEED_ME(App, setDefaultModule,    arginfo_cspeed_set_default_module,          ZEND_ACC_PUBLIC)
    CSPEED_ME(App, setDefaultController,arginfo_cspeed_set_default_controller,      ZEND_ACC_PUBLIC)
    CSPEED_ME(App, setDefaultAction,    arginfo_cspeed_set_default_action,          ZEND_ACC_PUBLIC)
    CSPEED_ME(App, run,                 arginfo_cspeed_run,                         ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/*}}}*/

/* {{{ proto void app_init() init the class CApp */
CSPEED_INIT(app)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs", "App", cspeed_app_functions);
    cspeed_app_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(cspeed_app_ce,   CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES),   ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_app_ce,   CSPEED_STRL(CSPEED_APP_DI_OBJECT),          ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_app_ce,   CSPEED_STRL(CSPEED_APP_MODULES),            ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_app_ce,   CSPEED_STRL(CSPEED_APP_PROPERTY),           ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);
    zend_declare_property_string(cspeed_app_ce, CSPEED_STRL(CSPEED_DEFAULT_MODULE), 
        CSPEED_APP_DEFAULT_MODULE,              ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_app_ce, CSPEED_STRL(CSPEED_DEFAULT_CONTROLLER), 
        CSPEED_APP_DEFAULT_CONTROLLER,          ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_app_ce, CSPEED_STRL(CSPEED_DEFAULT_ACTION), 
        CSPEED_APP_DEFAULT_ACTION,              ZEND_ACC_PRIVATE);


}
/*}}}*/







/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
