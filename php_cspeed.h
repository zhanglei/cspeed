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

#define PHP_CSPEED_VERSION "1.2.2"

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
  zend_array  global_config;
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

#define CSPEED_METHOD                 PHP_METHOD
#define CSPEED_ME                     PHP_ME
#define CSPEED_MINT_FUNCTION          PHP_MINIT_FUNCTION

#define CSPEED_INIT(function_name)    void function_name##_init()

#define CSPEED_STRL(str)              (str), strlen(str)

#define CSPEED_METHOD_IN_OBJECT(object, method_name) ( ( Z_OBJ_HT_P(object)->get_method(&Z_OBJ_P(object),\
 zend_string_init(CSPEED_STRL(method_name), 0), NULL)) != NULL ) /* Judge whether the method is in object */

#define CSPEED_STRING_NOT_EMPTY(char)     ((*char) != '\0')        /* Judge the string not empty */

#endif	/* PHP_CSPEED_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
