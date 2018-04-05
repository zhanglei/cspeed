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

#include "kernel/net/request.h"

zval *cspeed_request_get(const char *get_key)/*{{{ Get zval from $_GET */
{
    /* Get the given server path-info value */
    zval *get_variables = &PG(http_globals)[TRACK_VARS_GET];
    if (!get_key) {
        return get_variables;
    } else {
        /* const HashTable *ht, zend_string *key */
        zend_string *cspeed_get_key = zend_string_init(get_key, strlen(get_key), 0);
        zval *result = zend_hash_find(Z_ARRVAL_P(get_variables), cspeed_get_key);
        zend_string_release(cspeed_get_key);
        return result;
    }
}/*}}}*/

char *cspeed_request_get_str_key_val(const char *get_key) /*{{{ GET $_GET value*/
{
    zval *get_value = cspeed_request_get(get_key);
    if (get_value == NULL) {
        php_error_docref(NULL, E_NOTICE, "Unknow index %s", get_key);
        return "";
    } else {
        return Z_STRVAL_P(get_value);
    }
}/*}}}*/

zval *cspeed_reqeust_server(const char *server_key) /*{{{ Get zval * from $_SERVER */
{
    if (PG(auto_globals_jit)) {
        zend_string *server_str = zend_string_init(ZEND_STRL("_SERVER"), 0);
        zend_is_auto_global(server_str);
        zend_string_release(server_str);
    }
    /* Get the given server path-info value */
    zval *server_variables = &PG(http_globals)[TRACK_VARS_SERVER];
    if (!server_key) {
        return server_variables;
    } else {
        /* const HashTable *ht, zend_string *key */
        zend_string *cspeed_server_key = zend_string_init(server_key, strlen(server_key), 0);
        zval *result = zend_hash_find(Z_ARRVAL_P(server_variables), cspeed_server_key);
        zend_string_release(cspeed_server_key);
        return result;
    }
}/*}}}*/

char *cspeed_request_server_str_key_val(const char *server_key) /*{{{ GET $_SERVER value */
{
    zval *server_value = cspeed_reqeust_server(server_key);
    if (server_value == NULL) {
        php_error_docref(NULL, E_NOTICE, "Unknow index %s", server_key);
        return "";
    } else {
        return Z_STRVAL_P(server_value);
    }
}/*}}}*/

zval *cspeed_request_post(const char *post_key)/*{{{ Get zval from $_POST */
{
    /* Get the given server path-info value */
    zval *post_variables = &PG(http_globals)[TRACK_VARS_POST];
    if (!post_key) {
        return post_variables;
    } else {
        /* const HashTable *ht, zend_string *key */
        zend_string *cspeed_post_key = zend_string_init(post_key, strlen(post_key), 0);
        zval *result = zend_hash_find(Z_ARRVAL_P(post_variables), cspeed_post_key);
        zend_string_release(cspeed_post_key);
        return result;
    }
}/*}}}*/

char *cspeed_request_post_str_key_val(const char *post_key) /*{{{ GET $_POST value*/
{
    zval *post_value = cspeed_request_post(post_key);
    if (post_value == NULL) {
        php_error_docref(NULL, E_NOTICE, "Unknow index %s", post_key);
        return "";
    } else {
        return Z_STRVAL_P(post_value);
    }
}/*}}}*/

zend_bool cspeed_request_is_method(char *method_name) /*{{{ Return bool whether the REQEUST_METHOD match or not */
{
    char *request_method = cspeed_request_server_str_key_val("REQUEST_METHOD");
    if ( memcmp(request_method, method_name, strlen(request_method)) == 0) {
        return TRUE;
    }
    return FALSE;
}/*}}}*/

/*}}}*/

/*{{{ Request class ARG-INFO */

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_http_host, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_http_user_agent, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_server_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_server_port, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_server_addr, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_remote_port, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_remote_addr, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_request_scheme, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_server_protocol, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_document_root, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_document_uri, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_request_uri, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_script_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_path_info, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_isget, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_ispost, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_isput, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_ispatch, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_isdelete, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_ishead, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_isoptions, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_query_string, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get, 0, 0, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_request_get_post, 0, 0, 0)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

/*}}}*/


/*{{{ proto Request::getHttpHost()
 */
CSPEED_METHOD(Request, getHttpHost)
{
    RETURN_STRING(cspeed_request_server_str_key_val("HTTP_HOST"));
}/*}}}*/

/*{{{ proto Request::getHttpUserAgent()
 */
CSPEED_METHOD(Request, getHttpUserAgent)
{
    RETURN_STRING(cspeed_request_server_str_key_val("HTTP_USER_AGENT"));
}/*}}}*/

/*{{{ proto Request::getServerName()
 */
CSPEED_METHOD(Request, getServerName)
{
    RETURN_STRING(cspeed_request_server_str_key_val("SERVER_NAME"));
}/*}}}*/

/*{{{ proto Request::getServerAddr()
 */
CSPEED_METHOD(Request, getServerAddr)
{
    RETURN_STRING(cspeed_request_server_str_key_val("SERVER_ADDR"));
}/*}}}*/

/*{{{ proto Request::getRemotePort()
 */
CSPEED_METHOD(Request, getRemotePort)
{
    RETURN_STRING(cspeed_request_server_str_key_val("REMOTE_PORT"));
}/*}}}*/

/*{{{ proto Request::getRemoteAddr()
 */
CSPEED_METHOD(Request, getRemoteAddr)
{
    RETURN_STRING(cspeed_request_server_str_key_val("REMOTE_ADDR"));
}/*}}}*/

/*{{{ proto Request::getReqeustScheme()
 */
CSPEED_METHOD(Request, getReqeustScheme)
{
    RETURN_STRING(cspeed_request_server_str_key_val("REQUEST_SCHEME"));
}/*}}}*/

/*{{{ proto Request::getServerProtocol()
 */
CSPEED_METHOD(Request, getServerProtocol)
{
    RETURN_STRING(cspeed_request_server_str_key_val("SERVER_PROTOCOL"));
}/*}}}*/

/*{{{ proto Request::getDocumentRoot()
 */
CSPEED_METHOD(Request, getDocumentRoot)
{
    RETURN_STRING(cspeed_request_server_str_key_val("DOCUMENT_ROOT"));
}/*}}}*/


/*{{{ proto Request::getDocumentUri()
 */
CSPEED_METHOD(Request, getDocumentUri)
{
    RETURN_STRING(cspeed_request_server_str_key_val("DOCUMENT_URI"));
}/*}}}*/



/*{{{ proto Request::getQueryString()
 */
CSPEED_METHOD(Request, getQueryString)
{
    RETURN_STRING(cspeed_request_server_str_key_val("QUERY_STRING"));
}/*}}}*/


/*{{{ proto Request::getRequestUri()
 */
CSPEED_METHOD(Request, getRequestUri)
{
    RETURN_STRING(cspeed_request_server_str_key_val("REQUEST_URI"));
}/*}}}*/


/*{{{ proto Request::getScriptName()
 */
CSPEED_METHOD(Request, getScriptName)
{
    RETURN_STRING(cspeed_request_server_str_key_val("SCRIPT_NAME"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, getPathInfo)
{
    RETURN_STRING(cspeed_request_server_str_key_val("PATH_INFO"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isGet)
{
    RETURN_BOOL(cspeed_request_is_method("GET"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isPost)
{
    RETURN_BOOL(cspeed_request_is_method("POST"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isPut)
{
    RETURN_BOOL(cspeed_request_is_method("PUT"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isPatch)
{
    RETURN_BOOL(cspeed_request_is_method("PATCH"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isDelete)
{
    RETURN_BOOL(cspeed_request_is_method("DELETE"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isHead)
{
    RETURN_BOOL(cspeed_request_is_method("HEAD"));
}/*}}}*/

/*{{{ proto Request::getPathInfo()
 */
CSPEED_METHOD(Request, isOptions)
{
    RETURN_BOOL(cspeed_request_is_method("OPTIONS"));
}/*}}}*/

/*{{{ proto Request::get()
 */
CSPEED_METHOD(Request, get)
{
    zend_string *get_key = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|S", &get_key) == FAILURE) {
        return ;
    }
    zval *get_data = cspeed_request_get( get_key == NULL ? NULL : ZSTR_VAL(get_key) );
    if (get_data) {
        RETURN_ZVAL(get_data, 1, 0);
    } else {
        RETURN_NULL()
    }
}/*}}}*/

/*{{{ proto Request::getPost()
 */
CSPEED_METHOD(Request, getPost)
{
    zend_string *post_key = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|S", &post_key) == FAILURE) {
        return ;
    }
    zval *post_data = cspeed_request_post( post_key == NULL ? NULL : ZSTR_VAL( post_key ) );
    if (post_data) {
        RETURN_ZVAL(post_data, 1, 0);
    } else {
        RETURN_NULL()
    }
}/*}}}*/

/*}}}*/

/*{{{ All functions of the class Request */
static const zend_function_entry cspeed_request_functions[] = {
    CSPEED_ME(Request, getHttpHost,         arginfo_request_get_http_host,          ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getHttpUserAgent,    arginfo_request_get_http_user_agent,    ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getServerName,       arginfo_request_get_server_name,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getServerAddr,       arginfo_request_get_server_addr,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getRemotePort,       arginfo_request_get_remote_port,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getRemoteAddr,       arginfo_request_get_remote_addr,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getReqeustScheme,    arginfo_request_get_request_scheme,     ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getServerProtocol,   arginfo_request_get_server_protocol,    ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getDocumentRoot,     arginfo_request_get_document_root,      ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getRequestUri,       arginfo_request_get_request_uri,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getScriptName,       arginfo_request_get_script_name,        ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getPathInfo,         arginfo_request_get_path_info,          ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getQueryString,      arginfo_request_get_query_string,       ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, isGet,               arginfo_request_isget,                  ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, isPut,               arginfo_request_isput,                  ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, isPatch,             arginfo_request_ispatch,                ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, isDelete,            arginfo_request_isdelete,               ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, isHead,              arginfo_request_ishead,                 ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, isOptions,           arginfo_request_isoptions,              ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, get,                 arginfo_request_get,                    ZEND_ACC_PUBLIC)
    CSPEED_ME(Request, getPost,             arginfo_request_get_post,               ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/*}}}*/

/*{{{ The loading module when web server load the cspeed extension */
CSPEED_INIT(request)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\net", "Request", cspeed_request_functions);
    cspeed_request_ce = zend_register_internal_class(&ce);
}
/*}}}*/








/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */


