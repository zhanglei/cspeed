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
#include "ext/standard/info.h"

#include "kernel/db/pdo.h"              /* Use the PDO wrapper functions */
#include "kernel/db/adapter.h"
#include "kernel/tool/helper.h"
#include "kernel/db/model_interface.h"

#include "zend_smart_str.h"             /* Use the smart_str */

#include <ctype.h>


char *get_build_sql(zval *this) /*{{{ Return the builded SQL*/
{
    zval *select = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_SELECT), 
        1, 
        NULL
    );
    zval *from = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_FROM), 
        1, 
        NULL
    );
    zval *where = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
        1, 
        NULL
    );
    zval *group_by = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_GROUP_BY), 
        1, 
        NULL
    );
    zval *limit = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_LIMIT), 
        1, 
        NULL
    );
    zval *having = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_HAVING), 
        1, 
        NULL
    );
    zval *order_by = zend_read_property(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_ORDER_BY), 
        1, 
        NULL
    );

    return ZSTR_VAL(
        strpprintf(
            0, 
            "SELECT %s FROM %s%s%s%s%s%s", 
            Z_STRVAL_P(select), 
            Z_STRVAL_P(from), 
            Z_STRVAL_P(where), 
            Z_STRVAL_P(group_by), 
            Z_STRVAL_P(having), 
            Z_STRVAL_P(order_by), 
            Z_STRVAL_P(limit)
        )
    );
}/*}}}*/

void reset_adapter_sql(zval *this)/*{{{ Rest the SQL for the next running */
{
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_SELECT), 
        ""
    );
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_FROM), 
        ""
    );
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
        ""
    );
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_GROUP_BY), 
        ""
    );
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_LIMIT), 
        ""
    );
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_HAVING), 
        ""
    );
    zend_update_property_string(
        cspeed_adapter_ce, 
        this, 
        CSPEED_STRL(CSPEED_ADAPTER_ORDER_BY), 
        ""
    );
}/*}}}*/

zend_bool output_sql_errors(zval *pdo_statement, zval *model_object)  /*{{{ Return the SQL running errors*/
{
    zval retval;
    cspeed_pdo_error_info(pdo_statement, &retval);
    zval *sql_state = zend_hash_index_find(
        Z_ARRVAL(retval), 
        0
    );
    zval *sql_code = zend_hash_index_find(
        Z_ARRVAL_P(&retval), 
        1
    );
    zval *sql_info = zend_hash_index_find(
        Z_ARRVAL_P(&retval),
        2
    );
    if (!zend_string_equals(
            Z_STR_P(sql_state), 
            strpprintf(
                0, 
                "00000"
            )
        )
    ) {
        if ( model_object) {
            /* Invokeing object was from Adapter or Cs\mvc\Model */
            /* 1. When setErrorCallback exists call it */
            zval *error_callback = zend_read_property(
                Z_OBJCE_P(model_object), 
                model_object,
                CSPEED_STRL(CSPEED_DB_ERROR_CALLBACK),
                1,
                NULL
            );

            if ( error_callback && !ZVAL_IS_NULL(error_callback) ) {
                zval ret_val;
                zval params[] = {
                    *sql_code,
                    *sql_info
                };
                call_user_function(
                    EG(function_table), 
                    NULL, 
                    error_callback, 
                    &ret_val, 
                    2, 
                    params
                );
                if ( Z_TYPE(ret_val) == IS_FALSE ) {
                    zval_ptr_dtor(&ret_val);
                    cspeed_exit("");
                }
                zval_ptr_dtor(&ret_val);
            } else {
                /* 2. when onErrorCallback method exists in object, call it */
                zval ret_val;
                zval params[] = {
                    *sql_code,
                    *sql_info
                };
                call_method_with_object(
                    model_object, 
                    "onErrorCallback", 
                    2, 
                    params, 
                    &ret_val
                );
                if ( Z_TYPE(ret_val) == IS_FALSE ) {
                    zval_ptr_dtor(&ret_val);
                    cspeed_exit("");
                }
                zval_ptr_dtor(&ret_val);
            }
        }
        cspeed_print_info(
            E_ERROR, 
            "SQL: %d %s", 
            Z_LVAL_P(sql_code), 
            Z_STRVAL_P(sql_info)
        );
    }
    return FALSE;
}/*}}}*/

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_construct, 0, 0, 0)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_select, 0, 0, 1)
    ZEND_ARG_INFO(0, fields)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_from, 0, 0, 1)
    ZEND_ARG_INFO(0, table)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_where, 0, 0, 1)
    ZEND_ARG_INFO(0, where_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_and_where, 0, 0, 1)
    ZEND_ARG_INFO(0, where_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_group_by, 0, 0, 1)
    ZEND_ARG_INFO(0, group_by_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_having, 0, 0, 1)
    ZEND_ARG_INFO(0, having_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_order_by, 0, 0, 1)
    ZEND_ARG_INFO(0, order_by_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_limit, 0, 0, 1)
    ZEND_ARG_INFO(0, limit_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_create_command, 0, 0, 1)
    ZEND_ARG_INFO(0, raw_sql)
    ZEND_ARG_INFO(0, bind_params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_set_error_callback, 0, 0, 1)
    ZEND_ARG_INFO(0, error_callback)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_on_error_callback, 0, 0, 2)
    ZEND_ARG_INFO(0, error_code)
    ZEND_ARG_INFO(0, error_message)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_execute, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_begin, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_rollback, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_commit, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_last_insert_id, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_row_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_find, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_find_all, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_adapter_in_transaction, 0, 0, 0)
ZEND_END_ARG_INFO()

/*}}}*/

CSPEED_METHOD(Adapter, __construct)/*{{{ proto Adapter::__construct(array $options = [])*/
{    
    zval *pdo_options = NULL;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "|a", 
            &pdo_options
        ) == FAILURE
    ){
        return ;
    }
    /* If setting the data from the given options */
    if (pdo_options != NULL) {
        zval *option_value;
        if ( EXPECTED( (option_value = zend_hash_find(
            Z_ARRVAL_P(pdo_options), 
            zend_string_init(
                CSPEED_STRL("dsn"), 
                0
            ))) != NULL )) {
            CSPEED_G(db_master_dsn) = zend_string_copy(Z_STR_P(option_value));
        }
        if ( EXPECTED( (option_value = zend_hash_find(
            Z_ARRVAL_P(pdo_options), 
            zend_string_init(
                CSPEED_STRL("username"), 
                0
            ))) != NULL )) {
            CSPEED_G(db_master_username) = zend_string_copy(Z_STR_P(option_value));
        }
        if ( EXPECTED( (option_value = zend_hash_find(
            Z_ARRVAL_P(pdo_options), 
            zend_string_init(
                CSPEED_STRL("password"), 
                0
            ))) != NULL )) {
            CSPEED_G(db_master_password) = zend_string_copy(Z_STR_P(option_value));
        }
    }

    if (CSPEED_G(db_master_dsn) == NULL) {
        cspeed_print_info(
            E_ERROR, 
            "PDO need a valid dsn."
        );
        RETURN_FALSE
    }
    if (CSPEED_G(db_master_username) == NULL) {
        cspeed_print_info(
            E_ERROR, 
            "PDO need a valid username."
        );
        RETURN_FALSE
    }
    if (CSPEED_G(db_master_password) == NULL) {
        cspeed_print_info(
            E_ERROR, 
            "PDO need a valid password."
        );
        RETURN_FALSE
    }

    zval pdo_object;
    if (pdo_options == NULL) {
        cspeed_pdo_construct(
            &pdo_object, 
            ZSTR_VAL(CSPEED_G(db_master_dsn)), 
            ZSTR_VAL(CSPEED_G(db_master_username)), 
            ZSTR_VAL(CSPEED_G(db_master_password)), 
            NULL
        );
    } else if ( pdo_options && (Z_TYPE_P(pdo_options) == IS_ARRAY) ) 
    {
        zval *option_value;
        if ( EXPECTED( (option_value = zend_hash_find(Z_ARRVAL_P(pdo_options), 
            zend_string_init(CSPEED_STRL("options"), 0))) != NULL )) 
        {
            cspeed_pdo_construct(
                &pdo_object, 
                ZSTR_VAL(CSPEED_G(db_master_dsn)), 
                ZSTR_VAL(CSPEED_G(db_master_username)), 
                ZSTR_VAL(CSPEED_G(db_master_password)), 
                option_value
            );
        } else {
            cspeed_pdo_construct(
                &pdo_object, 
                ZSTR_VAL(CSPEED_G(db_master_dsn)), 
                ZSTR_VAL(CSPEED_G(db_master_username)), 
                ZSTR_VAL(CSPEED_G(db_master_password)), 
                NULL
            );
        }
    }
    /* Store the getting pdo object into the property */
    zend_update_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        &pdo_object
    );
    zend_update_static_property(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_DB_PDO_OBJECT), 
        &pdo_object
    );
    zend_update_static_property(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_DB_THIS_ADAPTER), 
        getThis()
    );
}/*}}}*/

CSPEED_METHOD(Adapter, select)/*{{{ proto Adapter::select($fields)*/
{
    zval *fields;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "z", 
            &fields
        ) == FAILURE
    ) {
        return ;
    }
    if (fields && (Z_TYPE_P(fields) == IS_ARRAY) ) {
        zval result;
        cspeed_build_quote_string(
            fields, 
            &result
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_SELECT), 
            Z_STRVAL(result)
        );
        zval_ptr_dtor(&result);
    } else if (fields && (Z_TYPE_P(fields) == IS_STRING)){
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_SELECT), 
            Z_STRVAL_P(fields)
        );
    } else {
        cspeed_print_info(
            E_ERROR, 
            "Parameter can only be array or string."
        );
    }
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, from)/*{{{ proto Adapter::from($table)*/
{
    zend_string *table;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC,
            "S", 
            &table
        ) == FAILURE
    ) {
        return ;
    }
    if ( !CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(table)) ) {
        cspeed_print_info(
            E_ERROR,
            "%s",
            "The tablename must be a valid string."
        );
    }
    zend_update_property_string(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_ADAPTER_FROM), 
        ZSTR_VAL(table)
    );
    zend_string_release(table);
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, where)/*{{{ proto Adapter::where($where_condition)*/
{
    zval *where;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "z", 
            &where
        ) == FAILURE
    ) {
        return ;
    }
    if (where && (Z_TYPE_P(where) == IS_ARRAY)){
        zval result;
        cspeed_build_equal_string(
            where, 
            " WHERE ", 
            &result
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
            Z_STRVAL(result)
        );
        zval_ptr_dtor(&result);
    } else if (where && (Z_TYPE_P(where) == IS_STRING)){
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
            ZSTR_VAL(
                strpprintf(
                    0, 
                    " WHERE %s", 
                    Z_STRVAL_P(where)
                )
            )
        );
    } else {
        cspeed_print_info(
            E_ERROR, 
            "Parameter can only be array or string."
        );
        RETURN_FALSE
    }
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, andWhere)/*{{{ proto Adapter::andWhere($where_condition)*/
{
    zval *where;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "z", 
            &where
        ) == FAILURE
    ) {
        return ;
    }
    /* Get the where condition */
    zval *previous_where = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
        1, 
        NULL
    );
    if (where && (Z_TYPE_P(where) == IS_ARRAY)){
        zval result;
        cspeed_build_equal_string(
            where, 
            "", 
            &result
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
            ZSTR_VAL(
                strpprintf(0, 
                    "%s AND %s", 
                    Z_STRVAL_P(previous_where), 
                    Z_STRVAL(result)
                )
            )
        );
        zval_ptr_dtor(&result);
    } else if (where && (Z_TYPE_P(where) == IS_STRING)){
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
            ZSTR_VAL(
                strpprintf(0, 
                    "%s AND %s", 
                    Z_STRVAL_P(previous_where), 
                    Z_STRVAL_P(where)
                )
            )
        );
    } else {
        cspeed_print_info(
            E_ERROR, 
            "Parameter can only be array or string."
        );
        RETURN_FALSE
    }
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, groupBy)/*{{{ proto Adapter::groupBy($groupBy)*/
{
    zval *group_by;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "z", 
            &group_by
        ) == FAILURE
    ) {
        return ;
    }
    if (group_by && (Z_TYPE_P(group_by) == IS_ARRAY)){
        zval result;
        cspeed_build_quote_string(
            group_by, 
            &result
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_GROUP_BY), 
            ZSTR_VAL(
                strpprintf(0, 
                    " GROUP BY %s", 
                    Z_STRVAL(result)
                )
            )
        );
        zval_ptr_dtor(&result);
    } else if (group_by && (Z_TYPE_P(group_by) == IS_STRING)){
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_GROUP_BY), 
            ZSTR_VAL(
                strpprintf(0, 
                    " GROUP BY %s", 
                    Z_STRVAL_P(group_by)
                )
            )
        );
    } else {
        cspeed_print_info(
            E_ERROR, 
            "Parameter can only be array or string."
        );
        RETURN_FALSE
    }
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, having)/*{{{ proto Adapter::having($having)*/
{
    zval *having;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "z", 
            &having
        ) == FAILURE
    ) {
        return ;
    }
    if (having && (Z_TYPE_P(having) == IS_ARRAY)){
        zval result;
        cspeed_build_equal_string(
            having, 
            " HAVING ", 
            &result
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_HAVING), 
            Z_STRVAL(result)
        );
        zval_ptr_dtor(&result);
    } else if (having && (Z_TYPE_P(having) == IS_STRING)){
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_HAVING), 
            ZSTR_VAL(
                strpprintf(0, 
                    " HAVING %s", 
                    Z_STRVAL_P(having)
                )
            )
        );
    } else {
        cspeed_print_info(
            E_ERROR, 
            "Parameter can only be array or string."
        );
        RETURN_FALSE
    }
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, orderBy)/*{{{ proto Adapter::orderBy($orderBy)*/
{
    zval *order_by;

    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "z", 
            &order_by
        ) == FAILURE
    ) {
        return ;
    }
    if (order_by && (Z_TYPE_P(order_by) == IS_ARRAY)){
        zval *var_value;
        smart_str order_by_str = {0};
        smart_str_appends(
            &order_by_str, 
            " ORDER BY "
        );
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(order_by), var_value) {
            if (Z_TYPE_P(var_value) == IS_STRING) {
                smart_str_appends(
                    &order_by_str, 
                    Z_STRVAL_P(var_value)
                );
                smart_str_appendc(
                    &order_by_str, 
                    ','
                );
            }
        } ZEND_HASH_FOREACH_END();
        smart_str_0(&order_by_str);
        char *temp_order_by = (char *)malloc(sizeof(char) * (ZSTR_LEN(order_by_str.s)));
        memset(
            temp_order_by, 
            0, 
            ZSTR_LEN(order_by_str.s)
        );
        memcpy(
            temp_order_by, 
            ZSTR_VAL(order_by_str.s),
            ZSTR_LEN(order_by_str.s) - 1
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_ORDER_BY), 
            temp_order_by
        );
        free(temp_order_by);
        smart_str_free(&order_by_str);
    } else if (order_by && (Z_TYPE_P(order_by) == IS_STRING)){
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_ORDER_BY), 
            ZSTR_VAL(
                strpprintf(0, 
                    " ORDER BY %s",
                     Z_STRVAL_P(order_by)
                )
            )
        );
    } else {
        cspeed_print_info(
            E_ERROR, 
            "Parameter can only be array or string."
        );
        RETURN_FALSE
    }
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(Adapter, limit)/*{{{ proto Adapter::limit($limit)*/
{
    zend_string *limit;
    if (zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC, 
            "S", 
            &limit
        ) == FAILURE
    ) {
        return ;
    }
    zend_string *temp_limit_str = strpprintf(
            0, 
            " LIMIT %s", 
            ZSTR_VAL(limit)
        );
    zend_update_property_string(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_ADAPTER_LIMIT), 
        ZSTR_VAL(temp_limit_str)
    );
    RETURN_ZVAL(
        getThis(), 
        1, 
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, createCommand)/*{{{ proto Adapter::createCommand($rawsql)*/
{
    zend_string *raw_sql;
    zval *bind_params = NULL;
    if (
        zend_parse_parameters(
            ZEND_NUM_ARGS() TSRMLS_CC,
            "S|a", 
            &raw_sql, 
            &bind_params
            ) == FAILURE
        ) {
        return ;
    }
    zend_update_property_string(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_ADAPTER_RAW_SQL), 
        ZSTR_VAL(raw_sql)
    );

    if (bind_params) {
        zend_update_property(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_BIND_PARAMS), 
            bind_params
        );
    }
    RETURN_ZVAL(
        getThis(),
        1,
        NULL
    );
}/*}}}*/

CSPEED_METHOD(Adapter, execute)/*{{{ proto Adapter::execute($rawsql)*/
{
    zval ret;
    zval retval;
    zval pdo_statement;

    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    zval *sql_property = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_ADAPTER_RAW_SQL), 
        1, 
        NULL
    );
    zval *param_property = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_ADAPTER_BIND_PARAMS), 
        1, 
        NULL
    );

    cspeed_pdo_prepare(
        pdo_object, 
        Z_STRVAL_P(sql_property), 
        &pdo_statement
    );
    cspeed_pdo_statement_set_fetch_mode(
        &pdo_statement, 
        2, 
        &ret
    );
    cspeed_pdo_statement_execute(
        &pdo_statement, 
        param_property, 
        &retval
    );

    if (!output_sql_errors(&pdo_statement, getThis())){
        zval result;
        cspeed_pdo_statement_fetch_all(
            &pdo_statement, 
            &result
        );
        zend_update_property_string(
            cspeed_adapter_ce, 
            getThis(), 
            CSPEED_STRL(CSPEED_ADAPTER_RAW_SQL), 
            ""
        );
        /*Reset the SQL*/
        RETURN_ZVAL(&result, 1, NULL);
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(Adapter, begin)/*{{{ proto Adapter::begin()*/
{
    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    zval retval;
    cspeed_pdo_begin_transaction(
        pdo_object, 
        &retval
    );
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Adapter, rollback)/*{{{ proto Adapter::rollback()*/
{
    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    zval retval;
    cspeed_pdo_roll_back(
        pdo_object, 
        &retval
    );
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Adapter, commit)/*{{{ proto Adapter::commit()*/
{
    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1,
        NULL
    );
    zval retval;
    cspeed_pdo_commit(
        pdo_object, 
        &retval
    );
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Adapter, lastInsertId)/*{{{ proto Adapter::lastInsertId()*/
{
    zval retval;
    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    cspeed_pdo_last_insert_id(
        pdo_object, 
        NULL, 
        &retval
    );
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Adapter, rowCount)/*{{{ proto Adapter::rowCount()*/
{
    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    zval retval;
    cspeed_pdo_statement_row_count(
        pdo_object, 
        &retval
    );
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Adapter, find)/*{{{ proto Adapter::find()*/
{
    zval ret;
    zval retval;
    zval pdo_statement;

    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );

    cspeed_pdo_prepare(
        pdo_object, 
        get_build_sql(getThis()), 
        &pdo_statement
    );
    cspeed_pdo_statement_set_fetch_mode(
        &pdo_statement, 
        2, 
        &ret
    );
    cspeed_pdo_statement_execute(
        &pdo_statement, 
        NULL, 
        &retval
    );

    if ( !output_sql_errors(&pdo_statement, getThis()) ){
        zval result;
        cspeed_pdo_statement_fetch(
            &pdo_statement, 
            &result
        );
        reset_adapter_sql(getThis()); /*Reset the SQL*/
        RETURN_ZVAL(&result, 1, NULL);
    }
    RETURN_NULL()
}/*}}}*/

CSPEED_METHOD(Adapter, findAll)/*{{{ proto Adapter::findAll()*/
{
    zval ret;
    zval retval;
    zval pdo_statement;

    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    cspeed_pdo_prepare(
        pdo_object, 
        get_build_sql(getThis()), 
        &pdo_statement
    );
    cspeed_pdo_statement_set_fetch_mode(
        &pdo_statement, 
        2, 
        &ret
    );
    cspeed_pdo_statement_execute(
        &pdo_statement, 
        NULL, 
        &retval
    );

    if ( !output_sql_errors(&pdo_statement, getThis()) ){
        zval result;
        cspeed_pdo_statement_fetch_all(
            &pdo_statement, 
            &result
        );
        reset_adapter_sql(getThis()); /*Reset the SQL*/
        RETURN_ZVAL(&result, 1, NULL);
    }
    RETURN_NULL()
}/*}}}*/

CSPEED_METHOD(Adapter, isInTransaction) /*{{{ proto Adapter::isInTransaction*/
{
    zval retval;

    zval *pdo_object = zend_read_property(
        cspeed_adapter_ce, 
        getThis(), 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        1, 
        NULL
    );
    cspeed_pdo_in_transaction(
        pdo_object,
        &retval
    );
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

/*{{{ proto Adapter::setErrorCallback($closure)*/
CSPEED_METHOD(Adapter, setErrorCallback)
{
    zval *error_callback;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &error_callback) == FAILURE) {
        return ;
    }

    if ( Z_TYPE_P(error_callback) != IS_OBJECT ){
        cspeed_print_info(
            E_ERROR,
            "%s",
            "Parameter must be a valid callback object."
        );
    }
    zend_string *error_handler_name = NULL;
    if ( !zend_is_callable(error_callback, 0, &error_handler_name) ) {
        cspeed_print_info(
            E_ERROR, 
            "Parameter must be callable."
        );
    }

    /* Store the data into property */
    zend_update_property(
        cspeed_adapter_ce, 
        getThis(),
        CSPEED_STRL(CSPEED_DB_ERROR_CALLBACK), 
        error_callback
    );
}/*}}}*/

/*{{{ proto Adapter::onErrorCallback($errorCode, $errorMessage)*/
CSPEED_METHOD(Adapter, onErrorCallback)
{

}/*}}}*/

static const zend_function_entry cspeed_adapter_functions[] = { /*{{{*/
    CSPEED_ME(Adapter, __construct, arginfo_cspeed_adapter_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, select, arginfo_cspeed_adapter_select, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, from, arginfo_cspeed_adapter_from, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, where, arginfo_cspeed_adapter_where, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, andWhere, arginfo_cspeed_adapter_and_where, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, groupBy, arginfo_cspeed_adapter_group_by, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, having, arginfo_cspeed_adapter_having, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, orderBy, arginfo_cspeed_adapter_order_by, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, limit, arginfo_cspeed_adapter_limit, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, createCommand, arginfo_cspeed_adapter_create_command, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, setErrorCallback, arginfo_cspeed_adapter_set_error_callback, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, onErrorCallback, arginfo_cspeed_adapter_on_error_callback, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, execute, arginfo_cspeed_adapter_execute, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, begin, arginfo_cspeed_adapter_begin, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, rollback, arginfo_cspeed_adapter_rollback, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, commit, arginfo_cspeed_adapter_commit, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, lastInsertId, arginfo_cspeed_adapter_last_insert_id, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, rowCount, arginfo_cspeed_adapter_row_count, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, find, arginfo_cspeed_adapter_find, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, findAll, arginfo_cspeed_adapter_find_all, ZEND_ACC_PUBLIC)
    CSPEED_ME(Adapter, isInTransaction, arginfo_cspeed_adapter_in_transaction, ZEND_ACC_PUBLIC)
    PHP_FE_END
};/*}}}*/

CSPEED_INIT(adapter)/*{{{*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(
        ce, 
        "Cs\\db\\pdo", 
        "Adapter", 
        cspeed_adapter_functions
    )
    cspeed_adapter_ce = zend_register_internal_class(&ce);
    /* Implements from the ModelInterface */
    zend_class_implements(cspeed_adapter_ce, 1, cspeed_model_interface_ce);
    /* All propertyies */
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_SELECT), 
        "*", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_FROM), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_WHERE), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_GROUP_BY), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_HAVING), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_ORDER_BY), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_LIMIT), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_string(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_RAW_SQL), 
        "", 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_null(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_ADAPTER_BIND_PARAMS), 
        ZEND_ACC_PROTECTED
    );
    zend_declare_property_null(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_DB_PDO_OBJECT), 
        ZEND_ACC_STATIC
    );
    zend_declare_property_null(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_DB_THIS_ADAPTER), 
        ZEND_ACC_STATIC
    );
    zend_declare_property_null(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_DB_THIS_PDO), 
        ZEND_ACC_PUBLIC
    );
    zend_declare_property_null(
        cspeed_adapter_ce, 
        CSPEED_STRL(CSPEED_DB_ERROR_CALLBACK), 
        ZEND_ACC_PROTECTED
    );
}/*}}}*/



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

