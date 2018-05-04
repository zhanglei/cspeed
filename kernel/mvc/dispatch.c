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

#include "kernel/CApp.h"            /* The App object */
#include "kernel/di/di.h"           /* The Di object */
#include "kernel/mvc/view.h"        /* The view object */
#include "kernel/mvc/dispatch.h"
#include "kernel/mvc/controller.h"  /* Use the controller features */
#include "kernel/net/request.h"     /* Use the Request object */
#include "kernel/net/response.h"    /* Use the Response object to response the user request */
#include "kernel/tool/require.h"    /* Use the require tool to do the file rendering */
#include "kernel/tool/helper.h"     /* The helper function tool */
#include "kernel/mvc/router.h"      /* The Router class */
#include "kernel/rpc/server.h"      /* The suport of the Server class */

#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_string.h"

#include "kernel/tool/component.h"
#include <stdlib.h>

#include "main/SAPI.h"          /* for sapi */

/*{{{ Renderin the file automatically */
void auto_render_view_file(zend_class_entry *controller_ptr, zval *controller_obj, zval *view_object)
{
    if ( strncasecmp(
            CORE_VIEW_NEED_RENDER, 
            CSPEED_STRL( ZSTR_VAL( CSPEED_G(core_view_auto_render) ) )
        ) == 0 ) {
        /* First, create the view object */
        object_init_ex(view_object, cspeed_view_ce);
        /* Initialise the view properties */
        initialise_view_object_properties(view_object);
        /* attach the CSpeed created view object to the Controller engine. */
        zend_update_property(
            controller_ptr, 
            controller_obj, 
            CSPEED_STRL(CSPEED_VIEW_INSTANCE), 
            view_object
        );
    } else {
        ZVAL_NULL(view_object);
    }
}/*}}}*/


/*{{{ Parsing the PATH_INFO to obtain the right Model|Controller|Action */
void parse_path_info(zval *path_info_array)
{   
    /* In ZTS, the web directory are not right. fix it. */
    char path[MAXPATHLEN];

    zend_ulong index; zval *value;

    /* The web directory */
    char *root_dir          = ZSTR_VAL(CSPEED_G(core_application));
    
    /* Get the default value from the engine. */
    zend_string *default_module    = zend_string_tolower(CSPEED_G(core_router_default_module));
    zend_string *default_controller= zend_string_tolower(CSPEED_G(core_router_default_controller));
    zend_string *default_action    = zend_string_tolower(CSPEED_G(core_router_default_action));

    /* The module path */
    zend_string *temp_module_path = NULL, *temp_controller_path = NULL;

    /* To check wheather the module is exists */
    zval *temp_mca_value;
    if ( EXPECTED((temp_mca_value = zend_hash_index_find(Z_ARRVAL_P(path_info_array), 1)) != NULL) ) {
        CSPEED_G(core_router_default_module) = zend_string_dup(Z_STR_P(temp_mca_value), 0);
        default_module = zend_string_copy(Z_STR_P(temp_mca_value));
        zend_hash_index_del(Z_ARRVAL_P(path_info_array), 1);
    }

    /* To check wheather the module is allowed or not. */
    int is_allowed = FALSE;
    if (zend_hash_num_elements(CSPEED_G(core_router_modules))) {
        zval *allowed_module;
        ZEND_HASH_FOREACH_VAL(CSPEED_G(core_router_modules), allowed_module){
            if ( ( memcmp(
                    Z_STRVAL_P(allowed_module), 
                    CSPEED_STRL( ZSTR_VAL(default_module) )
                ) == 0) || 
                 ( memcmp(
                    Z_STRVAL_P(allowed_module), 
                    CSPEED_STRL("*")
                ) == 0 )
             ) {
                is_allowed = TRUE;
                break;
            }
        } ZEND_HASH_FOREACH_END();
    }
    if (is_allowed == FALSE){
        php_error_docref(
            NULL, 
            E_ERROR, 
            "Module: `%s` are not allowed to access.", 
            ZSTR_VAL(default_module)
        );
    }

    /* After the module is allowed. to parsing the module is exists or not. */
    temp_module_path = strpprintf(
        0, 
        "%s/%s/modules/%s", 
        cspeed_get_cwd(path), 
        root_dir, 
        ZSTR_VAL(default_module)
    );
    
    /* Exists or not. */
    check_file_exists(ZSTR_VAL(temp_module_path));

    if (EXPECTED((temp_mca_value = zend_hash_index_find(Z_ARRVAL_P(path_info_array), 2)) != NULL)) {
        default_controller = zend_string_copy(Z_STR_P(temp_mca_value));
        CSPEED_G(core_router_default_controller) = zend_string_dup(Z_STR_P(temp_mca_value), 0);
        zend_hash_index_del(Z_ARRVAL_P(path_info_array), 2);
    }
    title_upper_string(ZSTR_VAL(default_controller));

    zend_string *ns_class_name = strpprintf(
        0, 
        "app\\modules\\%s\\controllers\\%s",
        ZSTR_VAL(default_module), 
        ZSTR_VAL(default_controller)
    );
    zval *app_object = zend_read_static_property(
        cspeed_app_ce, 
        CSPEED_STRL(CSPEED_APP_INSTANCE), 
        1
    );
    if (ZVAL_IS_NULL(app_object)){
        php_error_docref(
            NULL, 
            E_ERROR, 
            "You must create a Cs\\App object first."
        );
    }
    if (cspeed_autoload_file(
            ns_class_name, 
            app_object, 
            CSPEED_APP_AUTOLOAD_ALIASES
        ) == FALSE){
        return ;
    }
    /* After require the class from the controller file. create the controller object and do the initialise process */
    zval controller_obj;
    zend_class_entry *controller_ptr = zend_hash_find_ptr(
        EG(class_table), 
        zend_string_tolower(ns_class_name)
    );
    if (controller_ptr) {
        if (!instanceof_function(controller_ptr, cspeed_controller_ce)){
            php_error_docref(
                NULL, 
                E_ERROR, 
                "Controller class must extends from \\Cs\\mvc\\Controller class."
            );
        }
        object_init_ex(&controller_obj, controller_ptr);
    } else {
        php_error_docref(
            NULL, 
            E_ERROR, 
            "Controller class :`%s` not found.", 
            ZSTR_VAL(default_controller)
        );
    }
    CSPEED_G(core_router_default_controller) = zend_string_tolower(CSPEED_G(core_router_default_controller));
    /* Parsing the action in PATH_INFO */
    if (EXPECTED( (temp_mca_value = zend_hash_index_find(Z_ARRVAL_P(path_info_array), 3)) != NULL )) {
        default_action = zend_string_copy(Z_STR_P(temp_mca_value));
        CSPEED_G(core_router_default_action) = zend_string_dup(Z_STR_P(temp_mca_value), 0);
        zend_hash_index_del(Z_ARRVAL_P(path_info_array), 3);
    }

    /* After create the controller object, put the value of the related properties */
    if (CSPEED_G(di_object)){
        zval di_object;
        ZVAL_OBJ(&di_object, CSPEED_G(di_object));
        zend_update_property(
            controller_ptr, 
            &controller_obj, 
            CSPEED_STRL(CSPEED_DI_INSTANCE), 
            &di_object
        );
    } else {
        zend_update_property_null(
            controller_ptr, 
            &controller_obj, 
            CSPEED_STRL(CSPEED_DI_INSTANCE)
        );
    }
    if (CSPEED_G(router_object)){
        zval router_object;
        ZVAL_OBJ(
            &router_object, 
            CSPEED_G(router_object)
        );
        zend_update_property(
            controller_ptr, 
            &controller_obj, 
            CSPEED_STRL(CSPEED_ROUTER_INSTANCE), 
            &router_object
        );
    } else {
        zend_update_property_null(
            controller_ptr, 
            &controller_obj, 
            CSPEED_STRL(CSPEED_ROUTER_INSTANCE)
        );
    }

    zend_update_property_null(
        controller_ptr, 
        &controller_obj, 
        CSPEED_STRL(CSPEED_VIEW_INSTANCE)
    );

    /* To obtain the real value from the PATH-INFO */
    zval key_sets, value_sets;
    array_init(&key_sets);
    array_init(&value_sets);
    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(path_info_array), index, value){
        if ( (index != 0) && ((index & 1) == 0) ){    /* Key name*/
            add_next_index_zval(&key_sets, value);
        }
        if ( (index != 0) && ((index & 1) != 0) ) {   /* Key value */
            add_next_index_zval(&value_sets, value);
        }
    } ZEND_HASH_FOREACH_END();
    /* Set the value into the $_GET global variables.*/
    zval *global_get_variables = cspeed_request_get(NULL);
    zend_ulong key_index; zval *key_name;
    ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL(key_sets), key_index, key_name){
        zval *real_key_value = zend_hash_index_find(Z_ARRVAL(value_sets), key_index);
        if (real_key_value) {
            zend_string *key;
            if (Z_TYPE_P(key_name) == IS_LONG) {
                key = strpprintf(0, "%d", Z_LVAL_P(key_name));
            } else if ( Z_TYPE_P(key_name) == IS_STRING) {
                key = Z_STR_P(key_name);
            } else {
                continue;
            }
            add_assoc_zval(
                global_get_variables, 
                ZSTR_VAL(key), 
                real_key_value
            );
        }
    } ZEND_HASH_FOREACH_END();

    /* Auto render the view file */
    zval view_object;
    auto_render_view_file(
        controller_ptr, 
        &controller_obj, 
        &view_object
    );

    /* To call the parent initialise method to do the initialise */
    zend_class_entry *parent = controller_ptr;

    /* To fix the parent calling bug #1103 */
    recursive_call_parent_method_two(&controller_obj, "initialise");

    if (!instanceof_function(controller_ptr, cspeed_rpc_server_ce)){
        /* After running the initialise method. running the __beforeAction method */
        trigger_events(&controller_obj, strpprintf(0, 
            "%s", 
            EVENT_BEFORE_ACTION)
        );
        /* After adding the value into $_GET run the controller's action */
        zend_string *action_append_action = strpprintf(0, 
            "%sAction", 
            ZSTR_VAL(default_action)
        );
        if (CSPEED_METHOD_IN_OBJECT(&controller_obj, ZSTR_VAL(action_append_action))) {
            zval function_name, retval_ptr;
            ZVAL_STRING(&function_name, ZSTR_VAL(action_append_action));
            call_user_function(
                NULL, 
                &controller_obj, 
                &function_name, 
                &retval_ptr, 
                0, 
                NULL
            );
            zval_ptr_dtor(&function_name);
            zval_ptr_dtor(&retval_ptr);
        } else {
            zend_string_release(action_append_action);
            php_error_docref(
                NULL, 
                E_ERROR, 
                "Controller class has not the :`%s` method.", 
                ZSTR_VAL(action_append_action)
            );
        }
        zend_string_release(action_append_action);
        /* To auto render the view file or not. */
        if ( !ZVAL_IS_NULL(&view_object) ) {
            render_view_file(
                &view_object, 
                CSPEED_G(core_router_default_action), 
                NULL, 
                NULL
            );
        }
        /* Do the after_action work */
        trigger_events(
            &controller_obj, 
            strpprintf(
                0, 
                "%s", 
                EVENT_AFTER_ACTION
            )
        );
    }
    /* Release the unused memory */
    zval_ptr_dtor(&controller_obj);
    zval_ptr_dtor(&view_object);
    /* Release the not need memory */
    zend_string_release(temp_module_path);
    zend_string_release(default_module);
    zend_string_release(default_controller);
    zend_string_release(default_action);
}/*}}}*/

void dispather_url()    /* {{{ Dispatcher the URL */
{
    /* PATH_INFO */
    char *path_info = cspeed_request_server_str_key_val("PATH_INFO");

    int can_free = FALSE;

    int pattern_pos;

    /* To remove the url_pattern string */
    if ( EXPECTED( 
        ( pattern_pos = 
            stringstr( 
                path_info, 
                ZSTR_VAL(CSPEED_G(core_url_pattern))
            ) 
        ) != FALSE ) ) {
        
        char *path_two = (char *)malloc(sizeof(char) * (pattern_pos + 1));

        memset(path_two, 0, (pattern_pos + 1));
        memcpy(path_two, path_info, pattern_pos);

        path_info = path_two;

        can_free = TRUE;
    }

    char *query_pos; /* The query position begin string */

    /* To cut the Query string for the url */
    if ( EXPECTED( (query_pos = strchr(path_info, '?')) != NULL ) ) {
        path_info = substr(
            path_info, 
            0, 
            query_pos - path_info
        );
        can_free = TRUE;
    }

    /* To get the PATH_INFO array */
    zval path_info_array;
    array_init(&path_info_array);
    php_explode(
        zend_string_init(CSPEED_STRL("/"), 0), 
        zend_string_init(CSPEED_STRL(path_info), 0),
        &path_info_array, 
        ZEND_LONG_MAX
    );

    /* After parsing the PATH_INFO. to find the matched URL, if not. do the default process */
    zval router_zval_object;
    zend_object *router_object = CSPEED_G(router_object);

    /*If NULL, to do the default process */
    if (router_object) {

        ZVAL_OBJ(&router_zval_object, router_object);
        /* Foreach all routines, to find the matched URL and redirect to it */
        zval *all_routines = zend_read_property(
            cspeed_router_ce, 
            &router_zval_object, 
            CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), 
            1, 
            NULL
        );
        
        zend_string *pcre_url;zval *value;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(all_routines), pcre_url, value){
            if ( CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(pcre_url)) && (Z_TYPE_P(value) == IS_STRING) ){
                /* Do the PREG_REPLACE */
                pcre_cache_entry *pce_regexp;
                if ((pce_regexp = pcre_get_compiled_regex_cache(pcre_url)) != NULL) {
                    zval matches, subparts;
                    ZVAL_NULL(&subparts);
                    php_pcre_match_impl(
                        pce_regexp, 
                        CSPEED_STRL(path_info), 
                        &matches, 
                        &subparts, 
                        0, 
                        0, 
                        0, 
                        0
                    );
                    if (zend_hash_num_elements(Z_ARRVAL(subparts))) {
                     
                        zval function_name, retval, url_pattern, url_subject;
                        ZVAL_STRING(
                            &function_name, 
                            "preg_replace"
                        );
                        ZVAL_STRING(
                            &url_pattern, 
                            ZSTR_VAL(pcre_url)
                        );
                        ZVAL_STRING(
                            &url_subject, 
                            path_info
                        );
                        zval params[] = { url_pattern, *value, url_subject };
                        call_user_function(
                            EG(function_table), 
                            NULL, 
                            &function_name, 
                            &retval, 
                            3, 
                            params
                        );
                        zval_ptr_dtor(&function_name);
                        zval_ptr_dtor(&url_pattern);
                        zval_ptr_dtor(&url_subject);
                        /* Start redirecting */
                        sapi_header_line ctr = {0};
                        ctr.line_len    = spprintf(
                            &(ctr.line), 
                            0, 
                            "%s %s", 
                            "Location:", 
                            Z_STRVAL(retval)
                        );
                        ctr.response_code   = 0;
                        if (sapi_header_op(SAPI_HEADER_REPLACE, &ctr) == SUCCESS) {
                            zval_ptr_dtor(&retval);
                            efree(ctr.line);
                            return ;
                        } else {
                            zval_ptr_dtor(&retval);
                            efree(ctr.line);
                            php_error_docref(
                                NULL, 
                                E_ERROR, 
                                "Please install SAPI extension."
                            );
                        }
                    }
                }
            }
        } ZEND_HASH_FOREACH_END();
    }

    if ( can_free ) free(path_info);

    /*After Parsing the Router URL rules, parsing the PATH_INFO TO the Right Model|Controller|Action */
    parse_path_info(&path_info_array);
}





/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
