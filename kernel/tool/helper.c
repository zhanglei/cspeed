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
#include "kernel/mvc/model.h"
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

/*{{{ proto Return the result of the reservsed slash char* */
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
                        smart_str_appends(&where_str, ZSTR_VAL(strpprintf(0, " '%s", Z_STRVAL_P(relation_value))));
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
                    smart_str_appends(&where_str, ZSTR_VAL(strpprintf(0, "%d", Z_LVAL_P(var_value))));
                } else if ( Z_TYPE_P(var_value) == IS_DOUBLE ) {
                    smart_str_appends(&where_str, ZSTR_VAL(strpprintf(0, "%f", Z_DVAL_P(var_value))));
                } else if ( Z_TYPE_P(var_value) == IS_STRING ){
                    smart_str_appends(&where_str, Z_STRVAL_P(var_value));
                }
                smart_str_appends(&where_str, "' AND ");
            }
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
    memset(temp_select_str, 0, ZSTR_LEN(field_str.s));
    memcpy(temp_select_str, ZSTR_VAL(field_str.s), ZSTR_LEN(field_str.s) - 1);
    smart_str_free(&field_str);
    ZVAL_STRING(result, temp_select_str);
    free(temp_select_str);
}/*}}}*/

int 
check_file_exists(char *file_path) /* Checking wheather the given file is exists or not. */
{
    if (access(file_path, F_OK) == -1) {
        php_error_docref(NULL, E_ERROR, "File: %s not exists.", file_path);
    }
    return TRUE;
}

void 
recursive_call_parent_method(zend_class_entry *ce, char *method_name)/*{{{  Parent class's method to current ce */
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

void 
recursive_call_parent_method_two(zval *obj, char *method_name)/*{{{  Parent class's method to current ce */
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
            call_user_function(NULL, obj, &function_name, &retval, 1, params);
            zval_ptr_dtor(&function_name);
            zval_ptr_dtor(&retval);
        }
    }
}/*}}}*/

void
call_method_with_object(zval *object, char *method_name, uint32_t param_counts, zval params[], zval *ret_val)
{
    if (CSPEED_METHOD_IN_OBJECT(object, method_name)){
        zval function_name;
        ZVAL_STRING(&function_name, method_name);
        call_user_function(EG(function_table), object, &function_name, ret_val, param_counts, params);
        zval_ptr_dtor(&function_name);
    }
}

int 
cspeed_autoload_file(zend_string *class_name_with_namespace, zval *obj, char *alias) /*{{{ Load file */
{    
    if ( *(ZSTR_VAL(class_name_with_namespace)) == '\\' ) {
        class_name_with_namespace = strpprintf(0, "%s", ZSTR_VAL(class_name_with_namespace) + 1);
    }

    if (zend_hash_find_ptr(EG(class_table), zend_string_tolower(class_name_with_namespace)) != NULL) {
        return TRUE;
    }

    char *slash_pos = strchr(ZSTR_VAL(class_name_with_namespace), '\\');

    if (slash_pos == NULL) { /* No slash find */
        zend_string *real_file_path = strpprintf(0, "./%s.php", ZSTR_VAL(class_name_with_namespace));
        check_file_exists(ZSTR_VAL(real_file_path));
        if (cspeed_require_file(ZSTR_VAL(real_file_path), NULL, NULL, NULL) == FALSE){
            return FALSE;
        }
        zend_string_release(real_file_path);
    } else {                 /* find the slash */

        char *current_alias = (char *)malloc(sizeof(char) * (slash_pos - ZSTR_VAL(class_name_with_namespace) + 1));
        memset(current_alias, 0, (slash_pos - ZSTR_VAL(class_name_with_namespace) + 1));
        memcpy(current_alias, ZSTR_VAL(class_name_with_namespace), (slash_pos - ZSTR_VAL(class_name_with_namespace)));

        /* Before the steps the current_alias was `app` for example
            After we get the namespace alias, we can find the global aliases to find whether the alias is exists or not.
            if exists, replace it and plus the left character to form the correctly path
         */
        zval *all_app_aliases = zend_read_property(Z_OBJCE_P(obj), obj, CSPEED_STRL(alias), 1, NULL);
        zval *has_exists = zend_hash_find(Z_ARRVAL_P(all_app_aliases), zend_string_init(CSPEED_STRL(current_alias), 0));

        if ( strncmp(current_alias, ("Cs"), strlen("Cs")) == 0 ) {
            free(current_alias);
            php_error_docref(NULL, E_ERROR, "CSpeed framework not contain the class: `%s`.", ZSTR_VAL(class_name_with_namespace));
        }

        if ( has_exists ) {   /* Exists the need alias */
            int real_file_path_size = (Z_STRLEN_P(has_exists) + ZSTR_LEN(class_name_with_namespace)
                                        - (slash_pos - ZSTR_VAL(class_name_with_namespace)) + 5);
            char *real_file_path = (char *)malloc(sizeof(char) * real_file_path_size); /* five means the .php & space*/
            memset(real_file_path, 0, real_file_path_size);

            strncat(real_file_path, Z_STRVAL_P(has_exists), Z_STRLEN_P(has_exists));
            strncat(real_file_path, ZSTR_VAL(class_name_with_namespace) + (slash_pos - ZSTR_VAL(class_name_with_namespace)),
                                 ZSTR_LEN(class_name_with_namespace) - (slash_pos - ZSTR_VAL(class_name_with_namespace)));
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
            php_error_docref(NULL, E_ERROR, "Namespace alias: %s not found. please set it first before use.", current_alias);
        }
    }
    return TRUE;
} /*}}}*/


void
load_kernel_setting(zend_string *ini_config_file, zend_string *ini_config_node_name, char *path) /*{{{ Load the kernel default setting */
{
    if ( ini_config_file && CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(ini_config_file))) {
        zend_string *ini_real_file;
        if ( *(ZSTR_VAL(ini_config_file)) == '/' ) {
            ini_real_file = ini_config_file;
        } else {
            ini_real_file = strpprintf(0, "%s/%s", path, ZSTR_VAL(ini_config_file));
        }
        check_file_exists(ZSTR_VAL(ini_real_file));
        zval configs;
        zend_string *node_core_name = strpprintf(0, "%s%s%s", ini_config_node_name ? ZSTR_VAL(ini_config_node_name) : "",
                    ini_config_node_name ? ":" : "", "core" );
        zend_string *node_db_name = strpprintf(0, "%s%s%s", ini_config_node_name ? ZSTR_VAL(ini_config_node_name) : "",
                    ini_config_node_name ? ":" : "", "db" );
        /* Parsing the INI file */
        cspeed_parse_ini_file(ZSTR_VAL(ini_real_file), NULL , NULL, 1, &configs);

        if (ZVAL_IS_NULL(&configs)  && !zend_hash_num_elements(Z_ARRVAL(configs)) ) {
            php_error_docref(NULL, E_ERROR, "Configs empty.");
        }
        if ( ( Z_TYPE(configs) == IS_ARRAY ) ) {
            zval *config_value, *core_configs;
            if ( EXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_core_name)) == NULL )){
                php_error_docref(NULL, E_ERROR, "`%s` configs not found in config file.", ZSTR_VAL(node_core_name));
            }
            /*core.application*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_APPLICATION_NAME))) != NULL ) ) {
                CSPEED_G(core_application) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_APPLICATION_NAME);
            }
            /*core.bootstrap*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_BOOTSTRAP_NAME);
            }
            /*core.bootstrap*/
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_BOOTSTRAP_METHOD_NAME))) != NULL ) ) {
                CSPEED_G(core_bootstrap_method_string) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_BOOTSTRAP_METHOD_NAME);
            }
            /* core.router.modules */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_MODULES_NAME) )) != NULL )) {
                zval modules;
                array_init(&modules);
                php_explode(zend_string_init(CSPEED_STRL(","), 0), Z_STR_P(config_value), &modules, ZEND_LONG_MAX);
                zval_add_ref(&modules);
                CSPEED_G(core_router_modules) = Z_ARRVAL(modules);
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_MODULES_NAME);
            }
            /* core.router.default.module */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_MODULE_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_module) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_MODULE_NAME);
            }
            /* core.router.default.controller */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_CONTROLLER_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_controller) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_CONTROLLER_NAME);
            }
            /* core.router.default.action */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_DEFAULT_ACTION_NAME) )) != NULL )) {
                CSPEED_G(core_router_default_action) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_DEFAULT_ACTION_NAME);
            }
            /* core.view.ext */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_VIEW_EXT_NAME) )) != NULL )) {
                CSPEED_G(core_view_ext) = zend_string_copy(Z_STR_P(config_value));
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_VIEW_EXT_NAME);
            }
            /* core.view.auto_render */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_VIEW_AUTO_RENDER) )) != NULL )) {
                CSPEED_G(core_view_auto_render) = Z_STR_P(config_value);
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_VIEW_AUTO_RENDER);
            }
            /* core.url.pattern */
            if ( EXPECTED( (config_value = zend_hash_str_find(Z_ARRVAL_P(core_configs), 
                CSPEED_STRL(CORE_CONFIG_URL_PATTERN) )) != NULL )) {
                CSPEED_G(core_url_pattern) = Z_STR_P(config_value);
            } else {
                php_error_docref(NULL, E_ERROR, "Config :`%s` not set.", CORE_CONFIG_URL_PATTERN);
            }
            /* The DB config */
            if ( EXPECTED( (core_configs = zend_hash_find(Z_ARRVAL(configs), node_db_name )) == NULL )){
                php_error_docref(NULL, E_ERROR, "`%s` configs not found in config file.", ZSTR_VAL(node_db_name));
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
            php_error_docref(NULL, E_ERROR, "Configs data type wrong.");
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
                php_error_docref(NULL, E_ERROR, "function: `%s` not exists.", Z_STRVAL_P(filter));
            }
        } else if ( Z_TYPE_P(filter) == IS_OBJECT) {
            /* Check the object is callable or not. */
            zend_string *error_handler_name = NULL;
            if ( !zend_is_callable(filter, 0, error_handler_name) ) {
                php_error_docref(NULL, E_ERROR, "The argument must to be a valid callback.");
            }
            zval ret_val;
            zval params[] = { *parameter };
            call_user_function(EG(function_table), NULL, filter, &ret_val, 1, params);
            Z_TRY_ADDREF_P(&ret_val);
            ZVAL_COPY_VALUE(parameter, &ret_val);
        } else {
            php_error_docref(NULL, E_ERROR, "The argument must be valid function name or callable.");
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
            zend_property_info *property_info = zend_hash_find_ptr(&(Z_OBJCE_P(object))->properties_info, val_key);
            if ( property_info != NULL ) {
                if ( (property_info->flags & ZEND_ACC_SHADOW) 
                    || (property_info->flags & ZEND_ACC_PUBLIC)
                    || (property_info->flags & ZEND_ACC_PROTECTED)
                    || (property_info->flags & ZEND_ACC_PRIVATE)
                ) {
                    zend_update_property(Z_OBJCE_P(object), object, CSPEED_STRL(ZSTR_VAL(val_key)), val_value);
                } 
            } else {
                if ( !Z_OBJ_P(object)->properties ) {
                    rebuild_object_properties( Z_OBJ_P(object) );
                }
                zend_hash_add_new(Z_OBJ_P(object)->properties, (val_key), val_value);
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
        zval *value = zend_read_property(cspeed_model_ce, src_object, CSPEED_STRL(ZSTR_VAL(key)), 1, NULL);
        if ( !ZVAL_IS_NULL(value) ) {
            zend_update_property(cspeed_model_ce, dest_object, CSPEED_STRL(ZSTR_VAL(key)), value);
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
                object_and_properties_init(&t_object, Z_OBJCE_P(object), Z_OBJ_P(object)->properties);
                copy_object_properties(&t_object, object);
                zval_add_ref(val_value);
                SEPARATE_ZVAL(val_value);
                zend_update_property(cspeed_model_ce, &t_object, CSPEED_STRL(CSPEED_MODEL_ARRAY_DATA), val_value);
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
