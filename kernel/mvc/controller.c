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

#include "kernel/di/di.h"
#include "kernel/mvc/dispatch.h"
#include "kernel/mvc/controller.h"
#include "kernel/tool/component.h"

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_controller_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_controller_dispatch, 0, 0, 1)
  ZEND_ARG_INFO(0, arginfo_cspeed_dispatch)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(Controller, __construct)/*{{{ proto Controller::__construct() */
{
    
}/*}}}*/

/*{{{ proto Controller::dispatch($url)*/
CSPEED_METHOD(Controller, dispatch)
{
    /** 
     * Note that, the $url must contains within three parts: 
     * module/controller/action, if not, use the default instead. 
     */
    zend_string *url;
    zval *module;
    zval *controller;
    zval *action;

    char *module_name, *controller_name, *action_name;

    zval url_data;
    array_init(&url_data);

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &url) == FAILURE) {
        return ;
    }

    if ( !CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(url)) ) {
        php_error_docref(
            NULL,
            E_ERROR,
            "%s",
            "The argument `url` must be a valid string."
        );
    }

    if ( *(ZSTR_VAL(url)) != '/' ) {
        url = strpprintf(
            0,
            "/%s",
            ZSTR_VAL(url)
        );
    }

    php_explode(
        zend_string_init(CSPEED_STRL("/"), 0), 
        url,
        &url_data, 
        ZEND_LONG_MAX
    );

    if ( EXPECTED( 
            (module = zend_hash_index_find(Z_ARRVAL(url_data), 1))
            == NULL
    ) ) {
        module_name = ZSTR_VAL(CSPEED_G(core_router_default_module));
    } else {
        module_name = Z_STRVAL_P(module);
    }

    if (
        EXPECTED(
            ( controller = zend_hash_index_find(Z_ARRVAL(url_data), 2) )
            == NULL
        )
    ) {
        controller_name = ZSTR_VAL(CSPEED_G(core_router_default_controller));
    } else {
        controller_name = Z_STRVAL_P(controller);
    }

    if (
        EXPECTED(
            ( action = zend_hash_index_find(Z_ARRVAL(url_data), 3) )
            == NULL
        )
    ) {
        action_name = ZSTR_VAL(CSPEED_G(core_router_default_action));
    } else {
        action_name = Z_STRVAL_P(action);
    }

    if ( 
        ( strncmp(
            ZSTR_VAL(CSPEED_G(core_router_default_module)), 
            CSPEED_STRL(module_name)
        ) == 0) &&
        ( strncmp(
            ZSTR_VAL(CSPEED_G(core_router_default_controller)), 
            CSPEED_STRL(controller_name)
        ) == 0) &&
        ( strncmp(
            ZSTR_VAL(CSPEED_G(core_router_default_action)), 
            CSPEED_STRL(action_name)
        ) == 0)
    ) {
        php_error_docref(NULL, E_ERROR, "%s", "Can't dispatch the url to current pattern.");
    }


    parse_path_info(&url_data);

} /*}}}*/

/*{{{*/
static const zend_function_entry cspeed_controller_functions[] = {
    CSPEED_ME(
      Controller, 
      __construct, 
      arginfo_cspeed_controller_construct, 
      ZEND_ACC_PUBLIC
    )
    CSPEED_ME(
        Controller,
        dispatch,
        arginfo_cspeed_controller_dispatch,
        ZEND_ACC_PUBLIC
    )

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(controller) /*{{{*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(
      ce, 
      "Cs\\mvc", 
      "Controller", 
      cspeed_controller_functions
    );
    cspeed_controller_ce = zend_register_internal_class(&ce);

    /* Event feature */
    zend_do_inheritance(cspeed_controller_ce, cspeed_component_ce);

    zend_declare_property_null(
      cspeed_controller_ce, 
      CSPEED_STRL(CSPEED_DI_INSTANCE), 
      ZEND_ACC_PUBLIC
    );
    zend_declare_property_null(
      cspeed_controller_ce, 
      CSPEED_STRL(CSPEED_VIEW_INSTANCE), 
      ZEND_ACC_PUBLIC
    );
    zend_declare_property_null(
      cspeed_controller_ce, 
      CSPEED_STRL(CSPEED_ROUTER_INSTANCE), 
      ZEND_ACC_PUBLIC
    );

    zend_declare_class_constant_string(
      cspeed_controller_ce, 
      CSPEED_STRL(EVENT_BEFORE_ACTION), 
      EVENT_BEFORE_ACTION
    );
    zend_declare_class_constant_string(
      cspeed_controller_ce, 
      CSPEED_STRL(EVENT_AFTER_ACTION), 
       EVENT_AFTER_ACTION
    );
}/*}}}*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
