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
#include "kernel/di/di.h"
#include "kernel/mvc/router.h"

ZEND_DECLARE_MODULE_GLOBALS(cspeed)

/* True global resources - no need for thread safety here */
static int le_cspeed;

/* {{{ proto string getCSpeedVersion()
   Return the CSpeed version */
PHP_FUNCTION(getCSpeedVersion)
{
    RETURN_STRING(PHP_CSPEED_VERSION);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(cspeed)
{
#if defined(COMPILE_DL_CSPEED) && defined(ZTS)
    /* Allocate the memory for the system to use, only for ZTS */
    ZEND_INIT_MODULE_GLOBALS(cspeed, NULL, NULL);
#endif
    /* Initialise the all core class for the CSpeed engine */
    /* Event support */
    component_init();

    /* The IOC object class */
    object_factory_init();

    /* Bootstrap interface */
    bootinit_init();

    /* Di|App|View */
    di_init();
    app_init();
    view_init();

    /* NOTE THAT: The model_interface_init() must before the adapter_init() method */
    model_interface_init();
    adapter_init();
    
    /* Other init function */
    request_init();
    response_init();
    controller_init();

    /* ActiveRecord model */
    model_init();

    /* Config class */
    config_init();

    /* Router class */
    router_init();

    /* RPC class server */
    server_init();
    client_init();

    /* CLI */
    task_init();
    
    /* After initialise, return SUCCESS means OK */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cspeed)
{
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(cspeed)
{
#if defined(COMPILE_DL_CSPEED) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    CSPEED_G(core_application) = zend_string_init(CSPEED_STRL(CORE_APPLICATION), 0);
    CSPEED_G(core_bootstrap) = zend_string_init(CSPEED_STRL(CORE_BOOTSTRAP), 0);
    CSPEED_G(core_router_default_module) = zend_string_init(CSPEED_STRL(CORE_ROUTER_DEFAULT_MODULE), 0);
    CSPEED_G(core_router_default_controller) = zend_string_init(CSPEED_STRL(CORE_ROUTER_DEFAULT_CONTROLLER), 0);
    CSPEED_G(core_router_default_action) = zend_string_init(CSPEED_STRL(CORE_ROUTER_DEFAULT_ACTION), 0);
    CSPEED_G(core_view_ext) = zend_string_init(CSPEED_STRL(CORE_VIEW_EXT), 0);
    CSPEED_G(core_view_auto_render) = zend_string_init(CSPEED_STRL(CORE_VIEW_AUTO_RENDER), 0);
    CSPEED_G(core_url_pattern) = zend_string_init(CSPEED_STRL(CORE_URL_PATTERN), 0);

    /* Initialise the Router object & Di object */
    zval di_object, router_object;
    object_init_ex(&di_object, cspeed_di_ce);
    initialise_di_object_properties(&di_object);

    object_init_ex(&router_object, cspeed_router_ce);
    initialise_router_object_properties(&router_object);

    CSPEED_G(di_object) = Z_OBJ(di_object);
    CSPEED_G(router_object) = Z_OBJ(router_object);

    /* Return the status */
    return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cspeed)
{
    zend_string_release(CSPEED_G(core_application));
    zend_string_release(CSPEED_G(core_bootstrap));
    zend_string_release(CSPEED_G(core_router_default_module));
    zend_string_release(CSPEED_G(core_router_default_controller));
    zend_string_release(CSPEED_G(core_router_default_action));
    zend_string_release(CSPEED_G(core_view_ext));
    zend_string_release(CSPEED_G(core_view_auto_render));
    zend_string_release(CSPEED_G(core_url_pattern));

    /* free the memory */
    pefree(CSPEED_G(di_object), GC_FLAGS(CSPEED_G(di_object)) & IS_STR_PERSISTENT);
    pefree(CSPEED_G(router_object), GC_FLAGS(CSPEED_G(router_object)) & IS_STR_PERSISTENT);
    CSPEED_G(di_object) = NULL;
    CSPEED_G(router_object) = NULL;

    /* Return SUCCESS */
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cspeed)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "CSpeed support", "enabled");
    php_info_print_table_header(2, "Author", "Josin <774542602@qq.com|www.supjos.cn>");
    php_info_print_table_header(2, "Version", PHP_CSPEED_VERSION);
    php_info_print_table_end();
}
/* }}} */

/* {{{ cspeed_functions[]
 *
 * Every user visible function must have an entry in cspeed_functions[].
 */
const zend_function_entry cspeed_functions[] = {
    PHP_FE(getCSpeedVersion,    NULL)       /* For testing, remove later. */
    PHP_FE_END  /* Must be the last line in cspeed_functions[] */
};
/* }}} */

/* {{{ cspeed_module_entry
 */
zend_module_entry cspeed_module_entry = {
    STANDARD_MODULE_HEADER,
    "cspeed",
    cspeed_functions,
    PHP_MINIT(cspeed),
    PHP_MSHUTDOWN(cspeed),
    PHP_RINIT(cspeed),      /* Replace with NULL if there's nothing to do at request start */
    PHP_RSHUTDOWN(cspeed),  /* Replace with NULL if there's nothing to do at request end */
    PHP_MINFO(cspeed),
    PHP_CSPEED_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_CSPEED
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(cspeed)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
