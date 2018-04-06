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

#include "kernel/rpc/client.h"
#include <curl/curl.h>
#include <curl/easy.h>
#include "Zend/zend_smart_str.h"
#include "ext/json/php_json.h"

/*{{{ All ARGINFO for Client class*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_rpc_client_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_rpc_client_call, 0, 0, 2)
    ZEND_ARG_INFO(0, method_name)
    ZEND_ARG_INFO(0, method_params)
ZEND_END_ARG_INFO()
/*}}}*/

size_t cspeed_client_write_data(char* buffer, size_t size, size_t nmemb, void *stream)/*{{{ Receive the data from the cURL */
{  
    smart_str *data = (smart_str *)stream;  
    size_t len = size * nmemb;
    smart_str_appendl(data, buffer, len);
    return len;  
} /*}}}*/

CSPEED_METHOD(Client, __construct)/*{{{ proto Client::__construct($url)*/
{
    zend_string *url;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &url) == FAILURE) {
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(url))) {
        zend_update_property_string(cspeed_rpc_client_ce, getThis(), CSPEED_STRL(CSPEED_CLIENT_URL), ZSTR_VAL(url));
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter must be a valid URL.");
    }
}/*}}}*/

CSPEED_METHOD(Client, __call)/*{{{ proto Client::call($name, $params)*/
{
    zend_string *method_name;
    zval *method_params = NULL; /* It always to be an array, need json_encode */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|z", &method_name, &method_params) == FAILURE) {
        return ;
    }
    CURL *curl; CURLcode res;
    curl = curl_easy_init();
    smart_str cb_data  = {0};
    /* If init the cURL right. do the post */
    if( curl ) {
        /* Catching the URL setting in the construct method */
        zval *url = zend_read_property(cspeed_rpc_client_ce, getThis(), CSPEED_STRL(CSPEED_CLIENT_URL), 1, NULL);
        curl_easy_setopt(curl, CURLOPT_URL, Z_STRVAL_P(url));

        /* The data send to the Server */
        zval post_data;
        array_init(&post_data);
        add_assoc_string(&post_data, "jsonrpc", "2.0");

        /* RPC-id */
        zval *id = zend_read_property(cspeed_rpc_client_ce, getThis(), CSPEED_STRL(CSPEED_CLIENT_POST_ID), 1, NULL);
        add_assoc_zval(&post_data, "id", id);

        /* RPC-method */
        add_assoc_str(&post_data, "method", method_name);

        /* RPC-params */
        if ( method_params ) {
            add_assoc_zval(&post_data, "params", method_params);
        } else {
            add_assoc_string(&post_data, "params", "");
        }

        // /* JSON-encode */
        smart_str post_str = {0};
        php_json_encode(&post_str, &post_data, 0);
        smart_str_0(&post_str);
        struct curl_slist *plist = curl_slist_append(NULL, "Content-Type:application/json;charset=UTF-8");  
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER,      plist);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,      ZSTR_VAL(post_str.s));
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE,   ZSTR_LEN(post_str.s));
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,   cspeed_client_write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA,       (void *)&cb_data);
        curl_easy_setopt(curl, CURLOPT_POST,            1L);
        smart_str_free(&post_str);
        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);
        /* Check for errors */ 
        if(res != CURLE_OK) {
            curl_easy_cleanup(curl);
            php_error_docref(NULL, E_ERROR, "Calling the curl_easy_perform() failed: %s.", curl_easy_strerror(res));
        }
        /* After each operation increment the id by 1 */
        zend_update_property_long(cspeed_rpc_client_ce, getThis(), CSPEED_STRL(CSPEED_CLIENT_POST_ID), Z_LVAL_P(id) + 1);
    }
    /* always cleanup */ 
    curl_easy_cleanup(curl);
    curl_global_cleanup();
    smart_str_0(&cb_data);
    /**/
    ZVAL_STR(return_value, zend_string_copy(cb_data.s));
    /* return_value is the data */
    smart_str_free(&cb_data);
}/*}}}*/

static const zend_function_entry cspeed_rpc_client_functions[] = {  /*{{{ All methods to the Module class Client */
    CSPEED_ME(Client, __construct, arginfo_cspeed_rpc_client_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Client, __call, arginfo_cspeed_rpc_client_call, ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

CSPEED_INIT(client)/*{{{ proto void client_init()*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\rpc", "Client", cspeed_rpc_client_functions);
    cspeed_rpc_client_ce = zend_register_internal_class(&ce);

    /* The URL which to send the RPC request */
    zend_declare_property_string(cspeed_rpc_client_ce, CSPEED_STRL(CSPEED_CLIENT_URL), "", ZEND_ACC_PRIVATE);
    zend_declare_property_long(cspeed_rpc_client_ce, CSPEED_STRL(CSPEED_CLIENT_POST_ID), 1, ZEND_ACC_PRIVATE);
}/*}}}*/



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
