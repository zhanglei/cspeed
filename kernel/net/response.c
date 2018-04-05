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

#include "kernel/net/response.h"
#include "ext/json/php_json.h"
#include "Zend/zend_smart_str.h"

#include "main/SAPI.h"      /* For sapi header function */

/*{{{ All arginfo which the http response header response */

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_set_http_header, 0, 0, 2)
    ZEND_ARG_INFO(0, header_key)
    ZEND_ARG_INFO(0, header_value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_unset_header, 0, 0, 1)
    ZEND_ARG_INFO(0, header_key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_send, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_set_json_content, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_set_raw_content, 0, 0, 1)
    ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_response_set_redirect, 0, 0, 1)
    ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()

/*}}}*/

CSPEED_METHOD(Response, __construct)/*{{{ proto Response::__construct() Default set the status code 200 in header */
{
    zval header_variables;
    array_init(&header_variables);
    zend_update_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_HEADER_VARIABLES), &header_variables);
}
/*}}}*/

CSPEED_METHOD(Response, setHeader)/*{{{ proto Response::setheader($key, $value)*/
{
    zend_string *header_key, *header_value;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "SS", &header_key, &header_value) == FAILURE){
        return ;
    }
    zval *response_headers = zend_read_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_HEADER_VARIABLES), 1, NULL);
    add_assoc_string(response_headers, ZSTR_VAL(header_key), ZSTR_VAL(header_value));
}
/*}}}*/

CSPEED_METHOD(Response, unsetHeader)/*{{{ proto Response::unsetHeader($key)*/
{
    zend_string *header_key;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &header_key) == FAILURE){
        return ;
    }
    zval *response_headers = zend_read_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_HEADER_VARIABLES), 1, NULL);
    zend_hash_del(Z_ARRVAL_P(response_headers), header_key);
}
/*}}}*/

CSPEED_METHOD(Response, send) /*{{{ To send the response data plus the http header to request client */
{
    zval *response_data = zend_read_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_DATA), 1, NULL);
    zval *response_http_headers = zend_read_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_HEADER_VARIABLES), 1, NULL);
    if (response_http_headers && ( Z_TYPE_P(response_http_headers) == IS_ARRAY )) {
        zend_string *var_name;
        zval*var_value;
        sapi_header_line ctr = {0};
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(response_http_headers), var_name, var_value){
            ctr.line_len = spprintf(&(ctr.line), 0, "%s:%s", ZSTR_VAL(var_name), Z_STRVAL_P(var_value));
            ctr.response_code = 200;                                           
            sapi_header_op(SAPI_HEADER_REPLACE, &ctr);                           
            efree(ctr.line);
        } ZEND_HASH_FOREACH_END();
        efree(ctr.line);
    }
    php_write(Z_STRVAL_P(response_data), Z_STRLEN_P(response_data));
}
/*}}}*/

CSPEED_METHOD(Response, setJsonContent)/*{{{ Set the response with the given format  */
{
    zval *json_array;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &json_array) == FAILURE) {
        return ;
    }
    smart_str json_str = { 0 };
    php_json_encode(&json_str, json_array, 0);
    smart_str_0(&json_str);
#if 0
    zval function_name, retval_ptr;
    ZVAL_STRING(&function_name, "json_encode");
    uint32_t param_count = 1;
    zval params[] = { *json_array };
    call_user_function(EG(function_table), NULL, &function_name, &retval_ptr, param_count, params);
#endif
    zend_update_property_string(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_DATA), ZSTR_VAL(json_str.s));
    smart_str_free(&json_str);

    zval *response_headers = zend_read_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_HEADER_VARIABLES), 1, NULL);
    add_assoc_string(response_headers, "Content-Type", "application/json;charset=UTF-8");
}
/*}}}*/

CSPEED_METHOD(Response, setRawContent)/*{{{ Set the response with the given format  */
{
    zval *json_array;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &json_array) == FAILURE) {
        return ;
    }
    if (json_array) {
        zend_update_property(cspeed_response_ce, getThis(), CSPEED_STRL(CSPEED_RESPONSE_DATA), json_array);
    }
}
/*}}}*/

CSPEED_METHOD(Response, redirect)/*{{{ Set the response with the given format  */
{
    zend_string *url;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &url) == FAILURE) {
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(url))) {
        sapi_header_line ctr = {0};
        ctr.line_len    = spprintf(&(ctr.line), 0, "%s %s", "Location:", ZSTR_VAL(url));
        ctr.response_code   = 0;
        if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr) == SUCCESS) {
            efree(ctr.line);            
            RETURN_TRUE
        }
        efree(ctr.line);
    }
    RETURN_FALSE
}
/*}}}*/

/*{{{ All methods for the class Response */
static const zend_function_entry  cspeed_response_functions[] = {
    CSPEED_ME(Response, __construct,   arginfo_response_construct,          ZEND_ACC_PUBLIC)
    CSPEED_ME(Response, setHeader,     arginfo_response_set_http_header,    ZEND_ACC_PUBLIC)
    CSPEED_ME(Response, unsetHeader,   arginfo_response_set_http_header,    ZEND_ACC_PUBLIC)
    CSPEED_ME(Response, send,          arginfo_response_send,               ZEND_ACC_PUBLIC)
    CSPEED_ME(Response, setJsonContent,arginfo_response_set_json_content,   ZEND_ACC_PUBLIC)
    CSPEED_ME(Response, setRawContent, arginfo_response_set_raw_content,    ZEND_ACC_PUBLIC)
    CSPEED_ME(Response, redirect,      arginfo_response_set_redirect,       ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/


/*{{{*/
CSPEED_INIT(response)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\net", "Response", cspeed_response_functions);
    cspeed_response_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(cspeed_response_ce, CSPEED_STRL(CSPEED_RESPONSE_HEADER_VARIABLES), ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_response_ce, CSPEED_STRL(CSPEED_RESPONSE_DATA), "", ZEND_ACC_PRIVATE);
}/*}}}*/







/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */





