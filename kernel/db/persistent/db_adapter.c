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

#include "kernel/di/di.h"
#include "kernel/db/pdo.h"
#include "Zend/zend_hash.h"
#include "kernel/db/adapter.h"
#include "kernel/tool/helper.h"
#include "Zend/zend_smart_str.h"
#include "ext/standard/php_string.h" 
#include "kernel/db/persistent/yacc.h"
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

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_db_adapter_load_config, 0, 0, 1)
    ZEND_ARG_INFO(0, sys_config)
ZEND_END_ARG_INFO()

/*{{{ proto DbAdapter::__construct(array options = [])
 * Construct the DbAdapter with the given parameters. */
CSPEED_METHOD(DbAdapter, __construct)
{
}/*}}}*/

/*{{{ proto DbAdapter::createCommand($sql)
 * To prepare the SQL to the DbAdapter engine.
 * check the SQL grammer and so on... */
CSPEED_METHOD(DbAdapter, createCommand)
{
    zend_string *sql_command;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &sql_command) == FAILURE ) {
        return ;
    }
    /** Read the DbAdapter configs */
    zval *db_configs = zend_read_property(
        Z_OBJCE_P(getThis()), 
        getThis(), 
        CSPEED_STRL(DBADAPTER_CONFIGS), 
        1, 
        NULL
    );
    /* Find the tables configs */
    zval *tables = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("tables"));

    /*{{{*/
    zval table_replace;
    array_init(&table_replace);

    zval *hSize = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("hSize"));

    /*}}}*/
    SQL_PARSER_RESULT *sql_result = parse_sql(ZSTR_VAL(sql_command));
    if (sql_result->sql_type == 1)
    {
        /**
         *  SELECT query contains the following elements
         *  :
         *      SELECT fields FROM table_name where_opts groupby_opts orderby_opts limit_opts
         */
        size_t sql_length = strlen(sql_result->select_statement.select)
                          + strlen(sql_result->select_statement.fields)
                          + strlen(sql_result->select_statement.from)
                          + strlen(sql_result->select_statement.table_name)
                          + strlen(sql_result->select_statement.where_opts)
                          + strlen(sql_result->select_statement.groupby_opts)
                          + strlen(sql_result->select_statement.orderby_opts)
                          + strlen(sql_result->select_statement.limit_opts);
        char *select_result = (char *)malloc(sizeof(char) * sql_length);
        memset(select_result, 0, sql_length);

        /* The folowing job is to strncat the SQL */
        strncat(select_result, sql_result->select_statement.select, strlen(sql_result->select_statement.select));
        strncat(select_result, " ", strlen(" "));
        /**
         * If tabbles was not empty. do the replace job.
         */
        if (zend_hash_num_elements(Z_ARRVAL_P(tables))){
            /* To filter all tables */
            zval *table_name; zend_long index;
            int length = 0;

            /**{{{ fields*/
            char fields[256] = {0};
            char temp_fields[256] = {0};
            strcpy(fields, sql_result->select_statement.fields);
            /** }}}*/

            /**{{{ tableName*/
            char tableName[256] = {0};
            char temp_tableName[256] = {0};
            strcpy(tableName, sql_result->select_statement.table_name);
            /*}}}*/ 

            /*{{{ where_opts*/
            char where_opts[256] = {0};
            char temp_where_opts[256] = {0};
            strcpy(where_opts, sql_result->select_statement.where_opts);
            /**}}}*/

            /**{{{ groupby_opts */
            char groupby_opts[256] = {0};
            char temp_groupby_opts[256] = {0};
            strcpy(groupby_opts, sql_result->select_statement.groupby_opts);
            /*}}}*/

            /*{{{ orderby_opts*/
            char orderby_opts[256] = {0};
            char temp_orderby_opts[256] = {0};
            strcpy(orderby_opts, sql_result->select_statement.orderby_opts);
            /*}}}*/

            /**{{{ limit_opts*/
            char limit_opts[256] = {0};
            char temp_limit_opts[256] = {0};
            strcpy(limit_opts, sql_result->select_statement.limit_opts);
            /**}}}*/

            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(tables), index, table_name)
            {
                if ( (Z_TYPE_P(table_name) == IS_STRING) 
                    && (CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(tables))) )
                {
                    
                    add_next_index_string(
                        &table_replace,
                        ZSTR_VAL(
                            strpprintf(
                                0,
                                "?%d",
                                index
                            )
                        )
                    );

                    /**
                     * If fields not empty. replace the table name to `?x`
                     */
                    if ( CSPEED_STRING_NOT_EMPTY(fields) )
                    {
                        bzero(temp_fields, strlen(temp_fields));
                        replace_all_name(
                            fields, 
                            Z_STRVAL_P(table_name),
                            ZSTR_VAL(
                                strpprintf(
                                    0,
                                    "?%d",
                                    index
                                )
                            ), 
                            temp_fields,
                            &length,
                            0
                        );
                        strcpy(fields, temp_fields);
                    }
                    /**
                     * If tableName was not empty.
                     */
                    if (CSPEED_STRING_NOT_EMPTY(tableName)) {
                        bzero(temp_tableName, strlen(temp_tableName));
                        replace_all_name(
                            tableName, 
                            Z_STRVAL_P(table_name),
                            ZSTR_VAL(
                                strpprintf(
                                    0,
                                    "?%d",
                                    index
                                )
                            ), 
                            temp_tableName,
                            &length,
                            1
                        );
                        strcpy(tableName, temp_tableName);
                    }

                    /**
                     * where_opts not empty.
                     */
                    if (CSPEED_STRING_NOT_EMPTY(where_opts)) {
                        bzero(temp_where_opts, strlen(temp_where_opts));
                        replace_all_name(
                            where_opts, 
                            Z_STRVAL_P(table_name),
                            ZSTR_VAL(
                                strpprintf(
                                    0,
                                    "?%d",
                                    index
                                )
                            ), 
                            temp_where_opts,
                            &length,
                            1
                        );
                        strcpy(where_opts, temp_where_opts);
                    }

                    /**
                     * groupby_opts not empty
                     */
                    if (CSPEED_STRING_NOT_EMPTY(groupby_opts))
                    {
                        bzero(temp_groupby_opts, strlen(temp_groupby_opts));
                        replace_all_name(
                            groupby_opts, 
                            Z_STRVAL_P(table_name),
                            ZSTR_VAL(
                                strpprintf(
                                    0,
                                    "?%d",
                                    index
                                )
                            ), 
                            temp_groupby_opts,
                            &length,
                            1
                        );
                        strcpy(groupby_opts, temp_groupby_opts);
                    }

                    /**
                     * orderby_opts not empty
                     */
                    if (CSPEED_STRING_NOT_EMPTY(orderby_opts))
                    {
                        bzero(temp_orderby_opts, strlen(temp_orderby_opts));
                        replace_all_name(
                            orderby_opts, 
                            Z_STRVAL_P(table_name),
                            ZSTR_VAL(
                                strpprintf(
                                    0,
                                    "?%d",
                                    index
                                )
                            ), 
                            temp_orderby_opts,
                            &length,
                            1
                        );
                        strcpy(orderby_opts, temp_orderby_opts);
                    }

                    /**
                     * limit_opts not empty
                     */
                    if (CSPEED_STRING_NOT_EMPTY(limit_opts))
                    {
                        bzero(temp_limit_opts, strlen(temp_limit_opts));
                        replace_all_name(
                            limit_opts, 
                            Z_STRVAL_P(table_name),
                            ZSTR_VAL(
                                strpprintf(
                                    0,
                                    "?%d",
                                    index
                                )
                            ), 
                            temp_limit_opts,
                            &length,
                            1
                        );
                        strcpy(limit_opts, temp_limit_opts);
                    }

                } else {
                    cspeed_print_info(
                        E_ERROR,
                        "Config `tables` must be string value. "
                    );
                }
            } ZEND_HASH_FOREACH_END();

            /* strncat the fields to the select_result */
            strncat(select_result, fields, strlen(fields));
            strncat(select_result, " ", strlen(" "));
            strncat(select_result, sql_result->select_statement.from, strlen(sql_result->select_statement.from));
            strncat(select_result, " ", strlen(" "));
            strncat(select_result, tableName, strlen(tableName));
            strncat(select_result, " ", strlen(" "));
            strncat(select_result, where_opts, strlen(where_opts));
            strncat(select_result, " ", strlen(" "));
            strncat(select_result, groupby_opts, strlen(groupby_opts));
            strncat(select_result, " ", strlen(" "));
            strncat(select_result, orderby_opts, strlen(orderby_opts));
            strncat(select_result, " ", strlen(" "));
            strncat(select_result, limit_opts, strlen(limit_opts));

            /* SQL result strncat ending. */
        }

        /* To concat the SQL with the UNION SQL keyword */
        smart_str sql = {0};
        size_t index = 0;
        zend_long index_t;
        zval *table_name;
        for (;index < Z_LVAL_P(hSize); index++) {

            zval table_replace_subject;
            array_init(&table_replace_subject);

            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(tables), index_t, table_name)
            {
                add_next_index_str(
                    &table_replace_subject,
                    strpprintf(
                        0,
                        "%s_%d",
                        Z_STRVAL_P(table_name),
                        index
                    )
                );
            } ZEND_HASH_FOREACH_END();

            /* to replace */
            zval function_name, ret_val, subject;
            ZVAL_STRING(&subject, select_result);
            zval params[] = { table_replace, table_replace_subject, subject };
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
            smart_str_appends(&sql, "(");
            smart_str_appends(&sql, Z_STRVAL(ret_val));
            smart_str_appends(&sql, ")");
            zval_ptr_dtor(&subject);
            zval_ptr_dtor(&ret_val);

            /* Append the UNION */
            if (index < (Z_LVAL_P(hSize) - 1)) {
                smart_str_appends(&sql, " UNION ");
            }
        }
        /* smart_str end */
        smart_str_0(&sql);
        zend_update_property_str(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), sql.s);
        smart_str_free(&sql);
        free(select_result);
    }

    if (sql_result->sql_type >= 1064) {
        free(sql_result->sql_result);
        cspeed_print_info(
            E_ERROR,
            "SQL: 1064 %s.",
            sql_result->sql_result
        );
        RETURN_ZVAL(getThis(), 1, NULL);
    } else {
        zend_update_property_long(
            Z_OBJCE_P(getThis()), 
            getThis(), 
            CSPEED_STRL(RAW_SQL_TYPE), 
            sql_result->sql_type
        );
        free(sql_result->sql_result);
        RETURN_ZVAL(getThis(), 1, NULL);
    }
}/*}}}*/

/*{{{ proto DbAdapter::loadConfig($configs = [])
 * Load the configs to the DbAdapter to choose the right db to
 * do the job.*/
CSPEED_METHOD(DbAdapter, loadConfig)
{
    zval *db_configs;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &db_configs) == FAILURE ) {
        return ;
    }

    if (Z_TYPE_P(db_configs) == IS_ARRAY) {
        zend_update_property(
            Z_OBJCE_P(getThis()),
            getThis(),
            CSPEED_STRL(DBADAPTER_CONFIGS),
            db_configs
        );
    } else {
        cspeed_print_info(E_ERROR, "%s", "Parameters must be array.");
    }
}/*}}}*/

/*{{{ proto DbAdapter::execute()
 * To run the SQL and return the results to the invoker.*/
CSPEED_METHOD(DbAdapter, execute)
{
    /**
     * Check the Db running mode.
     *   Balance setting. 
     * 
     * db was an array, so it must be ZEND_HASH_FOREACH_NUM_KEY_VAL()
     */
    zval *db;
    zval *value;
    zend_long index;
    zval *db_configs    = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(DBADAPTER_CONFIGS), 1, NULL );

    zval *balance_value = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("balance"));
    zval *sql_statement = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), 1, NULL);
    zval *sql_type      = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL_TYPE), 1, NULL);
    if (Z_LVAL_P(balance_value) == 1)
    {
        /* RW apart */
        if (Z_LVAL_P(sql_type)== 1)
        {
            db = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("readDb"));
            if (ZVAL_IS_NULL(db) || !db)
            {
                cspeed_print_info(
                    E_ERROR,
                    "readDb was empty."
                );
            }
        }
        else
        {
            /* Using writeDb */
            db = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("writeDb"));
            if (ZVAL_IS_NULL(db) || !db)
            {
                cspeed_print_info(
                    E_ERROR,
                    "writeDb was empty."
                );
            } 
        }
    }
    else if (Z_LVAL_P(balance_value) == 2)
    {
        /* Using readDb */
        db = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("readDb"));
        if (ZVAL_IS_NULL(db) || !db)
        {
            cspeed_print_info(
                E_ERROR,
                "readDb was empty."
            );
        }

    }
    else if (Z_LVAL_P(balance_value) == 3)
    {
        /* Using writeDb */
        db = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("writeDb"));
        if (ZVAL_IS_NULL(db) || !db)
        {
            cspeed_print_info(
                E_ERROR,
                "writeDb was empty."
            );
        } 
    }

    /* Check the type of db was right or not. */
    if (Z_TYPE_P(db) != IS_ARRAY)
    {
        cspeed_print_info(
            E_ERROR,
            "Balance db setting must be array."
        );
    }

    /**
     * To get the PDO object to run the SQL
     */
    if (!CSPEED_G(di_object))
    {
        cspeed_print_info(
            E_ERROR,
            "The Di container not contain db settings. "
        );
    }

    zval di_object, *adapter_db;
    ZVAL_OBJ( &di_object, CSPEED_G(di_object) );
    zval *objects = zend_read_property(
        cspeed_di_ce, 
        &di_object, 
        CSPEED_STRL(CSPEED_DI_OBJECT), 
        1, 
        NULL
    );

    zval ultimate_result;
    array_init(&ultimate_result);

    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(db), index, value)
    {
        if ( !CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(value)) )
        {
            continue;
        }
        if (EXPECTED( (adapter_db = zend_hash_find(Z_ARRVAL_P(objects), Z_STR_P(value)) ) != NULL ))
        {
            /* The db adapter exsits. set the db */
            zval *adapter_pdo_object = zend_read_property(
                Z_OBJCE_P(getThis()), 
                adapter_db, 
                CSPEED_STRL(CSPEED_DB_THIS_PDO), 
                1, 
                NULL
            );
            zval temp_result;
            // cspeed_exit(Z_STRVAL_P(sql_statement));
            cspeed_pdo_execute_sql(adapter_pdo_object, getThis(), Z_STRVAL_P(sql_statement), NULL, &temp_result);
            zend_hash_merge(Z_ARRVAL(ultimate_result), Z_ARRVAL(temp_result), zval_add_ref, 0);
            zval_ptr_dtor(&temp_result);
        }
        else
        {
            //ZVAL_NULL(&ultimate_result);
        }
    } ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(&ultimate_result, 1, NULL);
}/*}}}*/

/*{{{ ALL functions the DbAdapter class contains. */
static const zend_function_entry db_adapter_functions[] = {
    CSPEED_ME(DbAdapter, __construct, arginfo_cspeed_db_adapter_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, createCommand, arginfo_cspeed_db_adapter_create_command, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, execute, arginfo_cspeed_db_adapter_execute, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, loadConfig, arginfo_cspeed_db_adapter_load_config, ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/

/*{{{ proto db_adapter_init().
 * To initialise the DbAdapter class in the PHP-Extension. */
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

    /*Declare the properties of the DbAdapter class or object. */
    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(SHARED_KEY), "", ZEND_ACC_PRIVATE);
    zend_declare_property_long(cspeed_db_adapter_ce, CSPEED_STRL(HORIZONTAL_SIZE), 1, ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(TABLE_NAME), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(RAW_SQL), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_db_adapter_ce, CSPEED_STRL(RAW_SQL_TYPE), "", ZEND_ACC_PRIVATE);

    /* Some zval type properties below shows: */
    zend_declare_property_null(cspeed_db_adapter_ce, CSPEED_STRL(DBADAPTER_CONFIGS), ZEND_ACC_PRIVATE);
}/*}}}*/






