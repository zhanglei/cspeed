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

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cspeed.h"

#include "kernel/tool/helper.h"
#include "kernel/mvc/model.h"

#include "kernel/CApp.h"
#include "kernel/di/di.h"
#include "kernel/db/adapter.h"
#include "kernel/tool/component.h"

#include "zend_smart_str.h"             /* Use the smart_str */

void 
reset_model_sql(zval *this) /*{{{ To clear the current sql result for the next execute */
{
    zend_update_property_string(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_WHERE_COND), "");
    zend_update_property_string(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_ORDER_BY), "");
    zend_update_property_string(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_SELECT), "*");
    zend_update_property_string(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_GROUP_BY), "");
}/*}}}*/

void
get_fields_from_object(zval *object, zval *ret_val) /*{{{ Get the fields from the object */
{
    /* When in save mode, the primary key will be add to the where contidition */
    zval *prop;
    zend_string *key;

    zval *columns = zend_read_property(cspeed_model_ce, object, CSPEED_STRL(CSPEED_MODEL_COLUMNS), 1, NULL);
    if (columns == NULL) return ;

    zval *new_record  = zend_read_property(cspeed_model_ce, object, CSPEED_STRL(CSPEED_MODEL_NEW_RECORD), 1, NULL);
    zval *primary_key = zend_read_property(cspeed_model_ce, object, CSPEED_STRL(CSPEED_MODEL_PRIMARY), 1, NULL);
    zval *where       = zend_read_property(cspeed_model_ce, object, CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 1, NULL);
    zval *array_data  = zend_read_property(cspeed_model_ce, object, CSPEED_STRL(CSPEED_MODEL_ARRAY_DATA), 1, NULL);
    
    int may_skip = 0;

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_OBJ_P(object)->properties, key, prop) {
        
        zval *column_key;
        int has_field = 0; 
        
        if ( !CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(key)) ) {
            continue;
        }

        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(columns), column_key) {
            if ( strncmp(ZSTR_VAL(key), Z_STRVAL_P(column_key), Z_STRLEN_P(column_key) ) == 0 ) {
                has_field = 1;
                break;
            }
        } ZEND_HASH_FOREACH_END();

        if ( !has_field ) {
            continue ;
        }

        /* The fields which suit the database's column */
        add_assoc_zval(ret_val, ZSTR_VAL(key), prop);

        /* if doing the UPDATE operation */
        if ( Z_LVAL_P(new_record) == IS_FALSE ) {
            if ( !may_skip && array_data && ( !ZVAL_IS_NULL(array_data) ) ) {
                zval result;
                /* Add the where condition */
                if ( where && CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(where)) ) {
                    cspeed_build_equal_string(array_data, "", &result);
                    zend_update_property_string(cspeed_model_ce, object, 
                                            CSPEED_STRL(CSPEED_MODEL_WHERE_COND), ZSTR_VAL(strpprintf(0, "%s AND %s", 
                                                Z_STRVAL_P(where), Z_STRVAL(result))));
                    may_skip = 1;
                } else {
                    zend_string *s_key;zval *s_value;
                    cspeed_build_equal_string(array_data, " WHERE ", &result);
                    zend_update_property_string(cspeed_model_ce, object, 
                                            CSPEED_STRL(CSPEED_MODEL_WHERE_COND), ZSTR_VAL(strpprintf(0, "%s", Z_STRVAL(result))));
                    may_skip = 1;
                }
                zval_ptr_dtor(&result);
            }
        }
    } ZEND_HASH_FOREACH_END();
}/*}}}*/

void
set_columns_to_model(zval *model_object)
{
    /* Get the columns for the table */
    zval ret, pdo_statement, retval_data;
    zval *table_name = zend_read_property(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), 1, NULL);
    zval *pdo_object_set = zend_read_property(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    zend_string *columns_sql = strpprintf(0, "SHOW COLUMNS FROM `%s`", Z_STRVAL_P(table_name));
    cspeed_pdo_prepare(pdo_object_set, ZSTR_VAL(columns_sql), &pdo_statement);
    cspeed_pdo_statement_set_fetch_mode(&pdo_statement, 2, &ret);
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval_data);
    
    if ( !output_sql_errors(&pdo_statement) ){
        zval result;
        cspeed_pdo_statement_fetch_all(&pdo_statement, &result);

        zval *each_column, column_data;
        array_init(&column_data);

        /* set the primary or not */
        int is_set_primary = 0;

        ZEND_HASH_FOREACH_VAL(Z_ARRVAL(result), each_column) {
            if ( each_column && ( Z_TYPE_P( each_column ) == IS_ARRAY) ) {
                zval *column_name = zend_hash_find(Z_ARRVAL_P(each_column), strpprintf(0, "Field"));
                if ( column_name != NULL ) {
                    add_next_index_string(&column_data, Z_STRVAL_P(column_name));
                }
                if ( !is_set_primary ) {
                    /* The primary key name, if set */
                    zval *primary_key = zend_hash_find(Z_ARRVAL_P(each_column), strpprintf(0, "Key"));
                    if ( (primary_key != NULL) && ( CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(primary_key))) ) {
                        zend_update_property_str(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_PRIMARY), Z_STR_P(column_name));
                        is_set_primary = 1;
                    }
                }
            }
        } ZEND_HASH_FOREACH_END();
        
        /* Set the data */
        zend_update_property(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_COLUMNS), &column_data);
    }
}

void 
initialise_the_model_object(zval *model_object, zend_long new_record, INTERNAL_FUNCTION_PARAMETERS)/*{{{*/
{
    zval ret_val;

    /* Fetch the PDO object from the given Di class */
    zval *pdo_object = zend_read_static_property(cspeed_adapter_ce, CSPEED_STRL(CSPEED_DB_PDO_OBJECT), 1);

    if (pdo_object == NULL || ZVAL_IS_NULL(pdo_object) ) {
        php_error_docref(NULL, E_ERROR, "Please set the Adapter class to \\Cs\\di\\Di container.");
    }
    /* Restore the PDO object into the Class property */
    zval *is_set_pdo = zend_read_property(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    if (ZVAL_IS_NULL(is_set_pdo)) {
        zval_add_ref(pdo_object);
        zend_update_property(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), pdo_object);
    }

    /* Getting which table must be fetch */
    call_method_with_object(model_object, "tableName", 0, NULL, &ret_val);
    zend_update_property_string(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), Z_STRVAL(ret_val));

    /* Update the value to IS_TRUE */
    if (new_record == IS_TRUE){
        zend_update_property_long(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_NEW_RECORD), IS_TRUE);
    } else if (new_record == IS_FALSE) {
        zend_update_property_long(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_NEW_RECORD), IS_FALSE);
    }

    /* Default format was object */
    zend_update_property_long(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_AS_ARRAY), IS_FALSE);

    /* Invoke the model's initialise method */
    zval params[] = { *model_object };
    call_method_with_object(model_object, "initialise", 1, params, &ret_val);
    zval_ptr_dtor(&ret_val);
}/*}}}*/

char *
build_insert_field_datas(zval *this)  /*{{{ Building the INSERT FIELDS AND VALUE */
{
    // zval *magic_datas = zend_read_property(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_MAGIC_DATAS), 1, NULL);
    zval magic_datas;
    array_init(&magic_datas);
    get_fields_from_object(this, &magic_datas);

    zend_string *var_key; zval *var_value;
    if ( !ZVAL_IS_NULL(&magic_datas) && (Z_TYPE_P(&magic_datas) == IS_ARRAY)  
            && zend_hash_num_elements(Z_ARRVAL_P(&magic_datas)) ){
        smart_str fields_str = {0};     /* INSERT INTO xx (id, i, t) VALUES(11, 22, 33); */
        smart_str datas_str  = {0};
        smart_str_appendc(&fields_str, '(');
        smart_str_appends(&datas_str, " VALUES(");
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(&magic_datas), var_key, var_value) {
            smart_str_appends(&fields_str, ZSTR_VAL(var_key));
            smart_str_appendc(&fields_str, ',');                         /* After finished, the value is (id, i, t, */
            /* VALUES */
            if (Z_TYPE_P(var_value) == IS_LONG) {
                smart_str_appends(&datas_str, ZSTR_VAL(strpprintf(0, "'%d'", Z_LVAL_P(var_value))));
                smart_str_appendc(&datas_str, ',');
            } else if (Z_TYPE_P(var_value) == IS_STRING) {
                smart_str_appends(&datas_str, ZSTR_VAL(strpprintf(0, "'%s'", Z_STRVAL_P(var_value))));
                smart_str_appendc(&datas_str, ',');
            } else if (Z_TYPE_P(var_value) == IS_DOUBLE) {
                smart_str_appends(&datas_str, ZSTR_VAL(strpprintf(0, "'%f'", Z_DVAL_P(var_value))));
                smart_str_appendc(&datas_str, ',');
            }
        } ZEND_HASH_FOREACH_END();

        /* char *str = "abcd" */
        *(ZSTR_VAL(fields_str.s) + ZSTR_LEN(fields_str.s) - 1) = ')';
        *(ZSTR_VAL(datas_str.s) + ZSTR_LEN(datas_str.s) - 1) = ')';
        
        /* End the smart_str */
        smart_str_0(&fields_str);
        smart_str_0(&datas_str);

        zend_string *result = strpprintf(0, "%s%s", ZSTR_VAL(fields_str.s), ZSTR_VAL(datas_str.s));
        smart_str_free(&fields_str);
        smart_str_free(&datas_str);
        return ZSTR_VAL(result);
    } else {
        return "";
    }
}/*}}}*/

char *
build_update_sql(zval *this)  /*{{{*/
{
    /* UPDATE xx SET a=3,b=3,d=2 */
    // zval *magic_datas = zend_read_property(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_MAGIC_DATAS), 1, NULL);
    zval magic_datas;
    array_init(&magic_datas);
    get_fields_from_object(this, &magic_datas);

    zend_string *var_key; zval *var_value;
    if ( !ZVAL_IS_NULL(&magic_datas) && Z_TYPE(magic_datas) == IS_ARRAY  
            && zend_hash_num_elements(Z_ARRVAL(magic_datas)) ){
        smart_str update_str = {0};
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(magic_datas), var_key, var_value) {
            smart_str_appends(&update_str, ZSTR_VAL(var_key));
            smart_str_appendc(&update_str, '=');
            if (Z_TYPE_P(var_value) == IS_LONG) {
                smart_str_appends(&update_str, ZSTR_VAL(strpprintf(0, "'%d'", Z_LVAL_P(var_value))));
            } else if (Z_TYPE_P(var_value) == IS_STRING) {
                smart_str_appends(&update_str, ZSTR_VAL(strpprintf(0, "'%s'", Z_STRVAL_P(var_value))));
            } else if (Z_TYPE_P(var_value) == IS_DOUBLE) {
                smart_str_appends(&update_str, ZSTR_VAL(strpprintf(0, "'%f'", Z_DVAL_P(var_value))));
            }
            smart_str_appendc(&update_str, ',');
        } ZEND_HASH_FOREACH_END();

        /* char *str = "abcd" */
        *(ZSTR_VAL(update_str.s) + ZSTR_LEN(update_str.s) - 1) = '\0';
        
        /* End the smart_str */
        smart_str_0(&update_str);
        
        zend_string *result = strpprintf(0, "%s", ZSTR_VAL(update_str.s));
        smart_str_free(&update_str);
        return ZSTR_VAL(result);
    } else {
        return "";
    }

}/*}}}*/

void 
build_delete_sql(zval *this)/* {{{ Build the DELETE where condition */
{
    // zval *magic_datas = zend_read_property(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_MAGIC_DATAS), 1, NULL);
    zval magic_datas;
    array_init(&magic_datas);
    get_fields_from_object(this, &magic_datas);

    if ( !ZVAL_IS_NULL(&magic_datas) && Z_TYPE(magic_datas) == IS_ARRAY  
            && zend_hash_num_elements(Z_ARRVAL(magic_datas)) ){
        zval result;
        cspeed_build_equal_string(&magic_datas, " WHERE ", &result);
        zend_update_property_string(cspeed_model_ce, this, 
            CSPEED_STRL(CSPEED_MODEL_WHERE_COND), Z_STRVAL(result));
        zval_ptr_dtor(&result);
    }
}/*}}}*/

void
set_model_ultimate_pdo(zval *this) /*{{{ To set the finally pdo object to do the job. */
{
    zval *is_set_ultimate_pdo = zend_read_property(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_ULTIMATE_PDO), 1, NULL);

    if ( !ZVAL_IS_NULL(is_set_ultimate_pdo) ) {
        zend_update_property(cspeed_model_ce, this, CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), is_set_ultimate_pdo);
    }
}/*}}}*/

void
set_executed_sql(zval *model_object, zend_string *sql) /*{{{ Set the executed sql to the model */
{
    zend_update_property_str(cspeed_model_ce, model_object, CSPEED_STRL(CSPEED_MODEL_EXECUTED_SQL), sql);
}/*}}}*/

/* {{{ All ARG-INFO for the Model class */
ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_find, 0, 0, 1)
    ZEND_ARG_INFO(0, where_conditions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_select, 0, 0, 1)
    ZEND_ARG_INFO(0, fields)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_where, 0, 0, 1)
    ZEND_ARG_INFO(0, where_conditions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_table_name, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_and_where, 0, 0, 1)
    ZEND_ARG_INFO(0, where_conditions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_order_by, 0, 0, 1)
    ZEND_ARG_INFO(0, order_by_conditions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_group_by, 0, 0, 1)
    ZEND_ARG_INFO(0, group_by_conditions)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_one, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_all, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_save, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_delete, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_set_db, 0, 0, 1)
    ZEND_ARG_INFO(0, di_db_name)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_get_db, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_get_adapter, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_get_executed_sql, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_cspeed_model_as_array, 0, 0, 0)
ZEND_END_ARG_INFO()
/* }}} */

CSPEED_METHOD(Model, __construct)/*{{{ proto Model::construct()*/
{
    initialise_the_model_object(getThis(), IS_TRUE, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}/*}}}*/

CSPEED_METHOD(Model, find)/*{{{ proto Model::find() To do the update() method */
{
    zval model_object;
    object_init_ex(&model_object, zend_get_called_scope(execute_data));

    /* Do some initialise job */
    initialise_the_model_object(&model_object, IS_FALSE, INTERNAL_FUNCTION_PARAM_PASSTHRU);

    /* Return the Model object */
    RETURN_ZVAL(&model_object, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, where)/*{{{ proto Model::where()*/
{
    zval *where;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &where) == FAILURE) {
        return ;
    }
    if ( where && (Z_TYPE_P(where) == IS_ARRAY) ){
        /*zend_string *val_key;*/
        zval result;
        cspeed_build_equal_string(where, " WHERE ", &result);
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), Z_STRVAL(result));
        zval_ptr_dtor(&result);
    } else if ( where && ( Z_TYPE_P(where) == IS_STRING)) {
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 
                                                    ZSTR_VAL(strpprintf(0, " WHERE %s", Z_STRVAL_P(where))));
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter can only be array or string.");
    }
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, andWhere)/*{{{ proto Model::andWhere()*/
{
    zval *where;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &where) == FAILURE) {
        return ;
    }

    zval *previous_where = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 1, NULL);
    if (where && (Z_TYPE_P(where) == IS_ARRAY)){
        /* Get the where condition */
        zval result;
        cspeed_build_equal_string(where, "", &result);
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 
            ZSTR_VAL(strpprintf(0, "%s AND %s ", Z_STRVAL_P(previous_where),  Z_STRVAL(result))));
        zval_ptr_dtor(&result);
    } else if(where && ( Z_TYPE_P(where) == IS_STRING )){
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 
            ZSTR_VAL(strpprintf(0, "%s AND %s", Z_STRVAL_P(previous_where), Z_STRVAL_P(where)))
        );
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter can only be array or string.");
    }
    zval_ptr_dtor(where);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, orderBy)/*{{{ proto Model::orderBy()*/
{
    zval *order_by;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &order_by) == FAILURE) {
        return ;
    }
    if (order_by && (Z_TYPE_P(order_by) == IS_ARRAY)){
        zval *var_value;
        smart_str order_by_str = {0};
        smart_str_appends(&order_by_str, " ORDER BY ");
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(order_by), var_value) {
            if (Z_TYPE_P(var_value) == IS_STRING) {
                smart_str_appends(&order_by_str, Z_STRVAL_P(var_value));
                smart_str_appendc(&order_by_str, ',');
            }
        } ZEND_HASH_FOREACH_END();
        smart_str_0(&order_by_str);
        char *temp_order_by = (char *)malloc(sizeof(char) * (ZSTR_LEN(order_by_str.s)));
        memset(temp_order_by, 0, ZSTR_LEN(order_by_str.s));
        memcpy(temp_order_by, ZSTR_VAL(order_by_str.s), ZSTR_LEN(order_by_str.s) - 1);
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ORDER_BY), temp_order_by);
        free(temp_order_by);
        smart_str_free(&order_by_str);
    } else if( order_by && (Z_TYPE_P(order_by) == IS_STRING)){
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ORDER_BY), 
            ZSTR_VAL(strpprintf(0, " ORDER BY %s", Z_STRVAL_P(order_by))));
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter can only be array or string.");
    }
    zval_ptr_dtor(order_by);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, groupBy)/*{{{ proto Model::groupBy()*/
{
    zval *group_by;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &group_by) == FAILURE) {
        return ;
    }
    if (group_by && (Z_TYPE_P(group_by) == IS_ARRAY) ){
        zval result;
        cspeed_build_quote_string(group_by, &result); 
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_GROUP_BY), 
            ZSTR_VAL(strpprintf(0, " GROUP BY %s", Z_STRVAL(result)))
        );
        zval_ptr_dtor(&result);
    } else if (group_by && (Z_TYPE_P(group_by) == IS_STRING)){
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_GROUP_BY), 
            ZSTR_VAL(strpprintf(0, " GROUP BY %s", Z_STRVAL_P(group_by))));
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter can only be array or string.");
    }
    zval_ptr_dtor(group_by);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, tableName) /*{{{ proto Model::tableName()*/
{
    /* Current Calling Class */
    zend_class_entry *called_scope = zend_get_called_scope(EG(current_execute_data));
    zend_string *class_name = zend_string_tolower(called_scope->name);
    char *location = strrchr(ZSTR_VAL(class_name), '\\');
    if ( location ) {
        RETURN_STRING(ZSTR_VAL(class_name) + (location - ZSTR_VAL(class_name) + 1));
    }
    RETURN_STR_COPY(zend_string_tolower(called_scope->name));
}/*}}}*/

CSPEED_METHOD(Model, one)/*{{{ proto Model::one()*/
{
    set_model_ultimate_pdo(getThis());
    set_columns_to_model(getThis());

    zval *table_name = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), 1, NULL);
    zval *select     = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_SELECT),     1, NULL);
    zval *where      = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 1, NULL);
    zval *group_by   = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_GROUP_BY),   1, NULL);
    zval *order_by   = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ORDER_BY),   1, NULL);
    zval *as_array   = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_AS_ARRAY),   1, NULL);

    zend_string *raw_sql = strpprintf(0, "SELECT %s FROM %s%s%s%s LIMIT 1", Z_STRVAL_P(select), 
        Z_STRVAL_P(table_name), Z_STRVAL_P(where),
        Z_STRVAL_P(group_by), Z_STRVAL_P(order_by));
    zval *pdo_object = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    zval pdo_statement; zval ret; zval retval;
    cspeed_pdo_prepare(pdo_object, ZSTR_VAL(raw_sql), &pdo_statement);
    cspeed_pdo_statement_set_fetch_mode(&pdo_statement, 2, &ret);
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval);

    /* Set the SQL to model for analysing */
    set_executed_sql(getThis(), raw_sql);

    if ( !output_sql_errors(&pdo_statement) ){
        zval result;
        cspeed_pdo_statement_fetch(&pdo_statement, &result);
        if ( Z_LVAL_P(as_array) == IS_TRUE ) {
            RETURN_ZVAL(&result, 1, NULL);
        } else if ( Z_LVAL_P(as_array) == IS_FALSE ) {
            zval_add_ref(&result);
            SEPARATE_ZVAL(&result);
            zend_update_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ARRAY_DATA), &result);
            add_object_property(getThis(), &result);
            RETURN_ZVAL(getThis(), 1, NULL);
        }
    }
    RETURN_NULL()
}/*}}}*/

CSPEED_METHOD(Model, select)    /*{{{ proto Model::select($fields)*/
{
    zval *fields;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &fields) == FAILURE) {
        return ;
    }
    if (fields && (Z_TYPE_P(fields) == IS_ARRAY)){
        /* Array fields */
        zval result;
        cspeed_build_quote_string(fields, &result); 
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_SELECT), Z_STRVAL(result));
    } else if (fields && (Z_TYPE_P(fields) == IS_STRING)){
        /* String */
        zend_update_property_string(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_SELECT), Z_STRVAL_P(fields));
    } else {
        php_error_docref(NULL, E_ERROR, "Parameter can only be array or string.");
    }
    zval_ptr_dtor(fields);
    RETURN_ZVAL(getThis(), 1, NULL);

}/*}}}*/

CSPEED_METHOD(Model, all)/*{{{ proto Model::all()*/
{
    set_model_ultimate_pdo(getThis());
    set_columns_to_model(getThis());

    zval *table_name = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), 1, NULL);
    zval *where      = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 1, NULL);
    zval *select     = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_SELECT), 1, NULL);
    zval *group_by   = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_GROUP_BY), 1, NULL);
    zval *order_by   = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ORDER_BY), 1, NULL);
    zval *as_array   = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_AS_ARRAY), 1, NULL);

    zval ret, retval, pdo_statement;
    zend_string *raw_sql = strpprintf(0, "SELECT %s FROM %s%s%s%s", Z_STRVAL_P(select) , 
        Z_STRVAL_P(table_name), Z_STRVAL_P(where), Z_STRVAL_P(group_by), Z_STRVAL_P(order_by));
    zval *pdo_object = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    cspeed_pdo_prepare(pdo_object, ZSTR_VAL(raw_sql), &pdo_statement);
    cspeed_pdo_statement_set_fetch_mode(&pdo_statement, 2, &ret);
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval);
    
    /* if using the all operation on the new model, reset the model to IS_FALSE do the update operation */
    zend_update_property_long(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_NEW_RECORD), IS_FALSE);

    /* Set the SQL to model for analysing */
    set_executed_sql(getThis(), raw_sql);

    if (!output_sql_errors(&pdo_statement)){
        zval result, ultimate_result;
        cspeed_pdo_statement_fetch_all(&pdo_statement, &result);
        if ( as_array && ( Z_LVAL_P(as_array) == IS_TRUE ) ) {
            RETURN_ZVAL(&result, 1, NULL);
        } else if (as_array && ( Z_LVAL_P(as_array) == IS_FALSE )) {
            add_multi_object_property(getThis(), &result, &ultimate_result);
            RETURN_ZVAL(&ultimate_result, 1, NULL);
        }
    }
    RETURN_NULL()
}/*}}}*/

CSPEED_METHOD(Model, save)/*{{{ proto Model::save()*/
{
    set_model_ultimate_pdo(getThis());
    set_columns_to_model(getThis());

    /* NULL insert, otherwise update*/
    zval *new_record = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_NEW_RECORD), 1, NULL);
    zval *where      = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 1, NULL);
    zval *table_name = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), 1, NULL);

    zend_string *execute_sql;
    if (Z_LVAL_P(new_record) == IS_TRUE) {
        /* INSERT INTO */
        execute_sql = strpprintf(0, "INSERT INTO %s %s",          /* INSERT INTO xx (id, t, r) VALUES(33, 8, 99) */
            Z_STRVAL_P(table_name),
            build_insert_field_datas(getThis())
        );

    } else {
        /* UPDATE */
        execute_sql = strpprintf(0, "UPDATE %s SET %s%s", /* UPDATE xx SET a=3,b=3,d=2 */
            Z_STRVAL_P(table_name),
            build_update_sql(getThis()),
            Z_STRVAL_P(where)
        );
    }

    zval retval, pdo_statement;
    trigger_events(getThis(), strpprintf(0, "%s", EVENT_BEFORE_SAVE));
    zval *pdo_object = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    cspeed_pdo_prepare(pdo_object, ZSTR_VAL(execute_sql), &pdo_statement);
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval);

    /* Set the SQL to model for analysing */
    set_executed_sql(getThis(), execute_sql);

    trigger_events(getThis(), strpprintf(0, "%s", EVENT_AFTER_SAVE));
    reset_model_sql(getThis());
    if (!output_sql_errors(&pdo_statement)){
        zval row_count;
        cspeed_pdo_statement_row_count(&pdo_statement, &row_count);
        RETURN_ZVAL(&row_count, 1, NULL);
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(Model, delete)/*{{{ proto Model::delete()*/
{
    set_model_ultimate_pdo(getThis());
    set_columns_to_model(getThis());
    /* If the magic datas exsits, do the concate-process first */
    build_delete_sql(getThis());
    /* After the concate process do the real step */
    zval *where      = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_WHERE_COND), 1, NULL);
    zval *table_name = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), 1, NULL);
    zend_string *execute_sql = strpprintf(0, "DELETE FROM %s%s", Z_STRVAL_P(table_name), Z_STRVAL_P(where));
    zval *pdo_object = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    if (ZVAL_IS_NULL(pdo_object)) {
        php_error_docref(NULL, E_ERROR, "Invalid PDO instance.");
    }
    zval pdo_statement;
    trigger_events(getThis(), strpprintf(0, "%s", EVENT_BEFORE_DELETE));
    cspeed_pdo_prepare(pdo_object, ZSTR_VAL(execute_sql), &pdo_statement);
    zval retval;
    cspeed_pdo_statement_execute(&pdo_statement, NULL, &retval);

    /* Set the SQL to model for analysing */
    set_executed_sql(getThis(), execute_sql);

    trigger_events(getThis(), strpprintf(0, "%s", EVENT_AFTER_DELETE));
    reset_model_sql(getThis());
    if (!output_sql_errors(&pdo_statement)){
        zval row_count;
        cspeed_pdo_statement_row_count(&pdo_statement, &row_count);
        RETURN_ZVAL(&row_count, 1, NULL);
    }
    RETURN_FALSE
}/*}}}*/

CSPEED_METHOD(Model, setDb)/*{{{ proto Model::setDb('db')*/
{
    zend_string *db_name;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &db_name) == FAILURE) {
        return ;
    }
    if ( CSPEED_G(di_object) ){
        zval di_object, *db_adapter;
        ZVAL_OBJ(&di_object, CSPEED_G(di_object));
        zval *objects = zend_read_property(cspeed_di_ce, &di_object, CSPEED_STRL(CSPEED_DI_OBJECT), 1, NULL);
        if (EXPECTED( (db_adapter = zend_hash_find(Z_ARRVAL_P(objects), db_name) ) != NULL )) 
        {
            /* Set the adapter to the property */
            zend_update_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ADAPTER), db_adapter);
            /* The db adapter exsits. set the db */
            zval *adapter_pdo_object = zend_read_property(cspeed_adapter_ce, db_adapter, CSPEED_STRL(CSPEED_DB_THIS_PDO), 1, NULL);
            zval_add_ref(adapter_pdo_object);
            zend_update_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ULTIMATE_PDO), adapter_pdo_object);
            RETURN_TRUE
        }
        RETURN_FALSE
    } else {
        php_error_docref(NULL, E_ERROR, "Please set the Adapter object into the Di container first.");
    }
}/*}}}*/

CSPEED_METHOD(Model, getDb)/*{{{ Get the PDO object*/
{
    zval *is_set_ultimate_pdo = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ULTIMATE_PDO), 1, NULL);

    if ( !ZVAL_IS_NULL(is_set_ultimate_pdo) ) {
        RETURN_ZVAL(is_set_ultimate_pdo, 1, 0);
    }

    is_set_ultimate_pdo = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), 1, NULL);
    if ( ZVAL_IS_NULL(is_set_ultimate_pdo) ) {
        php_error_docref(NULL, E_ERROR, "Sorry, you must set Adapter to the \\Cs\\di\\Di object");
    }
    RETURN_ZVAL(is_set_ultimate_pdo, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, getAdapter)/*{{{ Get the Adapter object*/
{
    zval *adapter_object = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_ADAPTER), 1, NULL);
    if ( !ZVAL_IS_NULL(adapter_object) ){
        RETURN_ZVAL(adapter_object, 1, 0);
    }
    adapter_object = zend_read_static_property(cspeed_adapter_ce, CSPEED_STRL(CSPEED_DB_THIS_ADAPTER), 1);
    if ( ZVAL_IS_NULL(adapter_object) ) {
        php_error_docref(NULL, E_ERROR, "Sorry, you must set Adapter to the \\Cs\\di\\Di object.");
    }
    RETURN_ZVAL(adapter_object, 1, 0);
}/*}}}*/

CSPEED_METHOD(Model, getExecutedSql)/*{{{ Get the last executed SQL*/
{
    zval *executed_sql = zend_read_property(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_EXECUTED_SQL), 1, NULL);
    RETURN_ZVAL(executed_sql, 1, NULL);
}/*}}}*/

CSPEED_METHOD(Model, asArray)/*Set the ResultSet are Array*/
{
    zend_update_property_long(cspeed_model_ce, getThis(), CSPEED_STRL(CSPEED_MODEL_AS_ARRAY), IS_TRUE);
    RETURN_ZVAL(getThis(), 1, NULL);
}/*}}}*/

/*{{{ All functions definitions */
static const zend_function_entry cspeed_model_functions[] = {
    CSPEED_ME(Model, __construct, arginfo_cspeed_model_construct, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, find, arginfo_cspeed_model_find, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
    CSPEED_ME(Model, select, arginfo_cspeed_model_select, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, tableName, arginfo_cspeed_model_table_name, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, where, arginfo_cspeed_model_where, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, andWhere, arginfo_cspeed_model_and_where, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, orderBy, arginfo_cspeed_model_order_by, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, groupBy, arginfo_cspeed_model_group_by, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, one, arginfo_cspeed_model_one, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, all, arginfo_cspeed_model_all, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, save, arginfo_cspeed_model_save, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, delete, arginfo_cspeed_model_delete, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, setDb, arginfo_cspeed_model_set_db, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, getDb, arginfo_cspeed_model_get_db, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, getAdapter, arginfo_cspeed_model_get_adapter, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, getExecutedSql, arginfo_cspeed_model_get_executed_sql, ZEND_ACC_PUBLIC)
    CSPEED_ME(Model, asArray, arginfo_cspeed_model_as_array, ZEND_ACC_PUBLIC)

    PHP_FE_END
};/*}}}*/


CSPEED_INIT(model)  /*{{{ Load the module function*/
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs\\mvc", "Model", cspeed_model_functions);
    cspeed_model_ce = zend_register_internal_class(&ce);

    zend_do_inheritance(cspeed_model_ce, cspeed_component_ce);

    /* Some internal properties */
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_MAGIC_DATAS), ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_PDO_OBJECT), ZEND_ACC_PRIVATE);
    zend_declare_property_long(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_NEW_RECORD), IS_TRUE, ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_ULTIMATE_PDO), ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_RESULT), ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_COLUMNS), ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_ADAPTER), ZEND_ACC_PRIVATE);
    zend_declare_property_null(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_ARRAY_DATA), ZEND_ACC_PRIVATE);
    zend_declare_property_bool(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_AS_ARRAY), IS_FALSE, ZEND_ACC_PRIVATE);

    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_PRIMARY), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_WHERE_COND), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_ORDER_BY), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_SELECT), "*", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_GROUP_BY), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_TABLE_NAME), "", ZEND_ACC_PRIVATE);
    zend_declare_property_string(cspeed_model_ce, CSPEED_STRL(CSPEED_MODEL_EXECUTED_SQL), "", ZEND_ACC_PRIVATE);

    zend_declare_class_constant_string(cspeed_model_ce, CSPEED_STRL(EVENT_BEFORE_SAVE), EVENT_BEFORE_SAVE);
    zend_declare_class_constant_string(cspeed_model_ce, CSPEED_STRL(EVENT_AFTER_SAVE), EVENT_AFTER_SAVE);
    zend_declare_class_constant_string(cspeed_model_ce, CSPEED_STRL(EVENT_BEFORE_DELETE), EVENT_BEFORE_DELETE);
    zend_declare_class_constant_string(cspeed_model_ce, CSPEED_STRL(EVENT_AFTER_DELETE), EVENT_AFTER_DELETE);
}/*}}}*/


















/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */



