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

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_db_adapter_set_error_callback, 0, 0, 1)
    ZEND_ARG_INFO(0, callback)
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
    zval *shardingKey = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("shardingKey"));

    /*}}}*/
    SQL_PARSER_RESULT *sql_result = parse_sql(ZSTR_VAL(sql_command), Z_STRVAL_P(shardingKey));
    
    /**
     * Update the SQL type to the property
     */
    zend_update_property_long( Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL_TYPE), sql_result->sql_type );
    
    /**
     * [SELECT]: type.
     */
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
            int length = 0;
            zval *table_name; zend_long index;

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
            if ( CSPEED_STRING_NOT_EMPTY(where_opts) ) {
                strncat(select_result, " ", strlen(" "));
                strncat(select_result, where_opts, strlen(where_opts));
            }
            if ( CSPEED_STRING_NOT_EMPTY(groupby_opts) ) {
                strncat(select_result, " ", strlen(" "));
                strncat(select_result, groupby_opts, strlen(groupby_opts));
            }
            if ( CSPEED_STRING_NOT_EMPTY(orderby_opts) ) {
                strncat(select_result, " ", strlen(" "));
                strncat(select_result, orderby_opts, strlen(orderby_opts));
            }
            if ( CSPEED_STRING_NOT_EMPTY(limit_opts) ) {
                strncat(select_result, " ", strlen(" "));
                strncat(select_result, limit_opts, strlen(limit_opts));
            }
            /* SQL result strncat ending. */
        }

        /* To concat the SQL with the UNION SQL keyword */
        int raw_sql = 0;
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
            if (strncmp(select_result, Z_STRVAL(ret_val), Z_STRLEN(ret_val)) == 0)
            {
                raw_sql = 1;
                smart_str_appends(&sql, Z_STRVAL(ret_val));
                break;
            }
            else 
            {
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
                raw_sql = 0;
            }
        }
        /* smart_str end */
        smart_str_0(&sql);
        zend_update_property_str(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), sql.s);
        smart_str_free(&sql);
        free(select_result);
        select_result = NULL;
    }  else if (sql_result->sql_type == 2) {
        /**
         * 2 : means the UPDATE command
         * format: 
         *    UPDATE update_table_name SET update_opts where_opts
         * WHEN doing the UPDATE or INSERT operation, 
         * User must provide the `shardingKey` to let the system to 
         * do the right job: choosing the right db to do.
         */
        int length = 0;
        
        size_t sql_length = strlen(sql_result->update_statement.update)
                          + strlen(sql_result->update_statement.table_name)
                          + strlen(sql_result->update_statement.set)
                          + strlen(sql_result->update_statement.update_opts)
                          + strlen(sql_result->update_statement.where_opts);
        char *update_result = (char *)malloc(sizeof(char) * sql_length);
        memset(update_result, 0, sql_length);

        /* The folowing job is to strncat the SQL */
        strncat(update_result, sql_result->update_statement.update, strlen(sql_result->update_statement.update));
        strncat(update_result, " ", strlen(" "));
        
        if (zend_hash_num_elements(Z_ARRVAL_P(tables))){

            /**
             * Some variables for the Loop
             */
            zval *table_name; zend_long index;

            /* Some temp variables */
            char tableName[256] = {0};
            char temp_tableName[256] = {0};
            strcpy(tableName, sql_result->update_statement.table_name);

            char update_opts[256] = {0};
            char temp_update_opts[256] = {0};
            strcpy(update_opts, sql_result->update_statement.update_opts);

            char where_opts[256] = {0};
            char temp_where_opts[256] = {0};
            strcpy(where_opts, sql_result->update_statement.where_opts);

            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(tables), index, table_name)
            {
                if ( (Z_TYPE_P(table_name) == IS_STRING) 
                    && (CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(tables))) )
                {
                    /**
                     * The table faker name
                     */
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
                }

                /**
                 * Replace the table name to the faker name;
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->update_statement.table_name))
                {
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
                 * update_opts
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->update_statement.update_opts))
                {
                    bzero(temp_update_opts, strlen(temp_update_opts));
                    replace_all_name(
                        update_opts, 
                        Z_STRVAL_P(table_name),
                        ZSTR_VAL(
                            strpprintf(
                                0,
                                "?%d",
                                index
                            )
                        ), 
                        temp_update_opts,
                        &length,
                        1
                    );
                    strcpy(update_opts, temp_update_opts);
                }

                /**
                 * where_opts
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->update_statement.where_opts))
                {
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
            } ZEND_HASH_FOREACH_END();

            /**
             * strncat the SQL to the executed sql
             */
            strncat(update_result, tableName, strlen(tableName));
            strncat(update_result, " ", strlen(" "));
            strncat(update_result, sql_result->update_statement.set, strlen(sql_result->update_statement.set));
            strncat(update_result, " ", strlen(" "));
            strncat(update_result, update_opts, strlen(update_opts));
            if ( CSPEED_STRING_NOT_EMPTY(where_opts) )
            {
                strncat(update_result, " ", strlen(" "));
                strncat(update_result, where_opts, strlen(where_opts));
            }

            /**
             * Caculator the string's hash value to the right db
             */
            char *sharding_key_value = sql_result->update_statement.sharding_key_value;
            if (sharding_key_value == NULL || !sharding_key_value)
            {
                cspeed_print_info(
                    E_ERROR,
                    "UPDATE command must provide `shardingKey` IN WHERE condition."
                );
            }
            unsigned long long int crc_value = string_crc(sharding_key_value, strlen(sharding_key_value));
            unsigned int table_hash_value = crc_value % Z_LVAL_P(hSize);

            /* To replace */
            zend_long index_t;
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
                        table_hash_value
                    )
                );
            } ZEND_HASH_FOREACH_END();
            zval function_name, ret_val, subject;
            ZVAL_STRING(&subject, update_result);
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
            zval_ptr_dtor(&subject);
            zval_ptr_dtor(&table_replace_subject);
            /**
             * Set the SQL to the RAW_SQL property
             */
            zend_update_property_str(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), Z_STR(ret_val));
            zend_update_property_long(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(DBADAPTER_DB_HASH), table_hash_value);
            zval_ptr_dtor(&ret_val);
            free(update_result);
            update_result = NULL;
        }
        free(sql_result->sql_result);
        sql_result->sql_result = NULL;
        RETURN_ZVAL(getThis(), 1, NULL);

    }  else if (sql_result->sql_type == 3) {
        /**
         * DELETE type
         *  format:
         *      DELETE FROM delete_table_name where_opts
         */
        char *sharding_key_value = sql_result->delete_statement.sharding_key_value;
        if (sharding_key_value == NULL || !sharding_key_value)
        {
            cspeed_print_info(
                E_ERROR,
                "DELETE command must provide `shardingKey` WHERE condition."
            );
        }
        unsigned long long int crc_value = string_crc(sharding_key_value, strlen(sharding_key_value));
        unsigned int table_hash_value = crc_value % Z_LVAL_P(hSize);

        int length = 0;
        
        size_t sql_length = strlen(sql_result->delete_statement.delete)
                          + strlen(sql_result->delete_statement.from)
                          + strlen(sql_result->delete_statement.table_name)
                          + strlen(sql_result->delete_statement.where_opts);
        char *delete_result = (char *)malloc(sizeof(char) * sql_length);
        memset(delete_result, 0, sql_length);

        /* The folowing job is to strncat the SQL */
        strncat(delete_result, sql_result->delete_statement.delete, strlen(sql_result->delete_statement.delete));
        strncat(delete_result, " ", strlen(" "));
        strncat(delete_result, sql_result->delete_statement.from, strlen(sql_result->delete_statement.from));

        if (zend_hash_num_elements(Z_ARRVAL_P(tables))){

            /**
             * Some variables for the Loop
             */
            zval *table_name; zend_long index;

            /* Some temp variables */
            char tableName[256] = {0};
            char temp_tableName[256] = {0};
            strcpy(tableName, sql_result->delete_statement.table_name);

            char where_opts[256] = {0};
            char temp_where_opts[256] = {0};
            strcpy(where_opts, sql_result->delete_statement.where_opts);

            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(tables), index, table_name)
            {
                if ( (Z_TYPE_P(table_name) == IS_STRING) 
                    && (CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(tables))) )
                {
                    /**
                     * The table faker name
                     */
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
                }

                /**
                 * Replace the table name to the faker name;
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->delete_statement.table_name))
                {
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
                 * where_opts
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->delete_statement.where_opts))
                {
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
            } ZEND_HASH_FOREACH_END();

            /**
             * strncat the SQL to the executed sql
             */
            strncat(delete_result, " ", strlen(" "));
            strncat(delete_result, tableName, strlen(tableName));
            if ( CSPEED_STRING_NOT_EMPTY(where_opts) )
            {
                strncat(delete_result, " ", strlen(" "));
                strncat(delete_result, where_opts, strlen(where_opts));
            }

            /* To replace */
            zend_long index_t;
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
                        table_hash_value
                    )
                );
            } ZEND_HASH_FOREACH_END();
            zval function_name, ret_val, subject;
            ZVAL_STRING(&subject, delete_result);
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
            zval_ptr_dtor(&subject);
            zval_ptr_dtor(&table_replace_subject);
            /**
             * Set the SQL to the RAW_SQL property
             */
            zend_update_property_str(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), Z_STR(ret_val));
            zend_update_property_long(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(DBADAPTER_DB_HASH), table_hash_value);
            zval_ptr_dtor(&ret_val);
            free(delete_result);
            delete_result = NULL;
        }
        free(sql_result->sql_result);
        sql_result->sql_result = NULL;
        RETURN_ZVAL(getThis(), 1, NULL);
    }  else if (sql_result->sql_type == 4) {
        /**
         * INSERT type
         * Caculator the string's hash value to the right db
         */
        char *sharding_key_value = sql_result->insert_statement.sharding_key_value;
        if (sharding_key_value == NULL || !sharding_key_value)
        {
            cspeed_print_info(
                E_ERROR,
                "INSERT command must provide `shardingKey` IN fields & values."
            );
        }
        unsigned long long int crc_value = string_crc(sharding_key_value, strlen(sharding_key_value));
        unsigned int table_hash_value = crc_value % Z_LVAL_P(hSize);

        int length = 0;
        
        size_t sql_length = strlen(sql_result->insert_statement.insert_into)
                          + strlen(sql_result->insert_statement.table_name)
                          + strlen(sql_result->insert_statement.fields)
                          + strlen(sql_result->insert_statement.values) + sizeof(char) * 15;
        char *insert_result = (char *)malloc(sizeof(char) * sql_length);
        memset(insert_result, 0, sql_length);

        /* The folowing job is to strncat the SQL */
        strncat(insert_result, sql_result->insert_statement.insert_into, strlen(sql_result->insert_statement.insert_into));
        strncat(insert_result, " ", strlen(" "));

        if (zend_hash_num_elements(Z_ARRVAL_P(tables))){

            /**
             * Some variables for the Loop
             */
            zval *table_name; zend_long index;

            /* Some temp variables */
            char tableName[256] = {0};
            char temp_tableName[256] = {0};
            strcpy(tableName, sql_result->insert_statement.table_name);

            char fields[256] = {0};
            char temp_fields[256] = {0};
            strcpy(fields, sql_result->insert_statement.fields);

            char values[256] = {0};
            char temp_values[256] = {0};
            strcpy(values, sql_result->insert_statement.values);

            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(tables), index, table_name)
            {
                if ( (Z_TYPE_P(table_name) == IS_STRING) 
                    && (CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(tables))) )
                {
                    /**
                     * The table faker name
                     */
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
                }

                /**
                 * Replace the table name to the faker name;
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->insert_statement.table_name))
                {
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
                 * fields
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->insert_statement.fields))
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
                        1
                    );
                    strcpy(fields, temp_fields);
                }

                /**
                 * values
                 */
                if (CSPEED_STRING_NOT_EMPTY(sql_result->insert_statement.values))
                {
                    bzero(temp_values, strlen(temp_values));
                    replace_all_name(
                        values, 
                        Z_STRVAL_P(table_name),
                        ZSTR_VAL(
                            strpprintf(
                                0,
                                "?%d",
                                index
                            )
                        ), 
                        temp_values,
                        &length,
                        1
                    );
                    strcpy(values, temp_values);
                }

            } ZEND_HASH_FOREACH_END();
            /**
             * strncat the SQL to the executed sql
             */
            strncat(insert_result, tableName, strlen(tableName));
            strncat(insert_result, " (", strlen(" ("));
            strncat(insert_result, fields, strlen(fields));
            strncat(insert_result, ") VALUES(", strlen(") VALUES("));
            strncat(insert_result, values, strlen(values));
            strncat(insert_result, ")", strlen(")"));
            /* To replace */
            zend_long index_t;
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
                        table_hash_value
                    )
                );
            } ZEND_HASH_FOREACH_END();
            zval function_name, ret_val, subject;
            ZVAL_STRING(&subject, insert_result);
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
            zval_ptr_dtor(&subject);
            zval_ptr_dtor(&table_replace_subject);
            /**
             * Set the SQL to the RAW_SQL property
             */
            zend_update_property_str(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), Z_STR(ret_val));
            zend_update_property_long(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(DBADAPTER_DB_HASH), table_hash_value);
            zval_ptr_dtor(&ret_val);
            free(insert_result);
            insert_result = NULL;
        }
        free(sql_result->sql_result);
        sql_result->sql_result = NULL;
        RETURN_ZVAL(getThis(), 1, NULL);
    }

    /**
     * If the grammer error occur.
     */
    if (sql_result->sql_type >= 1064) {
        free(sql_result->sql_result);
        zval *error_callback = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(CSPEED_DB_ERROR_CALLBACK), 1, NULL);
        if ( error_callback && !ZVAL_IS_NULL(error_callback) )
        {
            zval ret_val, code, info;
            ZVAL_LONG(&code, 1064);
            ZVAL_STRING(&info, sql_result->sql_result);
            zval params[] = { code, info };
            call_user_function(
                EG(function_table), 
                NULL, 
                error_callback, 
                &ret_val, 
                2, 
                params
            );
            /**
             * If the return value was true, continue the running.
             * otherwise to exit the program
             */
            if ( Z_TYPE(ret_val) == IS_TRUE ) {
                zval_ptr_dtor(&ret_val);
            } else {
                zval_ptr_dtor(&ret_val);
                cspeed_print_info(
                    E_ERROR,
                    "SQL: 1064 %s.",
                    sql_result->sql_result
                );
            }
        }
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

    /**
     * Verify the config was right or not.
     */
    if (Z_TYPE_P(db_configs) == IS_ARRAY) 
    {
        /**
         * Verify the config was right or not.
         * if not right. to exit the program and print the error info plus to give some advies.
         */
        zval *tables = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("tables"));
        if ( Z_TYPE_P(tables) != IS_ARRAY ) 
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be an array.", "tables");
        }

        /**
         * shardingKey
         */
        zval *shardingKey = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("shardingKey"));
        if ( (Z_TYPE_P(shardingKey) != IS_STRING) 
            || !CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(shardingKey)) )
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be valid string.", "shardingKey");
        }

        /**
         * hSize
         */
        zval *hSize = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("hSize"));
        if ( Z_TYPE_P(hSize) != IS_LONG )
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be integer value.", "hSize");
        }

        /**
         * readDb
         */
        zval *readDb = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("readDb"));
        if ( Z_TYPE_P(readDb) != IS_ARRAY )
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be array.", "readDb");
        }

        /**
         * writeDb
         */
        zval *writeDb = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("writeDb"));
        if ( Z_TYPE_P(writeDb) != IS_ARRAY )
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be array.", "writeDb");
        }

        /**
         * Balance
         */
        zval *balance = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("balance"));
        if ( Z_TYPE_P(balance) != IS_LONG ||
            Z_LVAL_P(balance) > 3 )
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be integer, from 1 to 3", "balance");
        }

        /**
         * displayDbNotExistError config
         */
        zval *displayDbNotExistError = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("displayDbNotExistError"));
        if ( (Z_TYPE_P(displayDbNotExistError) !=  IS_FALSE) &&
             (Z_TYPE_P(displayDbNotExistError) !=  IS_TRUE)  )
        {
            cspeed_print_info(E_ERROR, "Config: `%s` must be boolean.", "displayDbNotExistError");
        }

        /* If all configs were passed. update the value to the property. */
        zend_update_property(
            Z_OBJCE_P(getThis()),
            getThis(),
            CSPEED_STRL(DBADAPTER_CONFIGS),
            db_configs
        );

        /* After update return true */
        RETURN_TRUE;

    } else {
        /**
         * If config verify fail. print the error info
         * and exit the program
         */
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
     * db was an array, so it must be ZEND_HASH_FOREACH_NUM_KEY_VAL()
     */
    zval *db;
    zval *value;
    zend_long index;
    zval *sql_statement = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL), 1, NULL);
    zval *sql_type      = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(RAW_SQL_TYPE), 1, NULL);
    zval *db_configs    = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(DBADAPTER_CONFIGS), 1, NULL);
    zval *db_hash       = zend_read_property(Z_OBJCE_P(getThis()), getThis(), CSPEED_STRL(DBADAPTER_DB_HASH), 1, NULL);
    /**
     * Some configs from the db_configs which input by users.
     */
    zval *balance_value = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("balance"));
    zval *display_db_error = zend_hash_str_find(Z_ARRVAL_P(db_configs), CSPEED_STRL("displayDbNotExistError"));

    /**
     * Which db mode was buy balance config & readDb or writeDb
     */
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

    if (Z_LVAL_P(sql_type) == 1) {
        /**
         * The result of the SQL execute
         * initialise it
         */
        zval ultimate_result;
        array_init(&ultimate_result);

        /**
         * Loop the db array to find the db to run the SQL or execute the SQL
         */
        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(db), index, value)
        {
            /**
             * If the db name was empty string. skip it
             */
            if ( !CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(value)) )
            {
                continue;
            }

            /**
             * To find the db from the di container.
             */
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
                cspeed_exit(Z_STRVAL_P(sql_statement));
                cspeed_pdo_execute_sql(adapter_pdo_object, getThis(), Z_STRVAL_P(sql_statement), NULL, &temp_result, 1);
                zend_hash_merge(Z_ARRVAL(ultimate_result), Z_ARRVAL(temp_result), zval_add_ref, 0);
                zval_ptr_dtor(&temp_result);
            }
            else
            {
                if ( Z_TYPE_P(display_db_error) == IS_TRUE ) {
                    /**
                     * Display the error info. using the cspeed_print_info function to do
                     * this job. instead of the php_error_docref
                     */
                    cspeed_print_info(
                        E_ERROR,
                        "The db `%s` not exists in Di container.",
                        Z_STRVAL_P(value)
                    );
                } else {
                    /**
                     * Not display the db not exist error
                     * So when need to do other info. write the following todo code...
                     */
                    /* TODO... */
                }
            }
        } ZEND_HASH_FOREACH_END();

        /**
         * Return the result of the all value
         * it may be 
         */
        RETURN_ZVAL(&ultimate_result, 1, NULL);
    }
    else if ( (Z_LVAL_P(sql_type) == 2) || (Z_LVAL_P(sql_type) == 3) || (Z_LVAL_P(sql_type) == 4) ) { /* UPDATE|DELETE|INSERT */
        zval *adapter_name = zend_hash_index_find(Z_ARRVAL_P(db), Z_LVAL_P(db_hash));
        /**
         * If the db name was empty string. skip it
         */
        if ( !adapter_name || ZVAL_IS_NULL(adapter_name) || !CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(adapter_name)) )
        {
            cspeed_print_info(E_ERROR, "Can't access the [%d] db.", Z_LVAL_P(db_hash));
        }

        /**
         * To find the db from the di container.
         */
        if (EXPECTED( (adapter_db = zend_hash_find(Z_ARRVAL_P(objects), Z_STR_P(adapter_name)) ) != NULL ))
        {
            /* The db adapter exsits. set the db */
            zval *adapter_pdo_object = zend_read_property(
                Z_OBJCE_P(getThis()), 
                adapter_db, 
                CSPEED_STRL(CSPEED_DB_THIS_PDO), 
                1, 
                NULL
            );
            cspeed_exit(Z_STRVAL_P(sql_statement));
            cspeed_pdo_execute_sql(adapter_pdo_object, getThis(), Z_STRVAL_P(sql_statement), NULL, return_value, 0);
        }
        else
        {
            if ( Z_TYPE_P(display_db_error) == IS_TRUE ) {
                /**
                 * Display the error info. using the cspeed_print_info function to do
                 * this job. instead of the php_error_docref
                 */
                cspeed_print_info(
                    E_ERROR,
                    "The db `%s` not exists in Di container.",
                    Z_STRVAL_P(value)
                );
            } else {
                /**
                 * Not display the db not exist error
                 * So when need to do other info. write the following todo code...
                 */
                /* TODO... */
            }
        }
    }
}/*}}}*/

CSPEED_METHOD(DbAdapter, setErrorCallback)
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
        cspeed_db_adapter_ce, 
        getThis(),
        CSPEED_STRL(CSPEED_DB_ERROR_CALLBACK), 
        error_callback
    );
}

/*{{{ ALL functions the DbAdapter class contains. */
static const zend_function_entry db_adapter_functions[] = {
    CSPEED_ME(DbAdapter, __construct, arginfo_cspeed_db_adapter_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, createCommand, arginfo_cspeed_db_adapter_create_command, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, execute, arginfo_cspeed_db_adapter_execute, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, loadConfig, arginfo_cspeed_db_adapter_load_config, ZEND_ACC_PUBLIC)
    CSPEED_ME(DbAdapter, setErrorCallback, arginfo_cspeed_db_adapter_set_error_callback, ZEND_ACC_PUBLIC)

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
    zend_declare_property_null(cspeed_db_adapter_ce, CSPEED_STRL(DBADAPTER_DB_HASH), ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_db_adapter_ce, CSPEED_STRL(CSPEED_DB_ERROR_CALLBACK), ZEND_ACC_PROTECTED);
}/*}}}*/






