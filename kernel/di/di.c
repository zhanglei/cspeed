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

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(argin_info_cspeed_di_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_di_set, 0, 0, 2)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, object)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_di_get, 0, 0, 1)
  ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

/*}}}*/

void 
initialise_di_object_properties(zval *di_object)/*{{{ Initialise the Di object's properties */
{
    zval objects;
    array_init(&objects);
    zend_update_property(
      cspeed_di_ce, 
      di_object, 
      CSPEED_STRL(CSPEED_DI_OBJECT), 
      &objects
    );
    zval_ptr_dtor(&objects);
}/*}}}*/

CSPEED_METHOD(Di, __construct)  /*{{{ proto Di::construct() */
{
    initialise_di_object_properties(getThis());
}/*}}}*/

CSPEED_METHOD(Di, set) /*{{{ proto Di::set($key, function(){return new stdClass();}) */
{
    zend_string *key;
    zend_fcall_info zfi;
    zend_fcall_info_cache zfic;

    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "Sf", 
            &key, 
            &zfi, 
            &zfic,
            &zfi.param_count, 
            &zfi.params
        ) == FAILURE
    ) {
        return ;
    }

    if (!CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(key))) {
        php_error_docref(
          NULL, 
          E_ERROR, 
          "Parameter one must be a valid string index key."
        );
        RETURN_FALSE
    }
    zval retval;
    zfi.retval = &retval;
    zend_call_function(&zfi, &zfic);

    if (Z_TYPE(retval) == IS_OBJECT) {
        zval *objects = zend_read_property(
          cspeed_di_ce, 
          getThis(), 
          CSPEED_STRL(CSPEED_DI_OBJECT), 
          1, 
          NULL
        );
        add_assoc_zval(
          objects, 
          ZSTR_VAL(key), 
          &retval
        );
        RETURN_TRUE
    } else {
        php_error_docref(
          NULL, 
          E_ERROR, 
          "Parameter must return a class object."
        );
        RETURN_FALSE
    }

}/*}}}*/

CSPEED_METHOD(Di, get) /*{{{ proto Di::get($key) */
{
    zend_string *key;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "S", 
            &key
        ) == FAILURE
    ) {
        return ;
    }

    if (!CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(key))) {
        php_error_docref(
          NULL, 
          E_ERROR, 
          "Parameter must be a valid string key."
        );
        RETURN_FALSE
    }
    zval *objects = zend_read_property(
      cspeed_di_ce, 
      getThis(), 
      CSPEED_STRL(CSPEED_DI_OBJECT), 
      1, 
      NULL
    );
    zval *object = zend_hash_find(
      Z_ARRVAL_P(objects), 
      key
    );
    if (object) {
        RETURN_ZVAL(object, 1, NULL);
    } else {
        php_error_docref(
          NULL, 
          E_WARNING, 
          "Class didn't has the value associated with '%s' index key.", 
          ZSTR_VAL(key)
        );
        RETURN_FALSE
    }
}/*}}}*/

static const zend_function_entry cspeed_di_functions[] = { /*{{{ All methods of the class Di */
    CSPEED_ME(Di, __construct, argin_info_cspeed_di_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Di, set, arginfo_cspeed_di_set, ZEND_ACC_PUBLIC)
    CSPEED_ME(Di, get, arginfo_cspeed_di_get, ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(di) /*{{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(
        ce, 
        "Cs\\di", 
        "Di", 
        cspeed_di_functions
    );
    cspeed_di_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(
        cspeed_di_ce, 
        CSPEED_STRL(CSPEED_DI_OBJECT), 
        ZEND_ACC_PROTECTED
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
