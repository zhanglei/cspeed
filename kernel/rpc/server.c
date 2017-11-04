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

#include "Zend/zend_smart_str.h"
#include "main/SAPI.h"
#include "ext/json/php_json.h"
#include "ext/standard/head.h"

#include "kernel/mvc/controller.h"
#include "kernel/rpc/server.h"
#include "kernel/net/request.h"

/*{{{ All ARGINFO For class Server */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_server_contruct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_server_handle, 0, 0, 1)
    ZEND_ARG_INFO(0, controller_obj)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(Server, __construct)/*{{{ proto Server::__construct() */
{

}/*}}}*/

CSPEED_METHOD(Server, handle)   /*{{{ proto Server::handle */
{
    zval *object;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "o", &object) == FAILURE) {
        return ;
    }
    /* Only accept the POST method */
    if (!cspeed_request_is_method("POST")) {
        php_error_docref(NULL, E_ERROR, "RPC Server only accept POST  request.");
        return ;
    }
    /* Obtain the RAW POST data */
    smart_str raw_data = {0};
    php_stream *s = SG(request_info).request_body;
    while (!php_stream_eof(s)) {
        char buf[512];
        size_t len = php_stream_read(s, buf, sizeof(buf));
        smart_str_appendl(&raw_data, buf, len);
    }
    smart_str_0(&raw_data);
    /* Parsing the data from the request, it must be JSON data */
    zval json_data;
    php_json_decode(&json_data, CSPEED_STRL(ZSTR_VAL(raw_data.s)), 1, PHP_JSON_PARSER_DEFAULT_DEPTH);
    smart_str_free(&raw_data);

    if (ZVAL_IS_NULL(&json_data) || Z_TYPE(json_data) != IS_ARRAY) {
        char *result = "{\"jsonrpc\":\"2.0\", \"error\":{\"message\":\"Invalid request JSON data.\", \"code\":-32700 } \"id\":1 }";
        PHPWRITE(result, strlen(result));
        RETURN_FALSE
    }
    /* The given JSON must contain the following value */
    /* 1. jsonrpc Current must be `2.0` */
    zval *value, *params, result_value;

    array_init(&result_value);
    add_assoc_string(&result_value, "jsonrpc", "2.0" );
    
    if (UNEXPECTED(( value = zend_hash_str_find(Z_ARRVAL(json_data), CSPEED_STRL("id")) ) == NULL)) {
        /* Not has the ID value */
    } else {
        /* The ID key */
        add_assoc_long(&result_value, "id", Z_LVAL_P(value));
        
        if (UNEXPECTED(( value = zend_hash_str_find(Z_ARRVAL(json_data), CSPEED_STRL("method")) ) == NULL)) {
            zval_ptr_dtor(&json_data);
            char *result = "{\"jsonrpc\":\"2.0\",\"error\":{\"message\":\"Missing the method index.\",\"code\":-32602}, \"id\":1 }";
            PHPWRITE( result, strlen(result));
            RETURN_FALSE
        }
        zval temp_func;
        ZVAL_STRING(&temp_func, ZSTR_VAL(strpprintf(0, "%sRpc", Z_STRVAL_P(value))));
        if (CSPEED_METHOD_IN_OBJECT(object, Z_STRVAL_P(&temp_func))){
            zval retval;
            if (UNEXPECTED( ( params = zend_hash_str_find(Z_ARRVAL(json_data), CSPEED_STRL("params")) ) != NULL )) {
                call_user_function(NULL, object, &temp_func, &retval, 1, params);
            } else {
                call_user_function(NULL, object, &temp_func, &retval, 0, NULL);
            }
            zval_ptr_dtor(&temp_func);
            /* The result key */
            zval_add_ref(&retval);
            add_assoc_zval(&result_value, "result", &retval);
            zval_ptr_dtor(&retval);
        } else {
            zval_ptr_dtor(&temp_func);
            zval_ptr_dtor(&json_data);
            char *result = "{\"jsonrpc\":\"2.0\",\"error\":{\"message\":\"Method not found or exists.\",\"code\":-32601 },\"id\":1 }";
            PHPWRITE(result, strlen(result));
            RETURN_FALSE
        }
        /* Encode the value into String */
        smart_str return_string = {0};
        php_json_encode(&return_string, &result_value, 1);
        smart_str_0(&return_string);
        PHPWRITE(ZSTR_VAL(return_string.s), ZSTR_LEN(return_string.s));
        smart_str_free(&return_string);
        zval_ptr_dtor(&json_data);
        zval_ptr_dtor(&result_value);
    }
}/*}}}*/

static const zend_function_entry cspeed_server_functions[] = {    /*{{{ All functions in Server class*/
    CSPEED_ME(Server, __construct, arginfo_cspeed_server_contruct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Server, handle, arginfo_cspeed_server_handle, ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/


CSPEED_INIT(server) /*{{{ proto void server_init() */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\rpc", "Server", cspeed_server_functions);
    cspeed_rpc_server_ce = zend_register_internal_class(&ce);

    zend_do_inheritance(cspeed_rpc_server_ce, cspeed_controller_ce);
}/*}}}*/

