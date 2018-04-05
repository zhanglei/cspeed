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

#ifndef PHP_CSPEED_H
#define PHP_CSPEED_H

extern zend_module_entry cspeed_module_entry;
#define phpext_cspeed_ptr &cspeed_module_entry

#define PHP_CSPEED_VERSION "2.1.11"

#ifdef PHP_WIN32
#	define PHP_CSPEED_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_CSPEED_API __attribute__ ((visibility("default")))
#else
#	define PHP_CSPEED_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/
ZEND_BEGIN_MODULE_GLOBALS(cspeed)
    /* Below are the core configs */
    zend_string   *core_application;
    zend_string   *core_bootstrap;
    zend_string   *core_bootstrap_method_string;
    HashTable     *core_router_modules; /* HashTable */
    zend_string   *core_router_default_module;
    zend_string   *core_router_default_controller;
    zend_string   *core_router_default_action;
    zend_string   *core_view_ext;
    zend_string   *core_view_auto_render;
    zend_string   *core_url_pattern;

    /* Below are the DB configs */
    zend_string   *db_master_dsn;
    zend_string   *db_master_username;
    zend_string   *db_master_password;

    /* Below are the global object Di & Router */
    zend_object   *di_object;
    zend_object   *router_object;

    /* Below are the extension configs */
    HashTable     *db_connections;
    zval          *new_db_pdo_object;   /* The newest db connection. */
ZEND_END_MODULE_GLOBALS(cspeed)

/* Always refer to the globals in your function as CSPEED_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define CSPEED_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(cspeed, v)

#if defined(ZTS) && defined(COMPILE_DL_CSPEED)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#include <string.h>

/*Some macros for the cspeed to use*/
#define TRUE                          1
#define FALSE                         0

/* Define the CSpeed system config */
#define CORE_CONFIG_APPLICATION_NAME            "core.application"
#define CORE_CONFIG_BOOTSTRAP_NAME              "core.bootstrap"
#define CORE_CONFIG_BOOTSTRAP_METHOD_NAME       "core.bootstrap.method.string"
#define CORE_CONFIG_MODULES_NAME                "core.router.modules"
#define CORE_CONFIG_DEFAULT_MODULE_NAME         "core.router.default.module"
#define CORE_CONFIG_DEFAULT_CONTROLLER_NAME     "core.router.default.controller"
#define CORE_CONFIG_DEFAULT_ACTION_NAME         "core.router.default.action"
#define CORE_CONFIG_VIEW_EXT_NAME               "core.view.ext"
#define CORE_CONFIG_VIEW_AUTO_RENDER            "core.view.auto.render"
#define CORE_CONFIG_URL_PATTERN                 "core.url.pattern"

/* Db configs */
#define CORE_CONFIG_DB_DSN                      "db.master.dsn"
#define CORE_CONFIG_DB_USERNAME                 "db.master.username"
#define CORE_CONFIG_DB_PASSWORD                 "db.master.password"

/* Below are some default setting of the CSpeed engine */
#define CORE_APPLICATION                        "../app"
#define CORE_BOOTSTRAP                          "../app/bootinit.php"
#define CORE_ROUTER_MODULES                     "home"
#define CORE_ROUTER_DEFAULT_MODULE              "home"
#define CORE_ROUTER_DEFAULT_CONTROLLER          "index"
#define CORE_ROUTER_DEFAULT_ACTION              "index"
#define CORE_VIEW_EXT                           "html"
#define CORE_URL_PATTERN                        ".html"
#define CORE_VIEW_AUTO_RENDER                   "0"
#define CORE_BOOSTRAP_METHOD_STRING             "__init"
#define CORE_BOOTSTRAP_CLASS_NAME               "BootInit"

#define CORE_VIEW_NEED_RENDER                   "1"

/* Some macros for the CSpeed engine */
#define CSPEED_FN(function_name)        function_name##Action
#define CSPEED_METHOD                   PHP_METHOD
#define CSPEED_ME                       PHP_ME
#define CSPEED_MINT_FUNCTION            PHP_MINIT_FUNCTION
#define CSPEED_INIT(function_name)      void function_name##_init()
#define CSPEED_STRL(str)                (str), strlen(str)
#define CSPEED_METHOD_IN_OBJECT(object, method_name) ( ( Z_OBJ_HT_P(object)->get_method(&Z_OBJ_P(object),\
                                        zend_string_init(CSPEED_STRL(method_name), 0), NULL)) != NULL )
#define substring( x, y ) substr( (x), (y), -1 )
#define CSPEED_STRING_NOT_EMPTY(char)     ((*char) != '\0')

extern ZEND_DECLARE_MODULE_GLOBALS(cspeed);

#endif	/* PHP_CSPEED_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
