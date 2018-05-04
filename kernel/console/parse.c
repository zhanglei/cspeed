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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_cspeed.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"

#include "kernel/net/request.h"
#include "kernel/tool/helper.h"
#include "kernel/tool/require.h"
#include "kernel/tool/component.h"
#include "kernel/mvc/controller.h"

void 
parse_cli_path_info(zval *path_info_array)
/*{{{ Parsing the PATH_INFO to obtain the right Model|Controller|Action */
{   
    /* In ZTS, the web directory are not right. fix it. */
    char path[MAXPATHLEN];

    zend_ulong index; zval *value;

    /* The web directory */
    char *root_dir  = ZSTR_VAL(CSPEED_G(core_application));
    
    /* Get the default value from the engine. */
    zend_string *default_module    = CSPEED_G(core_router_default_module);
    zend_string *default_controller= CSPEED_G(core_router_default_controller);
    zend_string *default_action    = CSPEED_G(core_router_default_action);

    /* The module path */
    zend_string *temp_module_path = NULL, *temp_controller_path = NULL;

    /* To check wheather the module is exists */
    zval *temp_mca_value;
    if (UNEXPECTED((temp_mca_value = 
        zend_hash_index_find(
            Z_ARRVAL_P(path_info_array), 
            1
        )) != NULL)) {
        CSPEED_G(core_router_default_module) = zend_string_dup(
            Z_STR_P(temp_mca_value), 
            0
        );
        default_module = zend_string_copy(Z_STR_P(temp_mca_value));
        zend_hash_index_del(Z_ARRVAL_P(path_info_array), 1);
    }

    /* To check wheather the module is allowed or not. */
    int is_allowed = FALSE;
    if (zend_hash_num_elements(CSPEED_G(core_router_modules))) {
        zval *allowed_module;
        ZEND_HASH_FOREACH_VAL(CSPEED_G(core_router_modules), allowed_module){
            if ( (memcmp(
                    Z_STRVAL_P(allowed_module), 
                    CSPEED_STRL( ZSTR_VAL(default_module) ) 
                ) == 0) ||
                 (memcmp(
                    Z_STRVAL_P(allowed_module),
                    CSPEED_STRL("*")
                ) == 0)
            ) {
                is_allowed = TRUE;
                break;
            }
        } ZEND_HASH_FOREACH_END();
    }
    if (is_allowed == FALSE){
        zend_error(
            E_ERROR, 
            "Module: `%s` are not allowed to access.", 
            ZSTR_VAL(default_module)
        );
        return ;
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

    if ( EXPECTED((temp_mca_value = zend_hash_index_find(Z_ARRVAL_P(path_info_array), 2)) != NULL) ) {
        default_controller = zend_string_copy(Z_STR_P(temp_mca_value));
        CSPEED_G(core_router_default_controller) = zend_string_dup(Z_STR_P(temp_mca_value), 0);
        zend_hash_index_del(Z_ARRVAL_P(path_info_array), 2);
    }
    title_upper_string(ZSTR_VAL(default_controller));

    /* Loading the file directly */
    zend_string *controller_file = strpprintf(
        0, 
        "%s/controllers/%s.php", 
        ZSTR_VAL(temp_module_path), 
        ZSTR_VAL(default_controller)
    );
    check_file_exists(ZSTR_VAL(controller_file));
    cspeed_require_file(ZSTR_VAL(controller_file), NULL, NULL, NULL);

    /* After require the class from the controller file. create the controller object and do the initialise process */
    zval controller_obj;
    zend_class_entry *controller_ptr = zend_hash_find_ptr(
        EG(class_table), 
        zend_string_tolower(default_controller)
    );
    if (controller_ptr) {
        if (!instanceof_function(controller_ptr, cspeed_controller_ce)){
            zend_error(
                E_ERROR, 
                "Controller class must extends from \\Cs\\mvc\\Controller class."
            );
        }
        object_init_ex(&controller_obj, controller_ptr);
    } else {
        zend_error(
            E_ERROR, 
            "Controller class :`%s` not found.", 
            ZSTR_VAL(default_controller)
        );
        return ;
    }
    /* Parsing the action in PATH_INFO */
    if ( UNEXPECTED( (
        temp_mca_value = 
            zend_hash_index_find(
                Z_ARRVAL_P(path_info_array), 
                3
            )
        ) != NULL )) {
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
        ZVAL_OBJ(&router_object, CSPEED_G(router_object));
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
        if ( (index != 0) && ( ( index & 1 ) == 0) ){    /* Key name*/
            add_next_index_zval(&key_sets, value);
        }
        if ( (index != 0) && ( ( index & 1 ) != 0) ) {   /* Key value */
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
            add_assoc_zval(global_get_variables, ZSTR_VAL(key), real_key_value);
        }
    } ZEND_HASH_FOREACH_END();

    /* To call the parent initialise method to do the initialise */
    zend_class_entry *parent = controller_ptr;

    /* To fix the parent calling bug #1103 */
    recursive_call_parent_method_two(&controller_obj, "initialise");

    /* After running the initialise method. running the __beforeAction method */
    trigger_events(
        &controller_obj, 
        strpprintf(
            0, 
            "%s", 
            EVENT_BEFORE_ACTION
        )
    );

    /* After adding the value into $_GET run the controller's action */
    zend_string *action_append_action = strpprintf(
        0, 
        "%sAction", 
        ZSTR_VAL(default_action)
    );
    if (CSPEED_METHOD_IN_OBJECT(&controller_obj, ZSTR_VAL(action_append_action))) {
        zval function_name, retval_ptr;
        ZVAL_STRING(&function_name, ZSTR_VAL(action_append_action));
        call_user_function(NULL, &controller_obj, &function_name, &retval_ptr, 0, NULL);
        zval_ptr_dtor(&function_name);
        zval_ptr_dtor(&retval_ptr);
    } else {
        zend_string_release(action_append_action);
        zend_error(
            E_ERROR, 
            "Controller class has not the :`%s` method.", 
            ZSTR_VAL(action_append_action)
        );
        return ;
    }
    zend_string_release(action_append_action);
    /* Do the after_action work */
    trigger_events(
        &controller_obj, 
        strpprintf(
            0, 
            "%s", 
            EVENT_AFTER_ACTION
        )
    );

    /* Release the unused memory */
    zval_ptr_dtor(&controller_obj);
    /* Release the not need memory */
    zend_string_release(temp_module_path);
    zend_string_release(default_module);
    zend_string_release(default_controller);
    zend_string_release(default_action);
}/*}}}*/


void 
dispather_cli_url(zend_string *info)
/* {{{ Dispatcher the URL */
{ 
    char *path_info = ( *ZSTR_VAL(info) == '/' ) ? ZSTR_VAL(info) : 
        ZSTR_VAL(
            strpprintf(
                0, 
                "/%s", 
                ZSTR_VAL(info)
            )
        );

    zval path_info_array;
    array_init(&path_info_array);
    php_explode(
        zend_string_init(
            CSPEED_STRL("/"), 
            0
        ), 
        zend_string_init(
            CSPEED_STRL(path_info), 
            0
        ),
        &path_info_array, 
        ZEND_LONG_MAX
    );

    /*After Parsing the Router URL rules, parsing the PATH_INFO TO the Right Model|Controller|Action */
    parse_cli_path_info(&path_info_array);
}


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
