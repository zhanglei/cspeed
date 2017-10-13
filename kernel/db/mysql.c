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

#include "kernel/db/pdo.h"              /* Use the PDO wrapper functions */
#include "kernel/db/mysql.h"
#include "kernel/db/model_interface.h"

#include "zend_smart_str.h"             /* Use the smart_str */

#include <ctype.h>


char *get_build_sql(zval *this) /*{{{ Return the builded SQL*/
{
    zval *select = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_SELECT), 1, NULL);
    zval *from = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_FROM), 1, NULL);
    zval *where = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_WHERE), 1, NULL);
    zval *group_by = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_GROUP_BY), 1, NULL);
    zval *limit = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_LIMIT), 1, NULL);
    zval *having = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_HAVING), 1, NULL);
    zval *order_by = zend_read_property(cspeed_mysql_ce, this, CSPEED_STRL(CSPEED_MYSQL_ORDER_BY), 1, NULL);

    return ZSTR_VAL(strpprintf(0, " SELECT %s FROM %s%s%s%s%s%s", Z_STRVAL_P(select), Z_STRVAL_P(from), Z_STRVAL_P(where), Z_STRVAL_P(group_by), Z_STRVAL_P(having), Z_STRVAL_P(order_by), Z_STRVAL_P(limit) ));
}/*}}}*/

/*{{{*/
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_construct, 0, 0, 1)
    ZEND_ARG_INFO(0, options)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_select, 0, 0, 1)
    ZEND_ARG_INFO(0, fields)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_from, 0, 0, 1)
    ZEND_ARG_INFO(0, table)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_where, 0, 0, 1)
    ZEND_ARG_INFO(0, where_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_group_by, 0, 0, 1)
    ZEND_ARG_INFO(0, group_by_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_having, 0, 0, 1)
    ZEND_ARG_INFO(0, having_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_order_by, 0, 0, 1)
    ZEND_ARG_INFO(0, order_by_condition)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_limit, 0, 0, 1)
    ZEND_ARG_INFO(0, limit_condition)
    ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_query, 0, 0, 1)
    ZEND_ARG_INFO(0, raw_sql)
    ZEND_ARG_INFO(0, bind_params)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_execute, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_begin, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_rollback, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_commit, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_last_insert_id, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_row_count, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_find, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_mysql_find_all, 0, 0, 0)
ZEND_END_ARG_INFO()

/*}}}*/

CSPEED_METHOD(MySql, __construct)/*{{{ proto MySql::__construct(array $options = [])*/
{
    zval *options;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &options) == FAILURE) {
        return ;
    }
    zval *dsn = zend_hash_find(Z_ARRVAL_P(options), zend_string_init(CSPEED_STRL("dsn"), 0));
    if (dsn == NULL) {
        php_error_docref(NULL, E_ERROR, "Parameter need a dsn key.");
        RETURN_FALSE
    }
    zval *username = zend_hash_find(Z_ARRVAL_P(options), zend_string_init(CSPEED_STRL("username"), 0));
    if (username == NULL) {
        php_error_docref(NULL, E_ERROR, "Parameter need a username key.");
        RETURN_FALSE
    }
    zval *password = zend_hash_find(Z_ARRVAL_P(options), zend_string_init(CSPEED_STRL("password"), 0));
    if (password == NULL) {
        php_error_docref(NULL, E_ERROR, "Parameter need a password key.");
        RETURN_FALSE
    }
    zval *pdo_options = zend_hash_find(Z_ARRVAL_P(options), zend_string_init(CSPEED_STRL("options"), 0));
    zval pdo_object;
    if (pdo_options == NULL) {
        cspeed_pdo_construct(&pdo_object, Z_STRVAL_P(dsn), Z_STRVAL_P(username), Z_STRVAL_P(password), NULL);
    } else {
        cspeed_pdo_construct(&pdo_object, Z_STRVAL_P(dsn), Z_STRVAL_P(username), Z_STRVAL_P(password), pdo_options);
    }
    /* Store the getting pdo object into the property */
    zend_update_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), &pdo_object);
    zval_ptr_dtor(options);
    zval_ptr_dtor(&pdo_object);
}/*}}}*/

CSPEED_METHOD(MySql, select)/*{{{ proto MySql::select($fields)*/
{
    zval *fields;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &fields) == FAILURE) {
        return ;
    }
    smart_str field_str = {0};
    zval *value;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(fields), value) {
        if ( (Z_TYPE_P(value) == IS_STRING) && isalpha(*(Z_STRVAL_P(value))) ){
            smart_str_appendc(&field_str, '`');
            smart_str_appends(&field_str, Z_STRVAL_P(value));
            smart_str_appends(&field_str, "`,");
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&field_str);
    char *temp_select_str = (char *)malloc(sizeof(char) * ZSTR_LEN(field_str.s));
    memset(temp_select_str, 0, ZSTR_LEN(field_str.s));
    memcpy(temp_select_str, ZSTR_VAL(field_str.s), ZSTR_LEN(field_str.s) - 1);
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_SELECT), temp_select_str);
    free(temp_select_str);
    zval_ptr_dtor(fields);
    smart_str_free(&field_str);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, from)/*{{{ proto MySql::from($table)*/
{
    zend_string *table;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &table) == FAILURE) {
        return ;
    }
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_FROM), ZSTR_VAL(table));
    zend_string_release(table);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, where)/*{{{ proto MySql::where($where_condition)*/
{
    zval *where;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &where) == FAILURE) {
        return ;
    }
    /*zend_string *val_key;*/
    zend_string *val_key;
    zval *var_value;
    smart_str where_str = {0};
    smart_str_appends(&where_str, " WHERE ");
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(where), val_key, var_value) {
        if (isalpha(*(ZSTR_VAL(val_key)))) {
            smart_str_appendc(&where_str, '`');
            smart_str_appends(&where_str, ZSTR_VAL(val_key));
            smart_str_appends(&where_str, "`='");
            if (Z_TYPE_P(var_value) == IS_LONG){
                smart_str_appends(&where_str, ZSTR_VAL(strpprintf(0, "%d", Z_LVAL_P(var_value))));
            } else if (Z_TYPE_P(var_value) == IS_STRING) {
                smart_str_appends(&where_str, Z_STRVAL_P(var_value));
            }
            smart_str_appends(&where_str, "' AND ");
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&where_str);
    char *temp_where_str = (char *)malloc(sizeof(char) * ZSTR_LEN(where_str.s) - 4);
    memset(temp_where_str, 0, ZSTR_LEN(where_str.s) - 4);
    memcpy(temp_where_str, ZSTR_VAL(where_str.s), ZSTR_LEN(where_str.s) - 5);
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_WHERE), temp_where_str);
    smart_str_free(&where_str);
    zval_ptr_dtor(where);
    free(temp_where_str);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, groupBy)/*{{{ proto MySql::groupBy($groupBy)*/
{
    zval *group_by;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &group_by) == FAILURE) {
        return ;
    }
    zval *var_value;
    smart_str group_by_str = {0};
    smart_str_appends(&group_by_str, " GROUP BY ");
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(group_by), var_value) {
        if (Z_TYPE_P(var_value) == IS_STRING) {
            smart_str_appendc(&group_by_str, '`');
            smart_str_appends(&group_by_str, Z_STRVAL_P(var_value));
            smart_str_appends(&group_by_str, "`,");
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&group_by_str);
    char *temp_group_by = (char *)malloc(sizeof(char) * (ZSTR_LEN(group_by_str.s)));
    memset(temp_group_by, 0, ZSTR_LEN(group_by_str.s));
    memcpy(temp_group_by, ZSTR_VAL(group_by_str.s), ZSTR_LEN(group_by_str.s) - 1);
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_GROUP_BY), temp_group_by);
    free(temp_group_by);
    smart_str_free(&group_by_str);
    zval_ptr_dtor(group_by);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, having)/*{{{ proto MySql::having($having)*/
{
    zval *having;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &having) == FAILURE) {
        return ;
    }
    zend_string *var_key;
    zval *var_value;
    smart_str have_str = {0};
    smart_str_appends(&have_str, " HAVING ");
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(having), var_key, var_value) {
        if (isalpha(*(ZSTR_VAL(var_key)))) {
            smart_str_appendc(&have_str, '`');
            smart_str_appends(&have_str, ZSTR_VAL(var_key));
            smart_str_appends(&have_str, "`='");
            if (Z_TYPE_P(var_value) == IS_LONG){
                smart_str_appends(&have_str, ZSTR_VAL(strpprintf(0, "%d", Z_LVAL_P(var_value))));
            } else if (Z_TYPE_P(var_value) == IS_STRING) {
                smart_str_appends(&have_str, Z_STRVAL_P(var_value));
            }
            smart_str_appends(&have_str, "' AND ");
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&have_str);
    char *temp_having_str = (char *)malloc(sizeof(char) * (ZSTR_LEN(have_str.s) - 4 ));
    memset(temp_having_str, 0, ZSTR_LEN(have_str.s) - 4);
    memcpy(temp_having_str, ZSTR_VAL(have_str.s), ZSTR_LEN(have_str.s) - 5);
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_HAVING), temp_having_str);
    free(temp_having_str);
    smart_str_free(&have_str);
    zval_ptr_dtor(having);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, orderBy)/*{{{ proto MySql::orderBy($orderBy)*/
{
    zval *order_by;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &order_by) == FAILURE) {
        return ;
    }
    zval *var_value;
    smart_str order_by_str = {0};
    smart_str_appends(&order_by_str, " ORDER BY ");
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(order_by), var_value) {
        if (Z_TYPE_P(var_value) == IS_STRING) {
            /*smart_str_appendc(&order_by_str, '`');*/
            smart_str_appends(&order_by_str, Z_STRVAL_P(var_value));
            /*smart_str_appends(&order_by_str, "`,");*/
            smart_str_appendc(&order_by_str, ',');
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&order_by_str);
    char *temp_order_by = (char *)malloc(sizeof(char) * (ZSTR_LEN(order_by_str.s)));
    memset(temp_order_by, 0, ZSTR_LEN(order_by_str.s));
    memcpy(temp_order_by, ZSTR_VAL(order_by_str.s), ZSTR_LEN(order_by_str.s) - 1);
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_ORDER_BY), temp_order_by);
    free(temp_order_by);
    smart_str_free(&order_by_str);
    zval_ptr_dtor(order_by);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, limit)/*{{{ proto MySql::limit($limit)*/
{
    zend_long *num, *offset = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|l", &num, &offset) == FAILURE) {
        return ;
    }
    zend_string *temp_limit_str;
    if (offset) {
        temp_limit_str = strpprintf(0, " LIMIT %d,%d", num, offset);
    } else {
        temp_limit_str = strpprintf(0, " LIMIT %d", num);
    }
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_LIMIT), ZSTR_VAL(temp_limit_str));
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, query)/*{{{ proto MySql::query($rawsql)*/
{
    zend_string *raw_sql;
    zval *bind_params = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S|a", &raw_sql, &bind_params) == FAILURE) {
        return ;
    }
    zend_update_property_string(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_RAW_SQL), ZSTR_VAL(raw_sql));

    if (bind_params) {
        zend_update_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_BIND_PARAMS), bind_params);
    }
}/*}}}*/

CSPEED_METHOD(MySql, execute)/*{{{ proto MySql::execute($rawsql)*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);
    zval *sql_property = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_RAW_SQL), 1, NULL);
    zval *param_property = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_BIND_PARAMS), 1, NULL);

    zval pdo_statement;
    cspeed_pdo_prepare(pdo_object, Z_STRVAL_P(sql_property), &pdo_statement);
    
    zval ret;
    cspeed_pdo_statement_set_fetch_mode(&pdo_statement, 2, &ret);

    zval retval;
    cspeed_pdo_statement_execute(&pdo_statement, param_property, &retval);

    zval result;
    cspeed_pdo_statement_fetch_all(&pdo_statement, &result);

    RETURN_ZVAL(&result, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, begin)/*{{{ proto MySql::begin()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval retval;
    cspeed_pdo_begin_transaction(pdo_object, &retval);
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, rollback)/*{{{ proto MySql::rollback()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval retval;
    cspeed_pdo_roll_back(pdo_object, &retval);
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, commit)/*{{{ proto MySql::commit()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval retval;
    cspeed_pdo_commit(pdo_object, &retval);
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, lastInsertId)/*{{{ proto MySql::lastInsertId()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval retval;
    cspeed_pdo_last_insert_id(pdo_object, NULL, &retval);
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, rowCount)/*{{{ proto MySql::rowCount()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval retval;
    cspeed_pdo_statement_row_count(pdo_object, &retval);
    RETURN_ZVAL(&retval, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, find)/*{{{ proto MySql::find()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval pdo_statement;
    cspeed_pdo_prepare(pdo_object, get_build_sql(getThis()), &pdo_statement);
    
    zval ret;
    cspeed_pdo_statement_set_fetch_mode(&pdo_statement, 2, &ret);

    zval retval;
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval);

    zval result;
    cspeed_pdo_statement_fetch(&pdo_statement, &result);

    RETURN_ZVAL(&result, 1, NULL);
}/*}}}*/

CSPEED_METHOD(MySql, findAll)/*{{{ proto MySql::findAll()*/
{
    zval *pdo_object = zend_read_property(cspeed_mysql_ce, getThis(), CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), 1, NULL);

    zval pdo_statement;
    cspeed_pdo_prepare(pdo_object, get_build_sql(getThis()), &pdo_statement);
    
    zval ret;
    cspeed_pdo_statement_set_fetch_mode(&pdo_statement, 2, &ret);

    zval retval;
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval);

    zval result;
    cspeed_pdo_statement_fetch_all(&pdo_statement, &result);

    RETURN_ZVAL(&result, 1, NULL);
}/*}}}*/

static const zend_function_entry cspeed_mysql_functions[] = { /*{{{*/
    CSPEED_ME(MySql, __construct, arginfo_cspeed_mysql_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, select, arginfo_cspeed_mysql_select, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, from, arginfo_cspeed_mysql_from, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, where, arginfo_cspeed_mysql_where, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, groupBy, arginfo_cspeed_mysql_group_by, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, having, arginfo_cspeed_mysql_having, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, orderBy, arginfo_cspeed_mysql_order_by, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, limit, arginfo_cspeed_mysql_limit, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, query, arginfo_cspeed_mysql_query, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, execute, arginfo_cspeed_mysql_execute, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, begin, arginfo_cspeed_mysql_begin, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, rollback, arginfo_cspeed_mysql_rollback, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, commit, arginfo_cspeed_mysql_commit, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, lastInsertId, arginfo_cspeed_mysql_last_insert_id, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, rowCount, arginfo_cspeed_mysql_row_count, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, find, arginfo_cspeed_mysql_find, ZEND_ACC_PUBLIC)
    CSPEED_ME(MySql, findAll, arginfo_cspeed_mysql_find_all, ZEND_ACC_PUBLIC)
    PHP_FE_END
};/*}}}*/

CSPEED_INIT(mysql)/*{{{*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\db\\pdo", "MySql", cspeed_mysql_functions)
    cspeed_mysql_ce = zend_register_internal_class(&ce);

    /* Implements from the ModelInterface */
    zend_class_implements(cspeed_mysql_ce, 1, cspeed_model_interface_ce);

    zend_declare_property_null(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_PDO_OBJECT), ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_SELECT), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_FROM), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_WHERE), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_GROUP_BY), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_HAVING), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_ORDER_BY), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_LIMIT), "", ZEND_ACC_PROTECTED);
    zend_declare_property_string(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_RAW_SQL), "", ZEND_ACC_PROTECTED);
    zend_declare_property_null(cspeed_mysql_ce, CSPEED_STRL(CSPEED_MYSQL_BIND_PARAMS), ZEND_ACC_PROTECTED);
}/*}}}*/