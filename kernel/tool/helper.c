/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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
  | Author:                                                              |
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
#include "ext/date/php_date.h"
#include "ext/standard/info.h"
#include "ext/standard/php_dir.h"
#include "ext/standard/php_string.h"
#include "ext/standard/basic_functions.h"

#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#include "zend.h"
#include "zend_smart_str.h"             /* Use the smart_str */
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>


#include "kernel/mvc/model.h"
#include "kernel/tool/helper.h"
#include "kernel/net/request.h"
#include "kernel/tool/require.h"

void
/* Exit the CSpeed kernel engine. */
cspeed_exit(char *exit_info)
{
    zval ret_val;
    char *exit_string = ZSTR_VAL(strpprintf(
        0,
        "%s('%s');",
        "exit",
        (!exit_info || CSPEED_STRING_NOT_EMPTY(exit_info)) ? ZSTR_VAL(
            php_addslashes(
                strpprintf(
                    0, 
                    "%s", 
                    exit_info
                ), 
                IS_TRUE
            )
        ) : ""
    ));
    zend_eval_string(exit_string, &ret_val, "exit");
    zval_ptr_dtor(&ret_val);
}

void
/* Use this function to instead the default php_error_docref */
cspeed_print_info(int type, const char *format, ...)
{
    if ( CSPEED_G(core_debug_mode) == 1 ) {
        /* DEBUG MODE */    
        va_list args;
        char *buffer;

        va_start(args, format);
        vspprintf(&buffer, 0, format, args);
        cspeed_exit(buffer);
        efree(buffer);
        va_end(args);

    } else {
        /* Not in debug mode */
        va_list args;

        va_start(args, format);
        php_verror(NULL, "", type, format, args);
        va_end(args);
    }
}

void
/* Get the CSpeed engine's ROUTER_INFO based on PATH-INFO or $GET['xxx'] first char is '/' */
cspeed_get_path_info(zval *ret_path_info)
{
    char *path_info = NULL;

    if ( zend_string_equals(
             CSPEED_G(core_path_info_mode), 
             strpprintf(
                0, 
                "%s", 
                "AUTO"
            )
         )  ||
         zend_string_equals(
             CSPEED_G(core_path_info_mode), 
             strpprintf(
                0, 
                "%s", 
                "PATH"
            )
         )    
    ) {
        /* AUTO mode will get router info from PATH-INFO */
        path_info = cspeed_request_server_str_key_val("PATH_INFO");
    }

    if ( zend_string_equals(
             CSPEED_G(core_path_info_mode), 
             strpprintf(
                0, 
                "%s", 
                "GET"
            )
         ) ||
         zend_string_equals(
             CSPEED_G(core_path_info_mode), 
             strpprintf(
                0, 
                "%s", 
                "AUTO"
            )
         ) 
    ) {
        if ( !path_info || 
            !CSPEED_STRING_NOT_EMPTY(path_info) 
        ) {
            path_info = cspeed_request_get_str_key_val(ZSTR_VAL(CSPEED_G(get_router_pattern)));
        }
    }

    /**
     * In the PATH info, you must remember that the end of the string may be slash
     * if slashes exists at the end, trim it.
     */
    if ( CSPEED_STRING_NOT_EMPTY(path_info) &&
        (
            (*(path_info + strlen(path_info) - 1) == '/') ||
            (*path_info == '/')
        )
    ) {
        path_info = ZSTR_VAL(php_trim(strpprintf(
            0,
            "%s",
            path_info
        ), CSPEED_STRL("/"), 3));
    }

    /* Return the value to the parent-calling function. */
    ZVAL_STRING(ret_path_info, path_info);
}

char *cspeed_get_cwd(char *path)                    /*{{{ Return the current directory */
{
#if 0
    char path[MAXPATHLEN];
#endif
    char *ret=NULL;

    #if HAVE_GETCWD
        ret = VCWD_GETCWD(path, MAXPATHLEN);
    #elif HAVE_GETWD
        ret = VCWD_GETWD(path);
    #endif
    if (ret){
        return path;
    } else {
        return ".";
    }
}/*}}}*/

/*{{{ proto Return the result of the reservsed slash char* */
char *cspeed_reverse_slash_char(char *src)
{
    int i;
    for (i = 0; src[i] != '\0'; i++ ){
        if ( src[i] == '\\' ) {
            src[i] = '/';
        }
    }
    return src;
}/*}}}*/

char *title_upper_string(char *src) /*{{{ Upper case the first char */
{
    *src = toupper(src[0]);
    return src;
}/*}}}*/

char *title_lower_string(char *src) /*{{{ Upper case the first char */
{
    *src = tolower(src[0]);
    return src;
}/*}}}*/

/**
 * @brief substr
 * @param str       The string which need to be cut
 * @param start     The start pos which the sub-string begin
 * @param length    The sub-string's length
 * @return string   The result string which you need
 */
char *substr(char *str, int start, int length)
{
    int str_len = strlen(str);
    int result_len;
    if (length == -1) result_len = str_len - start;
    else result_len = length + 1;

    char *result = (char *)malloc(sizeof(char) * result_len);
    memset(result, 0, result_len);
    if (start >= 0) memcpy(result, str + start, result_len - 1);
    else memcpy(result, str + (str_len - abs(start)), result_len - 1);

    return result;
}

/**
 * @brief strstr
 * @param str
 * @param pattern
 * @return
 */
int stringstr(char *str, char *pattern)
{
    size_t start = 0;
    for (; start < strlen(str); start++) {
        if ( *(str + start) == *pattern ) {
            size_t j = 0;
            size_t pattern_len = strlen(pattern);
            for (; j <= pattern_len; j++) {
                if (j == pattern_len ) return start;
                if ( *(str + start + j) == *(pattern + j) ) continue;
                else return FALSE;
            }
        }
    }
    return FALSE;
}


/**
 * Replace the tablename with query_table_name or the raw table_name
 * return the value to the three parameter named dest.
 * so that the `dest` value must be writable.
 */
void replace_fake_name(char *str, char *find_str, char *fake_name, char *dest)
{
    /**
     * if exists the table name,replace it with fake_name
     *  max(supjos.id) as d
     */
    int pos_dot = stringexists(str, ".");
    int pos_lp  = stringexists(str, "(");
    
    /* if exists the query_table_name or not. */
    char temp_str[256] = {0};
    strncpy(temp_str, str, pos_dot == -1 ? strlen(str) : pos_dot);
    int pos = stringexists(temp_str, find_str);
    
    if ( pos != -1 ) {
        /* find the query_table_name */
        if ( pos_dot == -1 ){
            /* Dot[.] not found */
            sprintf(dest, "%s", fake_name);
        } else {
            /* found the dot[.] */
            if (pos_lp != -1) {
                char temp_lp[256] = {0};
                strncpy(temp_lp, str, pos_lp + 1);
                sprintf(dest, "%s%s.%s", temp_lp, fake_name, str + pos_dot + 1);
            } else {
                sprintf(dest, "%s.%s", fake_name, str + pos_dot + 1);
            }
        }
    } else {
        /* Not found the query_table_name */
        sprintf(dest, "%s", str);
    }
}

/**
 * Caculator the string's hash value
 * according to the expression:
 *  crc_value = crc_value * 31 + char
 */
unsigned long long int string_crc(char *str, size_t str_len)
{
    if (str_len < 1)
    {
        return 0;
    }

    size_t start = 0;
    unsigned long long int crc_value = 0;

    for(; start < str_len; start++)
    {
        crc_value += crc_value * 31 + *(str + start);
    }
    return crc_value;
}

/**
 * To replace all name into the need string.
 *  You must known that, the `dest` must be writeable !!!
 */
void replace_all_name(char *src, char *subject, char *replace_subject, char *dest, int *length, int space_first)
{
    int space = 0;
    char *temp_src = strdup(src);
    char *temp_result;

    if (space_first){
        temp_result = strtok(src, " ");
        if (strncmp(src, temp_src, strlen(temp_src)) == 0)
        {
            temp_result = strtok(src, ",");
            space = 0;
        } else {
            space = 1;
        }
    } else {
        temp_result = strtok(src, ",");
        if (strncmp(src, temp_src, strlen(temp_src)) == 0)
        {
            temp_result = strtok(src, " ");
            space = 1;
        }
    }
    if (temp_result == NULL)
    {
        replace_fake_name(src, subject, replace_subject, dest);
    }
    
    char return_string[125] = {0};
    while(temp_result != NULL)
    {
        replace_fake_name(temp_result, subject, replace_subject, return_string);
        strncat(dest, return_string, strlen(return_string));
        *length += strlen(return_string);
        if (space) {
            temp_result = strtok(NULL, " ");
        } else {
            temp_result = strtok(NULL, ",");
        }
        if (temp_result != NULL) {
            if (space) {
                strncat(dest, " ", strlen(" "));
            } else {
                strncat(dest, ",", strlen(","));
            }
            *length += 1;
        } 
    }
}

/* {{{ php_simple_ini_parser_cb */
void php_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
    switch (callback_type) {

        case ZEND_INI_PARSER_ENTRY:
            if (!arg2) {
                /* bare string - nothing to do */
                break;
            }
            Z_TRY_ADDREF_P(arg2);
            zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), arg2);
            break;

        case ZEND_INI_PARSER_POP_ENTRY:
        {
            zval hash, *find_hash;

            if (!arg2) {
                /* bare string - nothing to do */
                break;
            }

            if (!(Z_STRLEN_P(arg1) > 1 && Z_STRVAL_P(arg1)[0] == '0') && 
                is_numeric_string(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), NULL, NULL, 0) == IS_LONG) {
                zend_ulong key = (zend_ulong) zend_atol(Z_STRVAL_P(arg1), (int)Z_STRLEN_P(arg1));
                if ((find_hash = zend_hash_index_find(Z_ARRVAL_P(arr), key)) == NULL) {
                    array_init(&hash);
                    find_hash = zend_hash_index_update(Z_ARRVAL_P(arr), key, &hash);
                }
            } else {
                if ((find_hash = zend_hash_find(Z_ARRVAL_P(arr), Z_STR_P(arg1))) == NULL) {
                    array_init(&hash);
                    find_hash = zend_hash_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), &hash);
                }
            }

            if (Z_TYPE_P(find_hash) != IS_ARRAY) {
                zval_dtor(find_hash);
                array_init(find_hash);
            }

            if (!arg3 || (Z_TYPE_P(arg3) == IS_STRING && Z_STRLEN_P(arg3) == 0)) {
                Z_TRY_ADDREF_P(arg2);
                add_next_index_zval(find_hash, arg2);
            } else {
                array_set_zval_key(Z_ARRVAL_P(find_hash), arg3, arg2);
            }
        }
        break;

        case ZEND_INI_PARSER_SECTION:
            break;
    }
}/* }}} */

/* {{{ php_ini_parser_cb_with_sections */
void php_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)
{
    if (callback_type == ZEND_INI_PARSER_SECTION) {
        array_init(&BG(active_ini_file_section));
        zend_symtable_update(Z_ARRVAL_P(arr), Z_STR_P(arg1), &BG(active_ini_file_section));
    } else if (arg2) {
        zval *active_arr;

        if (Z_TYPE(BG(active_ini_file_section)) != IS_UNDEF) {
            active_arr = &BG(active_ini_file_section);
        } else {
            active_arr = arr;
        }

        php_simple_ini_parser_cb(arg1, arg2, arg3, callback_type, active_arr);
    }
}/* }}} */

/*{{{ parse the INI file, with the given key and node */
void cspeed_parse_ini_file(char *file_name, char *node_name, char *node_key, zend_bool parse_section, zval *retval)
{
    zend_file_handle fh;
    zend_ini_parser_cb_t ini_parser_cb;

    /* Set callback function */
    if (parse_section) {
        ZVAL_UNDEF(&BG(active_ini_file_section));
        ini_parser_cb = (zend_ini_parser_cb_t) php_ini_parser_cb_with_sections;
    } else {
        ini_parser_cb = (zend_ini_parser_cb_t) php_simple_ini_parser_cb;
    }

    /* Setup filehandle */
    memset(&fh, 0, sizeof(fh));
    fh.filename = file_name;
    fh.type = ZEND_HANDLE_FILENAME;

    zval ini_array_value;
    array_init(&ini_array_value);
    if (zend_parse_ini_file(&fh, 0, (int)ZEND_INI_SCANNER_NORMAL, ini_parser_cb, &ini_array_value) == FAILURE) {
        zval_dtor(&ini_array_value);
        ZVAL_NULL(&ini_array_value);
    } else {
        /* To find the key and the node value from the data */
        if (node_name != NULL) {
            zval *node_value = zend_hash_find(Z_ARRVAL_P(&ini_array_value), zend_string_init(CSPEED_STRL(node_name), 0));
            if (node_value == NULL) {
                ZVAL_NULL(retval);
            } else {
                if (node_key == NULL) {
                    Z_TRY_ADDREF_P(node_value);
                    ZVAL_COPY_VALUE(retval, node_value);
                } else {
                    zval *key_value = zend_hash_find(Z_ARRVAL_P(node_value), zend_string_init(CSPEED_STRL(node_key), 0));
                    if (key_value == NULL) {
                        ZVAL_NULL(retval);
                    } else {
                        Z_TRY_ADDREF_P(key_value);
                        ZVAL_COPY_VALUE(retval, key_value);
                    }
                }
            }
        } else if ( ( node_name == NULL ) && ( node_key != NULL ) ) {
            zval *key_value = zend_hash_find(Z_ARRVAL_P(&ini_array_value), zend_string_init(CSPEED_STRL(node_key), 0));
            if (key_value == NULL) {
                ZVAL_NULL(retval);
            } else {
                Z_TRY_ADDREF_P(key_value);
                ZVAL_COPY_VALUE(retval, key_value);
            }
        } else {
            Z_TRY_ADDREF_P(&ini_array_value);
            ZVAL_COPY_VALUE(retval, &ini_array_value);
        }
    }
}

void 
cspeed_build_equal_string(zval *array, char *begin_str, zval *result)/*{{{ Building the WHERE|HAVING strings */
{
    zval *var_value;
    zend_string *val_key;
    smart_str where_str = {0};
    smart_str_appends(&where_str, begin_str);
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array), val_key, var_value) {
        if (isalpha(*(ZSTR_VAL(val_key)))) {
            // if array
            if ( Z_TYPE_P(var_value) == IS_ARRAY ) {
                zend_string *relation_str; zval *relation_value;
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(var_value), relation_str, relation_value) {
                    if ( Z_TYPE_P(relation_value) == IS_OBJECT ) convert_to_string(relation_value);
                    if ( Z_TYPE_P(relation_value) == IS_STRING ) {
                        smart_str_appendc(&where_str, '`');
                        smart_str_appends(&where_str, ZSTR_VAL(val_key));
                        smart_str_appendc(&where_str, '`');
                        // Join the relation-character
                        smart_str_appends(&where_str, ZSTR_VAL(relation_str));
                        smart_str_appends(
                            &where_str, 
                            ZSTR_VAL(
                                strpprintf(
                                    0, 
                                    " '%s",
                                    Z_STRVAL_P(relation_value)
                                )
                            )
                        );
                        smart_str_appends(&where_str, "' AND ");
                    }
                } ZEND_HASH_FOREACH_END();
            } else {
            // if value
                smart_str_appendc(&where_str, '`');
                smart_str_appends(&where_str, ZSTR_VAL(val_key));
                smart_str_appends(&where_str, "`='");
                if ( Z_TYPE_P(var_value) == IS_OBJECT ) convert_to_string(var_value);
                if (Z_TYPE_P(var_value) == IS_LONG){
                    smart_str_appends(
                        &where_str, 
                        ZSTR_VAL(
                            strpprintf(
                                0, 
                                "%d", 
                                Z_LVAL_P(var_value)
                            )
                        )
                    );
                } else if ( Z_TYPE_P(var_value) == IS_DOUBLE ) {
                    smart_str_appends(
                        &where_str, 
                        ZSTR_VAL(
                            strpprintf(
                                0, 
                                "%f", 
                                Z_DVAL_P(var_value)
                            )
                        )
                    );
                } else if ( Z_TYPE_P(var_value) == IS_STRING ){
                    smart_str_appends(
                        &where_str, 
                        Z_STRVAL_P(var_value)
                    );
                }
                smart_str_appends(&where_str, "' AND ");
            }
        }
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&where_str);
    char *temp_where_str = (char *)malloc(sizeof(char) * ZSTR_LEN(where_str.s) - 4);
    memset(
        temp_where_str, 
        0, 
        ZSTR_LEN(where_str.s) - 4
    );
    memcpy(
        temp_where_str, 
        ZSTR_VAL(where_str.s), 
        ZSTR_LEN(where_str.s) - 5
    );
    smart_str_free(&where_str);
    ZVAL_STRING(result, temp_where_str);
    free(temp_where_str);
}/*}}}*/

void 
cspeed_build_quote_string(zval *array, zval *result)/*{{{ Building the Quote string */
{
    zval *value;
    smart_str field_str = {0};
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(array), value) {
        smart_str_appendc(&field_str, '`');
        if ( Z_TYPE_P(value) == IS_OBJECT ) convert_to_string(value);
        if ( (Z_TYPE_P(value) == IS_STRING) && isalpha(*(Z_STRVAL_P(value))) ){
            smart_str_appends(&field_str, Z_STRVAL_P(value));
        }
        smart_str_appends(&field_str, "`,");
    } ZEND_HASH_FOREACH_END();
    smart_str_0(&field_str);
    char *temp_select_str = (char *)malloc(sizeof(char) * ZSTR_LEN(field_str.s));
    memset(
        temp_select_str, 
        0, 
        ZSTR_LEN(field_str.s)
    );
    memcpy(
        temp_select_str, 
        ZSTR_VAL(field_str.s), 
        ZSTR_LEN(field_str.s) - 1
    );
    smart_str_free(&field_str);
    ZVAL_STRING(result, temp_select_str);
    free(temp_select_str);
}/*}}}*/

int 
check_file_exists(char *file_path) /* Checking wheather the given file is exists or not. */
{
    if (access(file_path, F_OK) == -1) {
        cspeed_print_info(
            E_ERROR, 
            "File: `%s` not exists.", 
            file_path
        );
    }
    return TRUE;
}

void 
/*{{{ Parent class's method to current ce */
recursive_call_parent_method(zend_class_entry *ce, char *method_name)
{
    if (ce) {
        recursive_call_parent_method(ce->parent, method_name);
        zval parent_obj;
        object_init_ex(&parent_obj, ce);
        if (CSPEED_METHOD_IN_OBJECT(&parent_obj, method_name)){
            zval function_name, retval;
            ZVAL_STRING(&function_name, method_name);
            call_user_function(
                NULL, 
                &parent_obj, 
                &function_name, 
                &retval, 
                0, 
                NULL
            );
            zval_ptr_dtor(&function_name);
            zval_ptr_dtor(&retval);
        }
        zval_ptr_dtor(&parent_obj);
        ce = ce->parent;
    }
}/*}}}*/

void 
/*{{{  Parent class's method to current ce */
recursive_call_parent_method_two(zval *obj, char *method_name)
{
    if (obj) {
        if (Z_OBJCE_P(obj)->parent){
            zval parent_obj;
            object_init_ex(&parent_obj, Z_OBJCE_P(obj)->parent);
            recursive_call_parent_method_two(&parent_obj, method_name);
        }        
        if (CSPEED_METHOD_IN_OBJECT(obj, method_name)){
            zval function_name, retval;
            ZVAL_STRING(&function_name, method_name);
            zval params[] = { *obj };
            call_user_function(
                NULL, 
                obj, 
                &function_name, 
                &retval, 
                1, 
                params
            );
            zval_ptr_dtor(&function_name);
            zval_ptr_dtor(&retval);
        }
    }
}/*}}}*/

void
/* Use this when the given parameters was c array */
call_method_with_object(zval *object, char *method_name, uint32_t param_counts, zval params[], zval *ret_val)
{
    if (CSPEED_METHOD_IN_OBJECT(object, method_name)){
        zval function_name;
        ZVAL_STRING(&function_name, method_name);
        call_user_function(
            EG(function_table), 
            object, 
            &function_name, 
            ret_val, 
            param_counts, 
            params
        );
        zval_ptr_dtor(&function_name);
    }
}

void
/* Use this function instead of above, when parameters was an php array */
call_method_with_object_params(
    zval *object, 
    char *method_name, 
    zval *parameters, 
    zval *ret_val
){
    if ( Z_TYPE_P(parameters) != IS_ARRAY ) {
        return ;
    }
    uint32_t params_count = zend_hash_num_elements(Z_ARRVAL_P(parameters));
    if ( !params_count ) return ;
    zval *params = (zval *)malloc(sizeof(zval) * params_count);
    uint32_t n = 0;
    zval *val_para;
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(parameters), val_para) {
        ZVAL_COPY(&params[n], val_para);
        n++;
    } ZEND_HASH_FOREACH_END();
    call_method_with_object(
        object, 
        method_name, 
        params_count, 
        params, 
        ret_val
    );
    free(params);
}

int 
 /*{{{ Load file */
cspeed_autoload_file( zend_string *class_name_with_namespace, zval *obj, char *alias){    
    
    if ( *(ZSTR_VAL(class_name_with_namespace)) == '\\' ) {

        class_name_with_namespace = strpprintf(
            0, 
            "%s", 
            ZSTR_VAL(class_name_with_namespace) + 1
        );
    }

    if (zend_hash_find_ptr(
        EG(class_table), 
        zend_string_tolower(class_name_with_namespace)
    ) != NULL) {
        return TRUE;
    }

    char *slash_pos = strchr(ZSTR_VAL(class_name_with_namespace), '\\');

    if (slash_pos == NULL) { /* No slash find */

        zend_string *real_file_path = strpprintf(
            0, 
            "./%s.php", 
            ZSTR_VAL(class_name_with_namespace)
        );

        check_file_exists(ZSTR_VAL(real_file_path));

        if (cspeed_require_file(ZSTR_VAL(real_file_path), NULL, NULL, NULL) == FALSE){
            return FALSE;
        }

        zend_string_release(real_file_path);
    } else {                 
        /* find the slash */
        char *current_alias = (char *)malloc(sizeof(char) * (slash_pos - ZSTR_VAL(class_name_with_namespace) + 1));
        
        memset(
            current_alias, 
            0, 
            (slash_pos - ZSTR_VAL(class_name_with_namespace) + 1)
        );
        
        memcpy(
            current_alias, 
            ZSTR_VAL(class_name_with_namespace), 
            (slash_pos - ZSTR_VAL(class_name_with_namespace))
        );

        /** 
         * Kernel contains the `app` namespace alias to do the default kernel loading
         * Developers can define other alias to do the freestyle autoloading job.
         */
        zval *all_app_aliases = zend_read_property(
            Z_OBJCE_P(obj), 
            obj, 
            CSPEED_STRL(alias), 
            1, 
            NULL
        );

        zval *has_exists = zend_hash_find(
            Z_ARRVAL_P(all_app_aliases), 
            zend_string_init(
                CSPEED_STRL(current_alias),
                0
            )
        );

        if ( strncmp(current_alias, ("Cs"), strlen("Cs")) == 0 ) {
            free(current_alias);
            cspeed_print_info(
                E_ERROR, 
                "CSpeed framework not contain the class: `%s`.", 
                ZSTR_VAL(class_name_with_namespace)
            );
        }

        if ( has_exists ) {   /* Exists the need alias */
            /* five means the .php & space*/
            int real_file_path_size = Z_STRLEN_P(has_exists) + ZSTR_LEN(class_name_with_namespace)
                                    - (slash_pos - ZSTR_VAL(class_name_with_namespace))
                                    + 5;

            /* Assign the memory and set it to zero. */
            char *real_file_path = (char *)malloc(sizeof(char) * real_file_path_size); 
            memset(real_file_path, 0, real_file_path_size);

            strncat(
                real_file_path, 
                Z_STRVAL_P(has_exists), 
                Z_STRLEN_P(has_exists)
            );
            strncat(
                real_file_path, 
                ZSTR_VAL(class_name_with_namespace) + (slash_pos - ZSTR_VAL(class_name_with_namespace)),
                ZSTR_LEN(class_name_with_namespace) - (slash_pos - ZSTR_VAL(class_name_with_namespace))
            );
            
            /* Append the .php suffix */
            strncat(real_file_path, ".php", strlen(".php"));
            
            /* Reslash all slash to reslash */
            cspeed_reverse_slash_char(real_file_path);
            
            /* check whether the file is exists or not */
            check_file_exists(real_file_path);
            
            /* After checking, require the file */
            if (cspeed_require_file(real_file_path, NULL, NULL, NULL) == FALSE){
                free(current_alias);
                free(real_file_path);
                return FALSE;
            }
            free(current_alias);
            free(real_file_path);
        } else {            /* Not found the needing alias */
            free(current_alias);
            cspeed_print_info(
                E_ERROR, 
                "Namespace alias: %s not found. please set it first before use.", 
                current_alias
            );
        }
    }
    return TRUE;
} /*}}}*/



void
/*{{{ Load the kernel default setting */
load_kernel_setting(zend_string *ini_config_file, zend_string *ini_config_node_name, char *path)
{
    if ( ini_config_file && CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(ini_config_file))) {

        /* All the ini file configs.  */
        zval configs;
        zend_string *ini_real_file;

        if ( *(ZSTR_VAL(ini_config_file)) == '/' ) {
            ini_real_file = ini_config_file;
        } else {
            ini_real_file = strpprintf(
                0, 
                "%s/%s", 
                path, 
                ZSTR_VAL(ini_config_file)
            );
        }
        check_file_exists(ZSTR_VAL(ini_real_file));
        
        zend_string *node_core_name = strpprintf(0, 
            "%s%s%s", 
            ini_config_node_name ? ZSTR_VAL(ini_config_node_name) : "",
            ini_config_node_name ? ":" : "", 
            "core" 
        );
        zend_string *node_db_name = strpprintf(0, 
            "%s%s%s", 
            ini_config_node_name ? ZSTR_VAL(ini_config_node_name) : "",
            ini_config_node_name ? ":" : "",
            "db"
        );
        /* Parsing the INI file, and put the config value into the configs */
        cspeed_parse_ini_file(
            ZSTR_VAL(ini_real_file), 
            NULL, 
            NULL, 
            1, 
            &configs
        );

        if (ZVAL_IS_NULL(&configs)  && !zend_hash_num_elements(Z_ARRVAL(configs)) ) {
            cspeed_print_info(
                E_ERROR, 
                "Configs empty."
            );
        }
        if ( ( Z_TYPE(configs) == IS_ARRAY ) ) {
            zval *config_value, *core_configs;
            if ( EXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_core_name)) == NULL )){
                cspeed_print_info(
                    E_ERROR, 
                    "`%s` configs not found in config file.", 
                    ZSTR_VAL(node_core_name)
                );
            }
            /*core.debug.mode*/
            if ( EXPECTED((config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_DEBUG_MODE) ) ) != NULL )
            ) {/* Value must be false or true, otherwise print error info */
                if ( (strncmp(
                        Z_STRVAL_P(config_value),
                        ("1"),
                        strlen("1")
                      ) == 0
                    ) || (
                        !CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(config_value)) && (atoi(Z_STRVAL_P(config_value)) == 0)
                    ) 
                ) {
                    /* Value was correct. */
                    CSPEED_G(core_debug_mode) = (zend_bool)atoi(Z_STRVAL_P(config_value));
                    /* if debug mode on, the logs_path must be setting too */
                } else {
                    cspeed_print_info(
                        E_ERROR,
                        "Config `%s` must be %s. ", 
                        CORE_DEBUG_MODE, 
                        "`true` or `false` or `on` or `off`"
                    );
                }
            }
            /*core.path.info.mode*/
            if ( EXPECTED((config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs),
                CSPEED_STRL(CORE_PATH_INFO_MODE) ) ) != NULL) 
            ) {
                if ( (strncmp(
                        "AUTO",
                        (ZSTR_VAL(php_string_toupper(Z_STR_P(config_value)))),
                        strlen(ZSTR_VAL(php_string_toupper(Z_STR_P(config_value))))
                     ) == 0) || 
                     (strncmp(
                        "PATH",
                        (ZSTR_VAL(php_string_toupper(Z_STR_P(config_value)))),
                        strlen(ZSTR_VAL(php_string_toupper(Z_STR_P(config_value))))
                     ) == 0) || 
                     (strncmp(
                        "GET",
                        (ZSTR_VAL(php_string_toupper(Z_STR_P(config_value)))),
                        strlen(ZSTR_VAL(php_string_toupper(Z_STR_P(config_value))))
                    ) == 0)
                ) {
                    CSPEED_G(core_path_info_mode) = php_string_toupper(Z_STR_P(config_value));
                } else {
                    cspeed_print_info(
                        E_ERROR,
                        "Config `%s` must be %s.",
                        CORE_PATH_INFO_MODE,
                        "`PATH` or `AUTO` or `GET`"
                    );
                }
            }
            /*get_router_pattern*/
            if ( EXPECTED((config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_PATH_INFO_SYMBOL) ) ) != NULL )
            ) {
                if (CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(config_value))) {
                    CSPEED_G(get_router_pattern) = Z_STR_P(config_value);
                }
            }
            /*boot_class_name*/
            if ( EXPECTED((
                    config_value = zend_hash_str_find(
                        Z_ARRVAL_P(core_configs),
                        CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_CLASS_NAME)
                    )
                ) != NULL)
            ){
                if ( CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(config_value)) ) {
                    CSPEED_G(boot_class_name) = Z_STR_P(config_value);
                }
            }
            /*core.application*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_APPLICATION_NAME))) != NULL ) ) {
                CSPEED_G(core_application) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_APPLICATION_NAME
                );
            }
            /*core.bootstrap*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_BOOTSTRAP_NAME
                );
            }
            /*core.bootstrap*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_METHOD_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap_method_string) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_BOOTSTRAP_METHOD_NAME
                );
            }
            /* core.router.modules */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_MODULES_NAME) )) != NULL )) {
                zval modules;
                array_init(&modules);
                php_explode(
                    zend_string_init(CSPEED_STRL(","), 0), 
                    Z_STR_P(config_value), 
                    &modules, 
                    ZEND_LONG_MAX
                );
                zval_add_ref(&modules);
                CSPEED_G(core_router_modules) = Z_ARRVAL(modules);
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_MODULES_NAME
                );
            }
            /* core.router.default.module */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_MODULE_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_module) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_DEFAULT_MODULE_NAME
                );
            }
            /* core.router.default.controller */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_CONTROLLER_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_controller) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_DEFAULT_CONTROLLER_NAME
                );
            }
            /* core.router.default.action */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_ACTION_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_action) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_DEFAULT_ACTION_NAME
                );
            }
            /* core.view.ext */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_VIEW_EXT_NAME) )) != NULL )) {
                CSPEED_G(core_view_ext) = zend_string_copy(Z_STR_P(config_value));
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_VIEW_EXT_NAME
                );
            }
            /* core.view.auto_render */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_VIEW_AUTO_RENDER) )) != NULL )) {

                if ( (strncmp(
                        Z_STRVAL_P(config_value),
                        ("1"),
                        strlen("1")
                      ) == 0
                    ) || (
                        !CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(config_value)) && (atoi(Z_STRVAL_P(config_value)) == 0)
                    ) 
                ) {
                    CSPEED_G(core_view_auto_render) = (zend_bool)atoi(Z_STRVAL_P(config_value));
                } else {
                    cspeed_print_info(
                        E_ERROR,
                        "Config `%s` must be %s.",
                        CORE_CONFIG_VIEW_AUTO_RENDER,
                        "`on` or `off` or `true` or `false`"
                    );
                }                
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_VIEW_AUTO_RENDER
                );
            }
            /* core.url.pattern */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_URL_PATTERN) )) != NULL )) {
                CSPEED_G(core_url_pattern) = Z_STR_P(config_value);
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "Config :`%s` not set.", 
                    CORE_CONFIG_URL_PATTERN
                );
            }

            /* The DB config */
            if ( EXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_db_name )) == NULL )){
                cspeed_print_info(
                    E_ERROR, 
                    "`%s` configs not found in config file.", 
                    ZSTR_VAL(node_db_name)
                );
            }
            /* Below are some optional configs to the DB engine, if have, set it to override the setting */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_DSN) )) != NULL )) {
                CSPEED_G(db_master_dsn) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_USERNAME) )) != NULL )) {
                CSPEED_G(db_master_username) = zend_string_copy(Z_STR_P(config_value));
            }
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DB_PASSWORD) )) != NULL )) {
                CSPEED_G(db_master_password) = zend_string_copy(Z_STR_P(config_value));
            }
            zend_string_release(node_core_name);
            zend_string_release(node_db_name);
        } else {
            zend_string_release(node_core_name);
            zend_string_release(node_db_name);
            cspeed_print_info(
                E_ERROR, 
                "Configs data type wrong."
            );
        }
    }
}

void 
parameter_filter(zval *filter, zval *parameter)
{
    if ( filter ) {
        if ( Z_TYPE_P(filter) == IS_STRING ) {
            /* function exists call the function */
            zend_function *func= zend_hash_find_ptr(EG(function_table), Z_STR_P(filter));
            if (func && (func->type != ZEND_INTERNAL_FUNCTION ||
                    func->internal_function.handler != zif_display_disabled_function)) {
                zval ret_val;
                zval params[] = { *parameter };
                call_user_function(EG(function_table), NULL, filter, &ret_val, 1, params);
                Z_TRY_ADDREF_P(&ret_val);
                ZVAL_COPY_VALUE(parameter, &ret_val);
            } else {
                cspeed_print_info(
                    E_ERROR, 
                    "function: `%s` not exists.", 
                    Z_STRVAL_P(filter)
                );
            }
        } else if ( Z_TYPE_P(filter) == IS_OBJECT) {
            /* Check the object is callable or not. */
            zend_string *error_handler_name = NULL;
            if ( !zend_is_callable(filter, 0, &error_handler_name) ) {
                cspeed_print_info(
                    E_ERROR, 
                    "The argument must to be a valid callback."
                );
            }
            zval ret_val;
            zval params[] = { *parameter };
            call_user_function(
                EG(function_table), 
                NULL, 
                filter, 
                &ret_val, 
                1, 
                params
            );
            Z_TRY_ADDREF_P(&ret_val);
            ZVAL_COPY_VALUE(parameter, &ret_val);
        } else {
            cspeed_print_info(
                E_ERROR, 
                "The argument must be valid function name or callable."
            );
        }
    }
}

void 
add_object_property(zval *object, zval *properties) /*{{{ Add the properties to the object */
{
    if ( !ZVAL_IS_NULL(object) && ( Z_TYPE_P(properties) == IS_ARRAY ) 
        && zend_hash_num_elements( Z_ARRVAL_P(properties) )  ) {
        /* zend_merge_properties(object, Z_ARRVAL_P(properties)); */
        
        zend_string *val_key; zval *val_value;

        ZEND_HASH_FOREACH_STR_KEY_VAL( Z_ARRVAL_P(properties), val_key, val_value ) {
            zend_property_info *property_info = zend_hash_find_ptr(
                &(Z_OBJCE_P(object))->properties_info, 
                val_key
            );
            if ( property_info != NULL ) {
                if ( (property_info->flags & ZEND_ACC_SHADOW) 
                    || (property_info->flags & ZEND_ACC_PUBLIC)
                    || (property_info->flags & ZEND_ACC_PROTECTED)
                    || (property_info->flags & ZEND_ACC_PRIVATE)
                ) {
                    zend_update_property(
                        Z_OBJCE_P(object), 
                        object, 
                        CSPEED_STRL(ZSTR_VAL(val_key)), 
                        val_value
                    );
                } 
            } else {
                if ( !Z_OBJ_P(object)->properties ) {
                    rebuild_object_properties( Z_OBJ_P(object) );
                }
                zend_hash_add_new(
                    Z_OBJ_P(object)->properties, 
                    (val_key), 
                    val_value
                );
            }

        } ZEND_HASH_FOREACH_END();
    }
} /*}}}*/

void
copy_object_properties(zval *dest_object, zval *src_object)
{
    zval *prop;
    zend_string *key;
    zend_property_info *property_info;

    ZEND_HASH_FOREACH_STR_KEY_VAL(&(Z_OBJCE_P(src_object))->properties_info, key, prop) {
        if ( Z_OBJCE_P(dest_object) != Z_OBJCE_P(src_object) ) {
            return ;
        }
        zval *value = zend_read_property(
            cspeed_model_ce, 
            src_object, 
            CSPEED_STRL(ZSTR_VAL(key)), 
            1, 
            NULL
        );
        if ( !ZVAL_IS_NULL(value) ) {
            zend_update_property(
                cspeed_model_ce, 
                dest_object, 
                CSPEED_STRL(ZSTR_VAL(key)), 
                value
            );
        }
    } ZEND_HASH_FOREACH_END();
}

void
add_multi_object_property(zval *object, zval *multi_properties, zval *ret_val) /*{{{ Add the properties to the object */
{
    if ( Z_TYPE_P(ret_val) != IS_ARRAY ) {
        array_init(ret_val);
    }

    if ( !ZVAL_IS_NULL(object) && ( Z_TYPE_P(multi_properties) == IS_ARRAY ) 
        && zend_hash_num_elements( Z_ARRVAL_P(multi_properties) )  ) {
        zval *val_value;

        ZEND_HASH_FOREACH_VAL( Z_ARRVAL_P(multi_properties), val_value ) {
            if ( (Z_TYPE_P(val_value) == IS_ARRAY) && zend_hash_num_elements(Z_ARRVAL_P(val_value)) ) {
                zval t_object;
#if 0
                /* Not to create_object use the zend function to do the copy job. */
                object_and_properties_init(
                    &t_object, 
                    Z_OBJCE_P(object), 
                    Z_OBJ_P(object)->properties
                );
                copy_object_properties(&t_object, object);
                zval_add_ref(val_value);
                SEPARATE_ZVAL(val_value);
#endif
                zend_object *dest_object = zend_objects_clone_obj(object);
                ZVAL_OBJ(&t_object, dest_object);
                zend_update_property(
                    cspeed_model_ce, 
                    &t_object, 
                    CSPEED_STRL(CSPEED_MODEL_ARRAY_DATA), 
                    val_value
                );
                add_object_property(&t_object, val_value);
                Z_TRY_ADDREF_P(&t_object);
                add_next_index_zval(ret_val, &t_object);
            }
        } ZEND_HASH_FOREACH_END();
    }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
