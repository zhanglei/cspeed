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
#include "kernel/bootstrap.h"
#include "kernel/net/request.h"

#include "kernel/tool/helper.h"
#include "kernel/tool/require.h"

#include "kernel/di/di.h"
#include "kernel/mvc/router.h"
#include "kernel/mvc/dispatch.h"

#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_string.h"

int cspeed_deal_reqeust(zend_string *url, zend_fcall_info *zfi, zend_fcall_info_cache *zfic, zval *ret_val) /*{{{ Deal the REQUEST */
{
    char *path_info = cspeed_request_server_str_key_val("PATH_INFO");
    if (CSPEED_STRING_NOT_EMPTY(path_info) 
        || ( ( *(ZSTR_VAL(url)) == '/' ) && (ZSTR_LEN(url) == 1) ) ) {
        zend_string *pattern = strpprintf(0, "#%s#", ZSTR_VAL(url));

        /* The nested thing to get the preg mathces */
        zval nested_pattern, nested_pattern_string;
        array_init(&nested_pattern);
        add_next_index_string(&nested_pattern, "#:action:#");
        add_next_index_string(&nested_pattern, "#:any:#");
        add_next_index_string(&nested_pattern, "#:controller:#");
        add_next_index_string(&nested_pattern, "#:id:#");
        add_next_index_string(&nested_pattern, "#:module:#");

        array_init(&nested_pattern_string);
        add_next_index_string(&nested_pattern_string, "([^0-9-][a-zA-Z0-9-]*)");
        add_next_index_string(&nested_pattern_string, "([^/]+)");
        add_next_index_string(&nested_pattern_string, "([^0-9-][a-zA-Z0-9-]*)");
        add_next_index_string(&nested_pattern_string, "([0-9]+)");
        add_next_index_string(&nested_pattern_string, "([^0-9-][a-zA-Z0-9-]*)");

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
                zend_fcall_info_args(zfi, &subparts);
                zend_call_function(zfi, zfic);
                zend_fcall_info_args_clear(zfi, 1);
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, ini_config_file)
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_alias, 0, 0, 2)
    ZEND_ARG_INFO(0, alias_name)
    ZEND_ARG_INFO(0, alias_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_bootstrap, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_run, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

/* CApp class's methods */
CSPEED_METHOD(App, __construct) /*{{{ proto App::__construct() */
{
    /* Do the Prepare job */
    zend_string *ini_config_file, *ini_config_node_name = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|S", &ini_config_file, &ini_config_node_name) == FAILURE) {
        return ;
    }
    char path[MAXPATHLEN];
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(ini_config_file))) {
        zend_string *ini_real_file = strpprintf(0, "%s/%s", cspeed_get_cwd(path), ZSTR_VAL(ini_config_file));
        if (access(ZSTR_VAL(ini_real_file), F_OK) == -1) {
            php_error_docref(NULL, E_ERROR, "Config file: `%s` not exists.", ZSTR_VAL(ini_real_file));
            RETURN_FALSE
        }
        zval configs;
        
        zend_string *node_core_name = strpprintf(0, "%s%s%s", ini_config_node_name ? ZSTR_VAL(ini_config_node_name) : "",
                    ini_config_node_name ? ":" : "", "core" );
        zend_string *node_db_name = strpprintf(0, "%s%s%s", ini_config_node_name ? ZSTR_VAL(ini_config_node_name) : "",
                    ini_config_node_name ? ":" : "", "db" );
        
        /* Parsing the INI file */
        cspeed_parse_ini_file(ZSTR_VAL(ini_real_file), NULL , NULL, 1, &configs);

        if (ZVAL_IS_NULL(&configs)  && !zend_hash_num_elements(Z_ARRVAL(configs)) ) {
            php_error_docref(NULL, E_ERROR, "Configs empty.");
            RETURN_FALSE
        }

        if ( ( Z_TYPE(configs) == IS_ARRAY ) ) {
            zval *config_value, *core_configs;
            if ( UNEXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_core_name)) == NULL )){
                php_error_docref(NULL, E_ERROR, "`%s` configs not found in config file.", ZSTR_VAL(node_core_name));
                RETURN_FALSE
            }
            
            /*core.application*/
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_APPLICATION_NAME))) != NULL ) ) {
                CSPEED_G(core_application) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_APPLICATION_NAME);
                RETURN_FALSE
            }

            /*core.bootstrap*/
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_BOOTSTRAP_NAME);
                RETURN_FALSE
            }

            /*core.bootstrap*/
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_METHOD_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap_method_string) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_BOOTSTRAP_METHOD_NAME);
                RETURN_FALSE
            }

            /* core.router.modules */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_MODULES_NAME) )) != NULL )) {
                zval modules;
                array_init(&modules);
                php_explode(zend_string_init(CSPEED_STRL(","), 0), Z_STR_P(config_value), &modules, ZEND_LONG_MAX);
                zval_add_ref(&modules);
                CSPEED_G(core_router_modules) = Z_ARRVAL(modules);
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_MODULES_NAME);
                RETURN_FALSE
            }

            /* core.router.default.module */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_MODULE_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_module) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_MODULE_NAME);
                RETURN_FALSE
            }

            /* core.router.default.controller */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_CONTROLLER_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_controller) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_CONTROLLER_NAME);
                RETURN_FALSE
            }

            /* core.router.default.action */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_ACTION_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_action) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_ACTION_NAME);
                RETURN_FALSE
            }

            /* core.view.ext */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_VIEW_EXT_NAME) )) != NULL )) {
                CSPEED_G(core_view_ext) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_VIEW_EXT_NAME);
                RETURN_FALSE
            }

            /* core.view.auto_render */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_VIEW_AUTO_RENDER) )) != NULL )) {
                CSPEED_G(core_view_auto_render) = Z_STR_P(config_value);
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_VIEW_AUTO_RENDER);
                RETURN_FALSE
            }

            if ( UNEXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_db_name )) == NULL )){
                php_error_docref(NULL, E_ERROR, "`%s` configs not found in config file.", ZSTR_VAL(node_db_name));
                RETURN_FALSE
            }

            /* Below are some optional configs to the DB engine, if have, set it to override the setting */
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_TYPE) )) != NULL )) {
                CSPEED_G(db_master_type) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_HOST) )) != NULL )) {
                CSPEED_G(db_master_host) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_PORT) )) != NULL )) {
                CSPEED_G(db_master_port) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_DB_NAME) )) != NULL )) {
                CSPEED_G(db_master_dbname) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_USERNAME) )) != NULL )) {
                CSPEED_G(db_master_username) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( UNEXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_PASSWORD) )) != NULL )) {
                CSPEED_G(db_master_password) = zend_string_copy(Z_STR_P(config_value));
            }
            zend_string_release(node_core_name);
            zend_string_release(node_db_name);
        } else {
            zend_string_release(node_core_name);
            zend_string_release(node_db_name);
            php_error_docref(NULL, E_ERROR, "Configs data type wrong.");
            RETURN_FALSE
        }
    } else {
        php_error_docref(NULL, E_ERROR, "Parameters must be a valid string.");
        RETURN_FALSE
    }

    /* In the constructor initialise the aliases with the default alias named `app` */
    zval *default_aliases = zend_read_property(cspeed_app_ce, getThis(), CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 1, NULL);
    zval all_aliases;
    array_init(&all_aliases);
    add_assoc_str(&all_aliases, "app", strpprintf(0, "%s/%s", path, ZSTR_VAL(CSPEED_G(core_application))));
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

CSPEED_METHOD(App, run)/*{{{ proto App::run() */
{
    dispather_url();
}/*}}}*/

CSPEED_METHOD(App, bootstrap)/*{{{ proto App::bootstrap()*/
{
    /* When user run this method, this method will to found the bootstrap file to do some bootstrap initialise job
     * Such as initialise the Router rules, and the global Di objects.
     */
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);
    /* To check the file and or load it or not  */
    zend_string *bootstrap_class_file = strpprintf(0, "%s/%s", path, ZSTR_VAL(CSPEED_G(core_bootstrap)));
    if ( check_file_exists(ZSTR_VAL( bootstrap_class_file )) ){
        cspeed_require_file(ZSTR_VAL( bootstrap_class_file ), NULL, NULL, NULL);
        /* Found the Bootstrap class */
        zend_class_entry *bootstrap_class_ptr = zend_hash_find_ptr(EG(class_table), 
                        zend_string_tolower(zend_string_init(CSPEED_STRL(CORE_BOOTSTRAP_CLASS_NAME), 0)));

        if (!instanceof_function(bootstrap_class_ptr, cspeed_bootstrap_ce)){
            php_error_docref(NULL, E_ERROR, "Bootstrap class must implements interface \\Cs\\Bootstrap.");
            RETURN_FALSE
        }

        if (bootstrap_class_ptr) {
            /* Initialise the Bootstrap class object */
            zval bootstrap_object;
            object_init_ex(&bootstrap_object, bootstrap_class_ptr);
            /* Found all methods starts with the __init string  */
            /* In the initialise job, you can setting the Di & Router */
            zval di_object, router_object;
            object_init_ex(&di_object, cspeed_di_ce);
            initialise_di_object_properties(&di_object);

            object_init_ex(&router_object, cspeed_router_ce);
            initialise_router_object_properties(&router_object);
            /* Begin to initialise */
            zend_string *bootstrap_function_name;
            ZEND_HASH_FOREACH_STR_KEY(&(bootstrap_class_ptr->function_table), bootstrap_function_name) {
                if (strncasecmp(ZSTR_VAL(bootstrap_function_name), 
                    CSPEED_STRL( ZSTR_VAL(CSPEED_G(core_bootstrap_method_string)) )) == 0 ) {
                    zval temp_function_name;
                    ZVAL_STRING(&temp_function_name, ZSTR_VAL(bootstrap_function_name));
                    zval retval;
                    zval params[] = { di_object, router_object };
                    call_user_function(NULL, &bootstrap_object, &temp_function_name, &retval, 2, params);
                    zval_ptr_dtor(&retval);
                    zval_ptr_dtor(&temp_function_name);
                }
            } ZEND_HASH_FOREACH_END();
            zval_ptr_dtor(&bootstrap_object);
            zval_add_ref(&di_object);
            zval_add_ref(&router_object);
            CSPEED_G(di_object) = Z_OBJ(di_object);
            CSPEED_G(router_object) = Z_OBJ(router_object);
            /* Return the App class object to user, to do the next job. such as the router parsing and so on. */
            RETURN_ZVAL(getThis(), 1, NULL);
        } else {
            php_error_docref(NULL, E_ERROR, "class Boostrap not exists.");
            RETURN_FALSE
        }
    } else {
        php_error_docref(NULL, E_ERROR, "Bootstrap file: `%s` not found.", ZSTR_VAL(CSPEED_G(core_bootstrap)));
        RETURN_FALSE
    }
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
    CSPEED_ME(App, setAlias,            arginfo_cspeed_set_alias,                   ZEND_ACC_PUBLIC)
    CSPEED_ME(App, run,                 arginfo_cspeed_run,                         ZEND_ACC_PUBLIC)
    CSPEED_ME(App, bootstrap,           arginfo_cspeed_bootstrap,                   ZEND_ACC_PUBLIC)
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
    zend_declare_property_null(cspeed_app_ce,   CSPEED_STRL(CSPEED_APP_PROPERTY),           ZEND_ACC_PUBLIC|ZEND_ACC_STATIC);

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
