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

#include "kernel/tool/helper.h"
#include "kernel/console/task.h"
#include "kernel/console/parse.h"


/* {{{ All ARG_INFO For Task class */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_task_construct, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_task_run, 0, 0, 0)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(Task, __construct)/*{{{ Task::construct()*/
{
    zend_string *ini_config_file = NULL, *ini_config_node_name = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|SS", &ini_config_file, &ini_config_node_name) == FAILURE) {
        return ;
    }
    char path[MAXPATHLEN];
    cspeed_get_cwd(path);
    if ( ini_config_file && CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(ini_config_file))) {
        zend_string *ini_real_file;
        if ( *(ZSTR_VAL(ini_config_file)) == '/' ) {
            ini_real_file = ini_config_file;
        } else {
            ini_real_file = strpprintf(0, "%s/%s", path, ZSTR_VAL(ini_config_file));
        }
        check_file_exists(ZSTR_VAL(ini_real_file));
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
            if ( EXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_core_name)) == NULL )){
                php_error_docref(NULL, E_ERROR, "`%s` configs not found in config file.", ZSTR_VAL(node_core_name));
                RETURN_FALSE
            }
            /*core.application*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_APPLICATION_NAME))) != NULL ) ) {
                CSPEED_G(core_application) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_APPLICATION_NAME);
                RETURN_FALSE
            }
            /*core.bootstrap*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_BOOTSTRAP_NAME);
                RETURN_FALSE
            }
            /*core.bootstrap*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_METHOD_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap_method_string) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_BOOTSTRAP_METHOD_NAME);
                RETURN_FALSE
            }
            /* core.router.modules */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
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
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_MODULE_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_module) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_MODULE_NAME);
                RETURN_FALSE
            }
            /* core.router.default.controller */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs),
                CSPEED_STRL(CORE_CONFIG_DEFAULT_CONTROLLER_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_controller) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_CONTROLLER_NAME);
                RETURN_FALSE
            }
            /* core.router.default.action */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_ACTION_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_action) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_ACTION_NAME);
                RETURN_FALSE
            }
            
            if ( EXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_db_name )) == NULL )){
                php_error_docref(NULL, E_ERROR, "`%s` configs not found in config file.", ZSTR_VAL(node_db_name));
                RETURN_FALSE
            }
            /* Below are some optional configs to the DB engine, if have, set it to override the setting */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_DSN) )) != NULL )) {
                CSPEED_G(db_master_dsn) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_USERNAME) )) != NULL )) {
                CSPEED_G(db_master_username) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
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
    }
}/*}}}*/

CSPEED_METHOD(Task, run)/*{{{ proto Task::run($module, $controller, $action)*/
{
    zend_string *url;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|S", &url) == FAILURE) {
        return ;
    }
    dispather_cli_url(url);
}/*}}}*/

static const zend_function_entry cspeed_task_functions[] = {/*{{{ proto Task's methods */
    CSPEED_ME(Task, __construct,    arginfo_cspeed_task_construct,  ZEND_ACC_PUBLIC)
    CSPEED_ME(Task, run,            arginfo_cspeed_task_run,        ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(task)/*{{{ proto void task_init() */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\console", "Task", cspeed_task_functions);
    cspeed_task_ce = zend_register_internal_class(&ce);
}/*}}}*/



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
