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
#include "php_main.h"
#include "php_cspeed.h"
#include "Zend/zend_API.h"
#include "ext/standard/info.h"

#include "Zend/zend_smart_str.h"
#include "kernel/tool/helper.h"
#include "ext/standard/php_string.h" 
#include "kernel/db/persistent/yacc.tab.h"
#include "kernel/db/persistent/db_adapter.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_db_adapter_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_db_adapter_create_command, 0, 0, 1)
    ZEND_ARG_INFO(0, dbname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_db_adapter_execute, 0, 0, 0)
ZEND_END_ARG_INFO()

/*{{{ proto DbAdapter::__construct(array options = ['sharedKey' => 'xxx', 'hSize' => 10, 'tablename' => 'xx'])*/
CSPEED_METHOD(DbAdapter, __construct)
{
    zval *dbadapter_options;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "a", 
            &dbadapter_options
        ) == FAILURE
    ){
        return ;
    }

    /* If setting the data from the given options */
    if (dbadapter_options && zend_hash_num_elements(Z_ARRVAL_P(dbadapter_options))) {
        zval *option_value;
        /* ShardKey */
        if ( EXPECTED( (option_value = zend_hash_find(
            Z_ARRVAL_P(dbadapter_options), 
            zend_string_init(
                CSPEED_STRL("sharedKey"), 
                0
            ))) != NULL )) {
            /* Store the value to the class/object. */
            /* TODO... */
            zend_update_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(SHARED_KEY), option_value);
        } else {
            cspeed_print_info(
                E_ERROR,
                "Error: DbAdapter must provide `%s` value.",
                "sharedKey"
            );
        }
        /* hSize */
        if ( EXPECTED( (option_value = zend_hash_find(
            Z_ARRVAL_P(dbadapter_options), 
            zend_string_init(
                CSPEED_STRL("hSize"), 
                0
            ))) != NULL )) {
            /* Store the value to the class/object. */
            /* TODO... */
            zend_update_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(HORIZONTAL_SIZE), option_value);
        } else {
            cspeed_print_info(
                E_ERROR,
                "Error: DbAdapter must provide `%s` value.",
                "hSize"
            );
        }
        /* tableName */
        if ( EXPECTED( (option_value = zend_hash_find(
            Z_ARRVAL_P(dbadapter_options), 
            zend_string_init(
                CSPEED_STRL("tableName"), 
                0
            ))) != NULL )) {
            /* Store the value to the class/object. */
            /* TODO... */
            zend_update_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(TABLE_NAME), option_value);        
        } else {
            cspeed_print_info(
                E_ERROR,
                "Error: DbAdapter must provide `%s` value.",
                "tableName"
            );
        }
    } else {
        cspeed_print_info(
            E_ERROR,
            "Error: DbAdapter accepts %s.",
            "array parameters"
        );
    }

}

CSPEED_METHOD(DbAdapter, createCommand)
{
    zend_string *dbname;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &dbname) == FAILURE ) {
        return ;
    }
    zval *table_name = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(TABLE_NAME), 1, NULL);
    char *sql_result = parse_sql(ZSTR_VAL(dbname), Z_STRVAL_P(table_name));
    if (sql_result == NULL) {
        cspeed_print_info(
            E_ERROR,
            "SQL: 1064 Unkown grammer."
        );
        RETURN_ZVAL(getThis(), 1, NULL);
    } else {
        zend_update_property_string(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), sql_result);
        free(sql_result);
        RETURN_ZVAL(getThis(), 1, NULL);
    }
}

CSPEED_METHOD(DbAdapter, execute)
{
    zval *table_name = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(TABLE_NAME), 1, NULL);
    zval *h_size = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(HORIZONTAL_SIZE), 1, NULL);
    zval *shared_key = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(SHARED_KEY), 1, NULL);
    zval *raw_sql = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), 1, NULL);

    if (!raw_sql || ZVAL_IS_NULL(raw_sql)) {
        cspeed_print_info(
            E_ERROR,
            "Please run the createCommand first."
        );
    }

    /* TRIM the RAW_SQL's first space chars */
    zend_string *prepared_sql = php_trim(Z_STR_P(raw_sql), CSPEED_STRL(" "), 3);
    if ( memcmp(ZSTR_VAL(php_string_tolower(prepared_sql)), "select", strlen("select")) == 0 )
    {
        /* Enter the SELECT mode */
        smart_str sql = {0};
        size_t index = 0;
        for (;index < Z_LVAL_P(h_size); index++) {
            /* to instead the true table */
            zend_string *true_table = strpprintf(
                0,
                "%s_%s_%d",
                Z_STRVAL_P(table_name),
                Z_STRVAL_P(shared_key),
                index
            );
            /* to replace */
            zval function_name, ret_val;
            zval search, replace, subject;
            ZVAL_STRING(&search, "?!!?");
            ZVAL_STR(&replace, true_table);
            ZVAL_STR(&subject, prepared_sql);
            zval params[] = { search, replace, subject };
            ZVAL_STRING(&function_name, "str_replace");
            call_user_function(
                EG(function_table), 
                NULL, 
                &function_name, 
                &ret_val, 
                3, 
                params
            );
            zval_ptr_dtor(&function_name);

            smart_str_appends(&sql, Z_STRVAL(ret_val));
            zval_ptr_dtor(&search);
            zval_ptr_dtor(&replace);
            zval_ptr_dtor(&subject);
            zval_ptr_dtor(&ret_val);

            /* Append the UNION */
            if (index < (Z_LVAL_P(h_size) - 1)) {
                smart_str_appends(&sql, " UNION ");
            }
        }
        /* smart_str end */
        smart_str_0(&sql);
        zend_update_property_str(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), sql.s);
        smart_str_free(&sql);
    } 
    else
    {

    }
    RETURN_STR(prepared_sql);
}

static const zend_function_entry db_adapter_functions[] = {
    CSPEED_ME(DbAdapter, __construct, arginfo_cspeed_db_adapter_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, createCommand, arginfo_cspeed_db_adapter_create_command, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, execute, arginfo_cspeed_db_adapter_execute, ZEND_ACC_PUBLIC)

    PHP_FE_END
};

CSPEED_INIT(db_adapter)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(
        ce,
        "Cs\\db\\persistent",
        "DbAdapter",
        db_adapter_functions
    );

    cspeed_db_adapter_ce = zend_register_internal_class(&ce);
    /* Note that, The DbAdapter class is final, can't be derived from. */
    cspeed_db_adapter_ce->ce_flags |= ZEND_ACC_FINAL; 

    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(SHARED_KEY), "", ZEND_ACC_PRIVATE);
    zend_declare_property_long(cspeed_db_adapter_ce, CSPEED_STRL(HORIZONTAL_SIZE), 5, ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(TABLE_NAME), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(RAW_SQL), "", ZEND_ACC_PRIVATE);
}