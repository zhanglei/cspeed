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

#ifndef CSPEED_NET_H
#define CSPEED_NET_H
  
zval *cspeed_request_get(const char *get_key);                      /* Return the $_GET variables for the current request */

char *cspeed_request_get_str_key_val(const char *get_key);          /* Return empty string or the $_GET value */

zval *cspeed_reqeust_server(const char *server_key);                /* Return the $_SERVER variables */

char *cspeed_request_server_str_key_val(const char *server_key);    /* Return the value of the given value-key */

zval *cspeed_request_post(const char *post_key);                    /* Return zval * value of the $_POST  */

char *cspeed_request_post_str_key_val(const char *post_key);        /* Return the value of the given $_POST */

zend_bool cspeed_request_is_method(char *method_name);              /* Return the current request is the given request method name */

extern zend_class_entry *cspeed_request_ce;                                /* Request class entry */

CSPEED_INIT(request);
#endif












/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
