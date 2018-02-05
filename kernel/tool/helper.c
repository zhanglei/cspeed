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
#include "ext/standard/info.h"
#include "php_cspeed.h"
#include "kernel/tool/helper.h"
#include "ext/standard/php_dir.h"
#include "ext/standard/basic_functions.h"

#include "zend.h"
#include "zend_ini_scanner.h"
#include "zend_language_scanner.h"
#include <zend_language_parser.h>

#include "zend_smart_str.h"             /* Use the smart_str */

#include <unistd.h>
#include <fcntl.h>

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


char *cspeed_reverse_slash_char(char *src)/*{{{ proto Return the result of the reservsed slash char* */
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

void php_simple_ini_parser_cb(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr) /* {{{ php_simple_ini_parser_cb */
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

            if (!(Z_STRLEN_P(arg1) > 1 && Z_STRVAL_P(arg1)[0] == '0') && is_numeric_string(Z_STRVAL_P(arg1), Z_STRLEN_P(arg1), NULL, NULL, 0) == IS_LONG) {
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

void php_ini_parser_cb_with_sections(zval *arg1, zval *arg2, zval *arg3, int callback_type, zval *arr)/* {{{ php_ini_parser_cb_with_sections */
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

void cspeed_parse_ini_file(char *file_name, char *node_name, char *node_key, zend_bool parse_section, zval *retval)/*{{{ parse the INI file, with the given key and node */
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

void cspeed_build_equal_string(zval *array, char *begin_str, zval *result)/*{{{ Building the WHERE|HAVING strings */
{
    zend_string *val_key;
    zval *var_value;
    smart_str where_str = {0};
    smart_str_appends(&where_str, begin_str);
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(array), val_key, var_value) {
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
    smart_str_free(&where_str);
    ZVAL_STRING(result, temp_where_str);
    free(temp_where_str);
}/*}}}*/

void cspeed_build_quote_string(zval *array, zval *result)/*{{{ Building the Quote string */
{
    zval *value;
    smart_str field_str = {0};
    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(array), value) {
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
    smart_str_free(&field_str);
    ZVAL_STRING(result, temp_select_str);
    free(temp_select_str);
}/*}}}*/

int check_file_exists(char *file_path) /* Checking wheather the given file is exists or not. */
{
    if (access(file_path, F_OK) == -1) {
        php_error_docref(NULL, E_ERROR, "File: %s not exists.", file_path);
        return FALSE;
    }
    return TRUE;
}

void recursive_call_parent_method(zend_class_entry *ce, char *method_name)/*{{{  Parent class's method to current ce */
{
    if (ce) {
        recursive_call_parent_method(ce->parent, method_name);
        zval parent_obj;
        object_init_ex(&parent_obj, ce);
        if (CSPEED_METHOD_IN_OBJECT(&parent_obj, method_name)){
            zval function_name, retval;
            ZVAL_STRING(&function_name, method_name);
            call_user_function(NULL, &parent_obj, &function_name, &retval, 0, NULL);
            zval_ptr_dtor(&function_name);
            zval_ptr_dtor(&retval);
        }
        zval_ptr_dtor(&parent_obj);
        ce = ce->parent;
    }
}/*}}}*/

void recursive_call_parent_method_two(zval *obj, char *method_name)/*{{{  Parent class's method to current ce */
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
            call_user_function(NULL, obj, &function_name, &retval, 0, NULL);
            zval_ptr_dtor(&function_name);
            zval_ptr_dtor(&retval);
        }
    }
}/*}}}*/

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
