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
#include "php_main.h"
#include "php_cspeed.h"
#include "ext/standard/info.h"
#include "ext/pcre/php_pcre.h"
#include "Zend/zend_inheritance.h"
#include "ext/standard/php_string.h"

#include "kernel/CApp.h"
#include "kernel/di/di.h"
#include "kernel/bootinit.h"
#include "kernel/mvc/router.h"
#include "kernel/net/request.h"
#include "kernel/tool/helper.h"
#include "kernel/tool/require.h"
#include "kernel/mvc/dispatch.h"
#include "kernel/tool/component.h"



int
cspeed_deal_reqeust(zend_string *url, zend_fcall_info *zfi, zend_fcall_info_cache *zfic, zval *ret_val) 
/*{{{ Deal the REQUEST */
{
    char *path_info = cspeed_request_server_str_key_val("PATH_INFO");
    if (CSPEED_STRING_NOT_EMPTY(path_info) 
        || ( ( *(ZSTR_VAL(url)) == '/' ) && (ZSTR_LEN(url) == 1) ) ) {
        zend_string *pattern = strpprintf(0, "#%s#", ZSTR_VAL(url));

        /* The nested thing to get the preg mathces */
        zval nested_pattern, nested_pattern_string;
        array_init(&nested_pattern);
        add_next_index_string(
            &nested_pattern, 
            "#:action:#"
        );
        add_next_index_string(
            &nested_pattern, 
            "#:any:#"
        );
        add_next_index_string(
            &nested_pattern, 
            "#:controller:#"
        );
        add_next_index_string(
            &nested_pattern, 
            "#:id:#"
        );
        add_next_index_string(
            &nested_pattern, 
            "#:module:#"
        );

        array_init(&nested_pattern_string);
        add_next_index_string(
            &nested_pattern_string, 
            "([^0-9-][a-zA-Z0-9-]*)"
        );
        add_next_index_string(
            &nested_pattern_string, 
            "([^/]+)"
        );
        add_next_index_string(
            &nested_pattern_string, 
            "([^0-9-][a-zA-Z0-9-]*)"
        );
        add_next_index_string(
            &nested_pattern_string, 
            "([0-9]+)"
        );
        add_next_index_string(
            &nested_pattern_string, 
            "([^0-9-][a-zA-Z0-9-]*)"
        );

        zval url_pattern;
        ZVAL_STRING(&url_pattern, ZSTR_VAL(pattern));

        zval preg_replace_result;
        zval preg_function_name;
        ZVAL_STRING(&preg_function_name, "preg_replace");
        zval preg_params[] = { nested_pattern, nested_pattern_string, url_pattern };
        call_user_function(
            CG(function_table), 
            NULL, 
            &preg_function_name, 
            &preg_replace_result, 
            3, 
            preg_params
        );

        pcre_cache_entry *pce_regexp;
        if ((pce_regexp = pcre_get_compiled_regex_cache(Z_STR(preg_replace_result))) != NULL) {
            zval matches, subparts;
            ZVAL_NULL(&subparts);
            
            php_pcre_match_impl(
                pce_regexp, 
                CSPEED_STRL(path_info), 
                &matches, 
                &subparts, 
                0, 
                0, 
                0, 
                0
            );

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
    }
    return -1;
}/*}}}*/

int handle_request(INTERNAL_FUNCTION_PARAMETERS)/*{{{ Handle the user input from the PHP level*/
{
    zend_string *url;
    zend_fcall_info zfi;
    zend_fcall_info_cache zfic;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Sf*", &url, &zfi, &zfic, 
        &zfi.params, &zfi.param_count) == FAILURE) {
        return -1;
    }
    zval retval;
    int result = cspeed_deal_reqeust(url, &zfi, &zfic, &retval);
    zval_ptr_dtor(&retval);
    return result;
} /*}}}*/

void handle_method_request(zval *object_ptr, char *method_name, INTERNAL_FUNCTION_PARAMETERS)/*{{{ Handle the request */
{
    int continue_or_false = -1;
    trigger_events(
        object_ptr, 
        strpprintf(
            0, 
            "%s", 
            CSPEED_APP_EVENT_BEORE_REQUEST
        )
    );
    if (cspeed_request_is_method(method_name)) {
        continue_or_false = handle_request(INTERNAL_FUNCTION_PARAM_PASSTHRU);
    }
    trigger_events(
        object_ptr, 
        strpprintf(
            0, 
            "%s", 
            CSPEED_APP_EVENT_AFTER_REQUEST
        )
    );
    if ( continue_or_false >= 0 ) {
        /* Fix: if meet the suited regular url, the next will abandoned */
        php_request_shutdown(NULL);
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_alias, 0, 0, 2)
    ZEND_ARG_INFO(0, alias_name)
    ZEND_ARG_INFO(0, alias_path)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_bootstrap, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_get_app, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_run, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_set_composer_loader, 0, 0, 1)
    ZEND_ARG_INFO(0, composer_loader_file)
ZEND_END_ARG_INFO()
/* }}} */

void 
initialise_app_object(zval *app_object, char *path)
{
    /* In the constructor initialise the aliases with the default alias named `app` */
    zval *default_aliases = zend_read_property(
        cspeed_app_ce, 
        app_object, 
        CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 
        1, 
        NULL
    );
    zval all_aliases;
    array_init(&all_aliases);
    add_assoc_str(
        &all_aliases, 
        "app", 
        strpprintf(
            0, 
            "%s/%s", 
            path, 
            ZSTR_VAL(CSPEED_G(core_application))
        )
    );
    zend_update_property(
        cspeed_app_ce, 
        app_object, 
        CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 
        &all_aliases
    );

    /* Setting the default loader */
    zval function_name, retval, callback;
    ZVAL_STRING(&function_name, "spl_autoload_register");
    array_init(&callback);
    add_next_index_zval(&callback, app_object);
    add_next_index_string(&callback, "autoload");
    uint32_t param_count = 1;
    zval params[] = { callback };
    call_user_function(
        CG(function_table), 
        NULL, 
        &function_name, 
        &retval, 
        param_count, 
        params
    );
    zval_ptr_dtor(&retval);
}

/* CApp class's methods */
CSPEED_METHOD(App, __construct) /*{{{ proto App::__construct() */
{
    zval *is_new_app = zend_read_static_property(
        cspeed_app_ce, 
        CSPEED_STRL(CSPEED_APP_INSTANCE), 
        1
    );
    if (!ZVAL_IS_NULL(is_new_app)) {
        php_error_docref(
            NULL, 
            E_ERROR, 
            "An application is running."
        );
        return ;
    }
    /* Do the Prepare job */
    zend_string *ini_config_file = NULL, *ini_config_node_name = NULL;
    if (zend_parse_parameters(
        ZEND_NUM_ARGS() TSRMLS_CC, 
        "|SS", 
        &ini_config_file, 
        &ini_config_node_name
    ) == FAILURE) {
        return ;
    }
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);
    
    /* Load the kernel default setting from the ini config file */
    load_kernel_setting(ini_config_file, ini_config_node_name, path);

    /* Keep the App object */
    zend_update_static_property(
        cspeed_app_ce, 
        CSPEED_STRL(CSPEED_APP_INSTANCE), 
        getThis()
    );

    /* Initialise the standard app object */
    initialise_app_object(getThis(), path);
}/*}}}*/

CSPEED_METHOD(App, get)/*{{{ proto App::get() */
{
    handle_method_request(getThis(), "GET", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, post)/*{{{ proto App::post() */
{
    handle_method_request(getThis(), "POST", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, put)/*{{{ proto App::put() */
{
    handle_method_request(getThis(), "PUT", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, patch)/*{{{ proto App::patch() */
{
    handle_method_request(getThis(), "PATCH", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, delete)/*{{{ proto App::delete() */
{
    handle_method_request(getThis(), "DELETE", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, head)/*{{{ proto App::head() */
{
    handle_method_request(getThis(), "HEAD", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, options)/*{{{ proto App::options*/
{
    handle_method_request(getThis(), "OPTIONS", INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(App, autoload)/*{{{ proto App::autoload The cspeed framework's autoload system */
{
    zend_string *class_name_with_namespace;             /* The class name which you want to load with the namespace */
    if (zend_parse_parameters(
        ZEND_NUM_ARGS() TSRMLS_CC, 
        "S", 
        &class_name_with_namespace
    ) == FAILURE) {
        return;
    }
    /* To load the file */
    cspeed_autoload_file(
        class_name_with_namespace, 
        getThis(), 
        CSPEED_APP_AUTOLOAD_ALIASES
    );
}/*}}}*/

CSPEED_METHOD(App, setAlias)/*{{{ proto App::setAlias() */
{
    zend_string *alias_name, *alias_path;
    if (zend_parse_parameters(
        ZEND_NUM_ARGS() TSRMLS_CC, 
        "SS", 
        &alias_name, 
        &alias_path
    ) == FAILURE) {
        return ;
    }
    if ( ( *ZSTR_VAL(alias_name) ) != '@') {
        php_error_docref(
            NULL, 
            E_ERROR, 
            "Namespace alias must be start with @."
        );
        RETURN_FALSE
    }
    zval *all_default_aliases = zend_read_property(
        cspeed_app_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES), 
        1, 
        NULL
    );
    add_assoc_str(
        all_default_aliases, 
        ZSTR_VAL(alias_name) + 1, 
        alias_path
    );
}/*}}}*/

CSPEED_METHOD(App, getApp)/*{{{ proto App::getApp()*/
{
    zval *app_object = zend_read_static_property(
        cspeed_app_ce, 
        CSPEED_STRL(CSPEED_APP_INSTANCE), 
        1
    );
    RETURN_ZVAL(app_object, 1, 0);
}/*}}}*/

CSPEED_METHOD(App, run)/*{{{ proto App::run() */
{
    trigger_events(
        getThis(), 
        strpprintf(
            0, 
            "%s", 
            CSPEED_APP_EVENT_BEORE_REQUEST
        )
    );
    dispather_url();
    trigger_events(
        getThis(), 
        strpprintf(
            0, 
            "%s", 
            CSPEED_APP_EVENT_AFTER_REQUEST
        )
    );
}/*}}}*/

CSPEED_METHOD(App, bootstrap)/*{{{ proto App::bootstrap()*/
{
    /* When user run this method, this method will to found the bootstrap file to do some bootstrap initialise job
     * Such as initialise the Router rules, and the global Di objects.
     */
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);
    /* To check the file and or load it or not  */
    zend_string *bootstrap_class_file = strpprintf(
        0, 
        "%s/%s", 
        path, 
        ZSTR_VAL(CSPEED_G(core_bootstrap))
    );
    check_file_exists(ZSTR_VAL( bootstrap_class_file ));
    if (cspeed_require_file(ZSTR_VAL( bootstrap_class_file ), NULL, NULL, NULL) == FALSE){
        RETURN_FALSE
    }
    /* Found the Bootstrap class */
    zend_class_entry *bootstrap_class_ptr = zend_hash_find_ptr(
        EG(class_table), 
        zend_string_tolower(
            zend_string_init(
                CSPEED_STRL(CORE_BOOTSTRAP_CLASS_NAME),
                0
            )
        )
    );

    if (!instanceof_function(bootstrap_class_ptr, cspeed_bootinit_ce)){
        php_error_docref(
            NULL, 
            E_ERROR, 
            "BootInit class must implements interface \\Cs\\BootInit."
        );
        RETURN_FALSE
    }

    if (bootstrap_class_ptr) {
        /* Initialise the Bootstrap class object */
        zval bootstrap_object;
        object_init_ex(&bootstrap_object, bootstrap_class_ptr);
        /* Found all methods starts with the __init string  */
        /* In the initialise job, you can setting the Di & Router */
        zval di_object, router_object;
        ZVAL_OBJ(&di_object, CSPEED_G(di_object));
        ZVAL_OBJ(&router_object, CSPEED_G(router_object));
        /* Begin to initialise */
        zend_string *bootstrap_function_name;
        ZEND_HASH_FOREACH_STR_KEY(&(bootstrap_class_ptr->function_table), bootstrap_function_name) {
            if (strncasecmp(ZSTR_VAL(bootstrap_function_name), 
                CSPEED_STRL( ZSTR_VAL(CSPEED_G(core_bootstrap_method_string)) )) == 0 ) {
                zval temp_function_name;
                ZVAL_STRING(&temp_function_name, ZSTR_VAL(bootstrap_function_name));
                zval retval;
                zval params[] = { di_object, router_object };
                call_user_function(
                    NULL, 
                    &bootstrap_object, 
                    &temp_function_name, 
                    &retval, 
                    2, 
                    params
                );
                zval_ptr_dtor(&retval);
                zval_ptr_dtor(&temp_function_name);
            }
        } ZEND_HASH_FOREACH_END();
        zval_ptr_dtor(&bootstrap_object);
        zval_add_ref(&di_object);
        zval_add_ref(&router_object);
        /* Return the App class object to user, to do the next job. such as the router parsing and so on. */
        RETURN_ZVAL(getThis(), 1, NULL);
    } else {
        php_error_docref(
            NULL, 
            E_ERROR, 
            "class BoosInit not exists."
        );
    }
}/*}}}*/

CSPEED_METHOD(App, setComposerLoader) /*{{{ proto App::setComposerLoader($filePathName) */
{
    zend_string *composer_file;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &composer_file) == FAILURE) {
        return ;
    }
    
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(composer_file))) {
        if ( *(ZSTR_VAL(composer_file)) == '/' ) {
            check_file_exists(ZSTR_VAL(composer_file));
            cspeed_require_file(ZSTR_VAL(composer_file), NULL, NULL, NULL);
        } else {
            zend_string *temp_file_path = strpprintf(
                0, 
                "%s/%s", 
                path, 
                ZSTR_VAL(composer_file)
            );
            check_file_exists(ZSTR_VAL(temp_file_path));
            cspeed_require_file(ZSTR_VAL(temp_file_path), NULL, NULL, NULL);
        }
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
    CSPEED_ME(App, getApp,              arginfo_cspeed_get_app,                     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    CSPEED_ME(App, run,                 arginfo_cspeed_run,                         ZEND_ACC_PUBLIC)
    CSPEED_ME(App, bootstrap,           arginfo_cspeed_bootstrap,                   ZEND_ACC_PUBLIC)
    CSPEED_ME(App, setComposerLoader,   arginfo_cspeed_set_composer_loader,         ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/*}}}*/

/* {{{ proto void app_init() init the class CApp */
CSPEED_INIT(app)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs", "App", cspeed_app_functions);
    cspeed_app_ce = zend_register_internal_class(&ce);

    /* To use the Event feature */
    zend_do_inheritance(cspeed_app_ce, cspeed_component_ce);

    zend_declare_property_null(
        cspeed_app_ce,   
        CSPEED_STRL(CSPEED_APP_AUTOLOAD_ALIASES),   
        ZEND_ACC_PRIVATE
    );
    zend_declare_property_null(
        cspeed_app_ce,   
        CSPEED_STRL(CSPEED_APP_INSTANCE),   
        ZEND_ACC_PRIVATE|ZEND_ACC_STATIC
    );

    zend_declare_class_constant_string(
        cspeed_app_ce, 
        CSPEED_STRL(CSPEED_APP_EVENT_BEORE_REQUEST), 
        CSPEED_APP_EVENT_BEORE_REQUEST
    );
    zend_declare_class_constant_string(
        cspeed_app_ce, 
        CSPEED_STRL(CSPEED_APP_EVENT_AFTER_REQUEST), 
        CSPEED_APP_EVENT_AFTER_REQUEST
    );
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
