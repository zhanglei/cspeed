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

/* The \Cs\console\Task class */
extern zend_class_entry *cspeed_task_ce;

/* {{{ All ARG_INFO For Task class */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_task_construct, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_task_run, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_task_autoload, 0, 0, 0)
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

    /* Load the kernel default setting from the ini config file */
    load_kernel_setting(ini_config_file, ini_config_node_name, path);

    /* To define the autoload class */
    /* In the constructor initialise the aliases with the default alias named `app` */
    zval *default_aliases = zend_read_property(cspeed_task_ce, getThis(), CSPEED_STRL(CSPEED_TASK_AUTOLOAD_ALIASES), 1, NULL);
    zval all_aliases;
    array_init(&all_aliases);
    add_assoc_str(&all_aliases, "app", strpprintf(0, "%s/%s", path, ZSTR_VAL(CSPEED_G(core_application))));
    zend_update_property(cspeed_task_ce, getThis(), CSPEED_STRL(CSPEED_TASK_AUTOLOAD_ALIASES), &all_aliases);

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
    /* --- end for the autoloader ---*/

}/*}}}*/

CSPEED_METHOD(Task, autoload)/*{{{ proto App::autoload The cspeed framework's autoload system */
{
    zend_string *class_name_with_namespace;             /* The class name which you want to load with the namespace */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &class_name_with_namespace) == FAILURE) {
        return;
    }
    /* To load the file */
    cspeed_autoload_file(class_name_with_namespace, getThis(), CSPEED_TASK_AUTOLOAD_ALIASES);
}/*}}}*/

CSPEED_METHOD(Task, run)/*{{{ proto Task::run($module, $controller, $action)*/
{
    zend_string *url;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &url) == FAILURE) {
        return ;
    }
    dispather_cli_url(url);
}/*}}}*/

static const zend_function_entry cspeed_task_functions[] = {/*{{{ proto Task's methods */
    CSPEED_ME(Task, __construct,    arginfo_cspeed_task_construct,  ZEND_ACC_PUBLIC)
    CSPEED_ME(Task, run,            arginfo_cspeed_task_run,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Task, autoload,       arginfo_cspeed_task_autoload,   ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(task)/*{{{ proto void task_init() */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\console", "Task", cspeed_task_functions);
    cspeed_task_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(cspeed_task_ce, CSPEED_STRL(CSPEED_TASK_AUTOLOAD_ALIASES), ZEND_ACC_PRIVATE);
}/*}}}*/



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
