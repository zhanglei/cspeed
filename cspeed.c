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

#include "kernel/db/pdo.h"

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
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
    di_init();
    app_init();
    view_init();

    /* NOTE THAT: The model_interface_init() must before the mysql_init() method */
    model_interface_init();
    mysql_init();
    
    /* Other init function */
    request_init();
    response_init();
    controller_init();

    /* ActiveRecord model */
    model_init();

    config_init();

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(cspeed)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
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
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(cspeed)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(cspeed)
{
	php_info_print_table_start();
    php_info_print_table_header(2, "CSpeed support", "enabled");
    php_info_print_table_header(2, "Author", "Josin<774542602@qq.com|www.supjos.cn>");
	php_info_print_table_header(2, "Version", PHP_CSPEED_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ cspeed_functions[]
 *
 * Every user visible function must have an entry in cspeed_functions[].
 */
const zend_function_entry cspeed_functions[] = {
	PHP_FE(getCSpeedVersion,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in cspeed_functions[] */
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
	PHP_RINIT(cspeed),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(cspeed),	/* Replace with NULL if there's nothing to do at request end */
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
