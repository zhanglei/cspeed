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
#include "kernel/tool/configs.h"

#include "kernel/tool/helper.h"

#include <unistd.h>         /* Access function */
#include <fcntl.h>          /* Access function */

/*{{{ ARGINFO */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_config_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_getConfigs, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_getConfig, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_load_config, 0, 0, 1)
    ZEND_ARG_INFO(0, file)
ZEND_END_ARG_INFO()
/*}}}*/

CSPEED_METHOD(Config, __construct)/*{{{ proto Config::__construct()*/
{

}/*}}}*/

CSPEED_METHOD(Config, loadConfig)/*{{{ proto Config::loadConfig()*/
{
    zend_string *file;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &file) == FAILURE) {
        return ;
    }
    
    char path[MAXPATHLEN];
    zend_string *real_inifile_path = strpprintf(0, "%s/%s", cspeed_get_cwd(path), ZSTR_VAL(file));
    if (access(ZSTR_VAL(real_inifile_path), F_OK) != -1) {
        zval ini_values;
        cspeed_parse_ini_file(ZSTR_VAL(real_inifile_path), NULL, NULL, 1, &ini_values);
        /* update the value to the property */
        zend_update_property(cspeed_config_ce, getThis(), CSPEED_STRL(CSPEED_CONFIG_VARIABLES), &ini_values);
        zval_ptr_dtor(&ini_values);
    }
}/*}}}*/

CSPEED_METHOD(Config, getConfigs) /*{{{ proto Config::getConfigs() */
{
    zval *configs = zend_read_property(cspeed_config_ce, getThis(), CSPEED_STRL(CSPEED_CONFIG_VARIABLES), 1, NULL);
    RETURN_ZVAL(configs, 1, NULL);
} /*}}}*/

CSPEED_METHOD(Config, getConfig) /*{{{ proto Config::getConfig($configKey)*/
{
    zend_string *config_key;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &config_key) == FAILURE) {
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(config_key))) {
        zval *configs = zend_read_property(cspeed_config_ce, getThis(), CSPEED_STRL(CSPEED_CONFIG_VARIABLES), 1, NULL);
        zval *key_value = zend_hash_find(Z_ARRVAL_P(configs), config_key);
        RETURN_ZVAL(key_value, 1, NULL);
    }
    RETURN_NULL()
}/*}}}*/

static const zend_function_entry cspeed_config_functions[] = { /*{{{*/
    CSPEED_ME(Config, __construct, arginfo_cspeed_config_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Config, loadConfig,  arginfo_cspeed_load_config,      ZEND_ACC_PUBLIC)
    CSPEED_ME(Config, getConfigs,  arginfo_cspeed_getConfigs,       ZEND_ACC_PUBLIC)
    CSPEED_ME(Config, getConfig,   arginfo_cspeed_getConfig,        ZEND_ACC_PUBLIC)
    PHP_FE_END
};/*}}}*/

CSPEED_INIT(config)/*{{{ Loading the Config */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\tool", "Config", cspeed_config_functions);
    cspeed_config_ce = zend_register_internal_class(&ce);

    zend_declare_property_null(cspeed_config_ce, CSPEED_STRL(CSPEED_CONFIG_VARIABLES), ZEND_ACC_PRIVATE);
}/*}}}*/










/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
