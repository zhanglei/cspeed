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

#include "kernel/tool/component.h"

int trigger_events(zval *object_ptr, zend_string *event_name)/*{{{ To raise the event from the event list*/
{
    /* Call all the functions in the array_queue_lists */
    zval *events = zend_read_property(
        Z_OBJCE_P(object_ptr), 
        object_ptr, 
        CSPEED_STRL(CSPEED_COMPONENT_EVENTS), 
        1, 
        NULL
    );
    if (ZVAL_IS_NULL(events)) {
        return FALSE;
    }
    zval *event_lists = zend_hash_find(Z_ARRVAL_P(events), event_name);
    if (event_lists){
        zend_ulong index; zval *var_value;
        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(event_lists), index, var_value) {
            zval retval;
            zval params[] = { *object_ptr };
            call_user_function(
                EG(function_table), 
                NULL, 
                var_value, 
                &retval, 
                1, 
                params
            );
            zval_ptr_dtor(&retval);
            zval_ptr_dtor(params);
        } ZEND_HASH_FOREACH_END();
        return TRUE;
    }
    return FALSE;
}/*}}}*/

/*{{{ ARGINFO For the CSpeed */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_component_construct, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_component_on, 0, 0, 2)
    ZEND_ARG_INFO(0, event_name)
    ZEND_ARG_INFO(0, callback_function)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_component_off, 0, 0, 1)
    ZEND_ARG_INFO(0, event_name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_component_trigger, 0, 0, 1)
    ZEND_ARG_INFO(0, event_name)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(Component, __construct)/*{{{ proto Component::__construct()*/
{

}/*}}}*/

CSPEED_METHOD(Component, on)/*{{{ proto Component::on(EVENT_LOGIN, function(){})*/
{
    zend_string *event_name; zval *func_name_or_obj;
    if (zend_parse_parameters(
        ZEND_NUM_ARGS() TSRMLS_CC,
        "Sz", 
        &event_name, 
        &func_name_or_obj
    ) == FAILURE) {
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(event_name))) {
        /* Check the property is null or not, if null initialise it before using */
        zval *events = zend_read_property(
            cspeed_component_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_COMPONENT_EVENTS), 
            1, 
            NULL
        );
        if (ZVAL_IS_NULL(events)) {
            zval temp_events;
            array_init(&temp_events);
            zend_update_property(
                cspeed_component_ce, 
                getThis(), 
                CSPEED_STRL(CSPEED_COMPONENT_EVENTS), 
                &temp_events
            );
        }
        /* Find if there exist the given event_name */
        zend_bool is_exist = zend_hash_exists(Z_ARRVAL_P(events), event_name);
        if (is_exist) {
            /* if exists, to add the given callback to the current exist array_list */
            zval *event_lists = zend_hash_find(Z_ARRVAL_P(events), event_name);
            zval_add_ref(func_name_or_obj);
            add_next_index_zval(
                event_lists, 
                func_name_or_obj
            );
        } else {
            /* Not exist, to initialise it and then append the callback */
            zval event_array;
            array_init(&event_array);
            zval_add_ref(func_name_or_obj);
            add_next_index_zval(
                &event_array, 
                func_name_or_obj
            );
            zval_add_ref(&event_array);
            add_assoc_zval(
                events, 
                ZSTR_VAL(event_name), 
                &event_array
            );
        }
        RETURN_TRUE
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(Component, off)/*{{{ proto Component::off(EVENT_LOGIN, $handler)*/
{
    zend_string *event_name; zval *handler = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|z", &event_name, &handler) == FAILURE) {
        return ;
    }

    if(CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(event_name))) {
        /* if handler null, unset all event_name handler, otherwise remove the given handler */
        zval *events = zend_read_property(
            cspeed_component_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_COMPONENT_EVENTS), 
            1, 
            NULL
        );
        if (ZVAL_IS_NULL(events)) {
            RETURN_FALSE
        }
        zval *event_lists = zend_hash_find(Z_ARRVAL_P(events), event_name);
        if (handler) {
            /* Given handler */
            zend_ulong index; zval *var_value;
            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(event_lists), index, var_value) {
                if (Z_TYPE_P(var_value) == IS_STRING) {
                    if (memcmp(Z_STRVAL_P(var_value), Z_STRVAL_P(handler), Z_STRLEN_P(handler)) == 0) {
                        zend_hash_index_del(Z_ARRVAL_P(event_lists), index);
                        RETURN_TRUE
                    }
                }
                if ( Z_TYPE_P(var_value) == IS_OBJECT && Z_TYPE_P(handler) == IS_OBJECT ) {
                    if ( Z_OBJ_P(var_value) == Z_OBJ_P(handler) ) {
                        zend_hash_index_del(Z_ARRVAL_P(event_lists), index);
                        RETURN_TRUE
                    }
                }
                if ( Z_TYPE_P(var_value) == IS_ARRAY && Z_TYPE_P(handler) == IS_ARRAY) {
                    zval *object = zend_hash_index_find(Z_ARRVAL_P(var_value), 0);
                    zval *method = zend_hash_index_find(Z_ARRVAL_P(var_value), 1);

                    zval *remove_object = zend_hash_index_find(Z_ARRVAL_P(handler), 0);
                    zval *remove_method = zend_hash_index_find(Z_ARRVAL_P(handler), 1);
                    if (memcmp(ZSTR_VAL(Z_OBJ_P(object)->ce->name), ZSTR_VAL(Z_OBJ_P(remove_object)->ce->name), 
                        ZSTR_LEN(Z_OBJ_P(object)->ce->name)) == 0) {
                        zend_hash_index_del(Z_ARRVAL_P(event_lists), index);
                        RETURN_TRUE
                    }
                }
            } ZEND_HASH_FOREACH_END();
        } else {
            /* Not given handler */
            zend_hash_del(Z_ARRVAL_P(events), event_name);
            RETURN_TRUE
        }
    } else {
        php_error_docref(
            NULL, 
            E_ERROR, 
            "Parameter one must be an valid string index."
        );
        return ;
    }

}/*}}}*/

CSPEED_METHOD(Component, trigger)/*{{{ proto Component::trigger(event_name)*/
{
    zend_string *event_name; 
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &event_name) == FAILURE) {
        return ;
    }

    if(CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(event_name))) {
        /* Call all the functions in the array_queue_lists */
        if (trigger_events(getThis(), event_name)) {
            RETURN_TRUE
        }
    }
    RETURN_FALSE
}/*}}}*/

static const zend_function_entry cspeed_component_functions[] = { /*{{{ All functions for the Components */
    CSPEED_ME(Component, __construct,   arginfo_cspeed_component_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Component, on,            arginfo_cspeed_component_on,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Component, off,           arginfo_cspeed_component_off,       ZEND_ACC_PUBLIC)
    CSPEED_ME(Component, trigger,       arginfo_cspeed_component_trigger,   ZEND_ACC_PUBLIC)
    PHP_FE_END
};/*}}}*/


CSPEED_INIT(component)/* void component_init() */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\tool", "Component", cspeed_component_functions);
    cspeed_component_ce = zend_register_internal_class(&ce);

    /* To store all events */
    zend_declare_property_null(
        cspeed_component_ce, 
        CSPEED_STRL(CSPEED_COMPONENT_EVENTS), 
        ZEND_ACC_PROTECTED
    );
}/*}}}*/

