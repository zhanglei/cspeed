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

#include "kernel/CApp.h"           /* The view object */
#include "kernel/di/di.h"           /* The view object */
#include "kernel/mvc/view.h"        /* The view object */
#include "kernel/mvc/dispatch.h"
#include "kernel/mvc/controller.h"  /* Use the controller features */
#include "kernel/net/request.h"     /* Use the Request object */
#include "kernel/net/response.h"    /* Use the Response object to response the user request */
#include "kernel/tool/require.h"    /* Use the require tool to do the file rendering */
#include "kernel/tool/helper.h"     /* The helper function tool */
#include "kernel/mvc/router.h"      /* The Router class */

#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_string.h"

#include <unistd.h>         /* Access function */
#include <fcntl.h>          /* Access function */

void request_dispatcher_url(zval *capp_object)      /*{{{ This method handle the URL request and dispatcher it to differnt controller' action */
{
    /* The parameter capp_object was the CApp class object, which you can use it to load the User setting from the php-level */

    /* NOTE THAT: 
        CSpeed parse the PATH_INFO info. to do the job. if your web server didn't support, the CSpeed will not support.
    */
    char *path_info = cspeed_request_server_str_key_val("PATH_INFO");
    char *back_path_info = (char *)malloc(sizeof(char) * strlen(path_info) + 1);
    memset(back_path_info, 0, strlen(path_info)+1);
    memcpy(back_path_info, CSPEED_STRL(path_info));

    /* Try to get the default module|controller|action from the User-level */
    zval *app_default_module = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_DEFAULT_MODULE), 1, NULL);
    zval *app_default_controller = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_DEFAULT_CONTROLLER), 1, NULL);
    zval *app_default_action = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_DEFAULT_ACTION), 1, NULL);

    char *default_module     =  Z_STRVAL_P(app_default_module);
    char *default_controller =  Z_STRVAL_P(app_default_controller);
    char *default_action     =  Z_STRVAL_P(app_default_action);

    char *path_array[3] = { NULL };             /* Only need three element for the `module`, `controller`, `action` */

    if (CSPEED_STRING_NOT_EMPTY(path_info)){
        char delims[] = "/";
        char *result = NULL;
        result = strtok( path_info + 1, delims );
        int i;
        for (i = 0; result != NULL; ++i)
        {
            if (i > 2 ) { break; }
            path_array[i] = result;
            result = strtok( NULL, delims);
        }
    }

    /* Setting the M|C|A to the right string */
    if (path_array[0] != NULL) {
        default_module = path_array[0];
    }
    if (path_array[1] != NULL) {
        title_upper_string(path_array[1]);
        default_controller = path_array[1];
    }
    if (path_array[2] != NULL) {
        default_action = ZSTR_VAL(strpprintf(0, "%sAction", path_array[2]));
    }

    /* Below are doing the router URL change */
    /* After the right regular URL parsing, do the routine setting, if setting */
    zval *app_di_object = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_APP_DI_OBJECT), 1, NULL);

    zval *di_objects = ZVAL_IS_NULL(app_di_object) ? NULL : zend_read_property(cspeed_di_ce, app_di_object, 
                                                                CSPEED_STRL(CSPEED_DI_OBJECT), 1, NULL);
    zval *router_object = di_objects ? zend_hash_find(Z_ARRVAL_P(di_objects), zend_string_init(CSPEED_STRL("router"), 0)) : NULL;
    
    zval *all_routines = router_object ? zend_read_property(cspeed_router_ce, router_object, 
                                                                CSPEED_STRL(CSPEED_ROUTER_ALL_ROUTINES), 1, NULL) : NULL;
    if (all_routines && zend_hash_num_elements(Z_ARRVAL_P(all_routines))){
        /* If the Routine in the routine settting */
        /* Use the preg_match to match all the URL routine */
        zend_string *vvar_key;
        zval *vvar_value;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(all_routines), vvar_key, vvar_value){
            pcre_cache_entry *pce_regexp;
            if ((pce_regexp = pcre_get_compiled_regex_cache(vvar_key)) == NULL) {
                php_error_docref(NULL, E_ERROR, "Can't compile PCRE_Regex.");
                return ;
            } else {
                zval matches, subparts;
                ZVAL_NULL(&subparts);
                php_pcre_match_impl(pce_regexp, CSPEED_STRL(back_path_info), &matches, &subparts, 0, 0, 0, 0);

                if (!zend_hash_num_elements(Z_ARRVAL(subparts))) {
                    zval_ptr_dtor(&subparts);
                    /* Noting match the given url routine do the default */
                } else {
                    /* The given URL matches the path_info, parsing the module|controller|action of it */
                    zend_hash_index_del(Z_ARRVAL(subparts), 0);
                    
                    /* Find if the given module|controller|action is given or number is given
                     * if number, to fetch the url of the subparts
                     */
                    zval *module = zend_hash_find(Z_ARRVAL_P(vvar_value), zend_string_init( CSPEED_STRL("module") ,0));
                    if (module && (Z_TYPE_P(module) == IS_LONG)) {
                        zval *real_module = zend_hash_index_find(Z_ARRVAL(subparts), Z_LVAL_P(module));
                        if (real_module == NULL) {
                            php_error_docref(NULL, E_ERROR, "Can't found the offset %s module int the URL.",
                                             Z_LVAL_P(module));
                            return ;
                        }
                        Z_TRY_ADDREF_P(real_module);
                        default_module = Z_STRVAL_P(real_module);
                    } else if (module && (Z_TYPE_P(module) == IS_STRING)) {
                        default_module = Z_STRVAL_P(module);
                    }
                    zval *controller = zend_hash_find(Z_ARRVAL_P(vvar_value), zend_string_init(CSPEED_STRL("controller"), 0));
                    if (controller && (Z_TYPE_P(controller) == IS_LONG)){
                        zval *real_controller = zend_hash_index_find(Z_ARRVAL(subparts), Z_LVAL_P(controller));
                        if (real_controller == NULL){
                            php_error_docref(NULL, E_ERROR, "Can't found the offset %d controller int the URL.",
                                            Z_LVAL_P(controller));
                            return ;
                        }
                        if (Z_TYPE_P(real_controller) == IS_STRING){
                            Z_TRY_ADDREF_P(real_controller);
                            default_controller = title_upper_string(Z_STRVAL_P(real_controller));
                        }
                    } else if (controller && (Z_TYPE_P(controller) == IS_STRING)){
                        default_controller = title_upper_string(Z_STRVAL_P(controller));
                    }
                    zval *action = zend_hash_find(Z_ARRVAL_P(vvar_value), zend_string_init(CSPEED_STRL("action"), 0));
                    if (action && (Z_TYPE_P(action) == IS_LONG)){
                        zval *real_action = zend_hash_index_find(Z_ARRVAL(subparts), Z_LVAL_P(action));
                        if (real_action == NULL){
                            php_error_docref(NULL, E_ERROR, "Can't found the offset %d action in the URL.",
                                            Z_LVAL_P(action));
                            return ;
                        }
                        Z_TRY_ADDREF_P(real_action);
                        default_action = ZSTR_VAL(strpprintf(0, "%sAction", Z_STRVAL_P(real_action)));
                    } else if (action && (Z_TYPE_P(action) == IS_STRING )){
                        default_action = ZSTR_VAL(strpprintf(0, "%sAction", Z_STRVAL_P(action)));
                    }
                    zval_ptr_dtor(&subparts);
                }
            }
        } ZEND_HASH_FOREACH_END();
        /*Free the Memory of the CSpeed malloc*/
        free(back_path_info);
    }
    /* End the Routine setting */

    /* All registered module */
    zval *all_modules = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_APP_MODULES), 1, NULL);

    /* To judge the module is correctly register or not */
    /* First we must to check whether the module is registered or not */
    if (Z_ISNULL_P(all_modules)){
        php_error_docref(NULL, E_ERROR, "Registered module empty. You must register it first.");
        return ;
    }
    /* Below are the module initialise process: */
    /* First we must find which the given module is allowed or not */
    zval *exists_module_file = zend_hash_find(Z_ARRVAL_P(all_modules), strpprintf(0, "%s", CSPEED_STRL(default_module)));
    if (exists_module_file == NULL) {
        php_error_docref(NULL, E_ERROR, "Module: %s not registered. You must register it first.", default_module);
        return ;
    }
    default_module = Z_STRVAL_P(exists_module_file);
    /* Loading the module file to do the initialise job */
    zend_string *module_file = strpprintf(0, "%s/%s/Module.php", cspeed_get_cwd(), Z_STRVAL_P(exists_module_file));
#if 0 
    /* Below are the Module initialise function to force the Module.php must exists. to do the initialise job. */
    if (access(ZSTR_VAL( module_file ), F_OK) == -1) {
        php_error_docref(NULL, E_ERROR, "Module file :%s not found.", ZSTR_VAL(module_file));
        return ;
    }
#endif
    if (access(ZSTR_VAL( module_file ), F_OK) != -1) {
        /*RESET the default_moudle using the registerModules() function */
        cspeed_require_file(ZSTR_VAL(module_file), NULL, NULL, NULL);
        zend_class_entry *module_ptr = zend_hash_find_ptr(CG(class_table), 
            zend_string_tolower(zend_string_init(CSPEED_STRL("Module"), 0)));
        if (module_ptr) {
            zval module_object;
            object_init_ex(&module_object, module_ptr);
            if (CSPEED_METHOD_IN_OBJECT(&module_object, "initialise")){
                zval module_initialise_function;
                ZVAL_STRING(&module_initialise_function, "initialise");
                zval initialise_retval;
                call_user_function(NULL, &module_object, &module_initialise_function, &initialise_retval, 0, NULL);
            } else {
                php_error_docref(NULL, E_ERROR, "Module class must has the initialise method to do the initialise job.");
                return ;
            }
        } else {
            php_error_docref(NULL, E_ERROR, "Module class not found in file Module.php.");
            return ;
        }

        zend_string *module_path_exists = strpprintf(0, "%s/%s", cspeed_get_cwd(), default_module);
        if (access(ZSTR_VAL(module_path_exists), F_OK) == -1) {
            php_error_docref(NULL, E_ERROR, "Module registered. But not exists in project.");
            return ;
        }
        zend_string_release(module_path_exists);
    }

    /* Combine the full path to include the file */
    zend_string *full_include_controller_path = strpprintf(0, "%s/%s/controllers/%s.php",
        cspeed_get_cwd(), default_module, default_controller);
    if (access(ZSTR_VAL(full_include_controller_path), F_OK) == -1) {
        php_error_docref(NULL, E_ERROR, "Controller file :%s not found.", ZSTR_VAL(full_include_controller_path));
        return ;
    }
    cspeed_require_file(ZSTR_VAL(full_include_controller_path), NULL, NULL, NULL);
    zend_string_release(full_include_controller_path);
    /* After loading the controller file, find the loading file */
    zend_class_entry *controller_ptr = zend_hash_find_ptr(CG(class_table), zend_string_tolower(
        zend_string_init(CSPEED_STRL(default_controller), 0)));
    if (controller_ptr) {
        /*Add the view into the controller*/
        zval view_object;
        object_init_ex(&view_object, cspeed_view_ce);
        zval object_variables;
        array_init(&object_variables);
        zend_update_property(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_VARIABLES), &object_variables);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_ROOT_DIR), default_module);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_CONTROLLER_MODULE_ID), default_module);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_CONTROLLER_CONTROLLER_ID), default_controller);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_CONTROLLER_ACTION_ID), default_action);
        zend_declare_property_null(controller_ptr, CSPEED_STRL("view"), ZEND_ACC_PUBLIC);
        /*}}}*/

        zval controller_obj;
        object_init_ex(&controller_obj, controller_ptr);
        if (instanceof_function(controller_ptr, cspeed_controller_ce)) {
            zval *di_objects = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_APP_DI_OBJECT), 1, NULL); /* Di */
            if (!ZVAL_IS_NULL(di_objects) && (Z_TYPE_P(di_objects) == IS_OBJECT)) {
                zend_update_property(controller_ptr, &controller_obj, CSPEED_STRL(CSPEED_DI_INSTANCE), di_objects);
            }
        }

        zend_update_property(controller_ptr, &controller_obj, CSPEED_STRL("view"), &view_object);
        if (CSPEED_METHOD_IN_OBJECT(&controller_obj, default_action)){
            /* After creating the controller object, you can do some initialise thing before calling the need action */

            /* Adding the other code before */
            zval function_name, retval_ptr;
            ZVAL_STRING(&function_name, default_action);
            uint32_t param_count = 0;
            zval *params = NULL;
            call_user_function(NULL, &controller_obj, &function_name, &retval_ptr, param_count, params);
        } else {
            php_error_docref(NULL, E_ERROR, "Controller %s didn't has the %s method.", default_controller, default_action);
            return ;
        }
    } else {
        php_error_docref(NULL, E_ERROR, "Controller class: %s not exists the include file.", default_controller);
        return ;
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
