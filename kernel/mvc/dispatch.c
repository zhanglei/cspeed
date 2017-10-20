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

#include <unistd.h>         /* Access function */
#include <fcntl.h>          /* Access function */

void request_dispatcher_url(zval *capp_object)      /*{{{ This method handle the URL request and dispatcher it to differnt controller' action */
{
    /* The parameter capp_object was the CApp class object, which you can use it to load the User setting from the php-level */

    /* NOTE THAT: 
        CSpeed parse the PATH_INFO info. to do the job. if your web server didn't support, the CSpeed will not support.
    */
    char *path_info = cspeed_request_server_str_key_val("PATH_INFO");
    
    char *default_module        = CSPEED_DISPATCH_DEFAULT_MODULE;
    char *default_controller    = CSPEED_DISPATCH_DEFAULT_CONTROLLER;
    char *default_action        = CSPEED_DISPATCH_DEFAULT_ACTION;

    char *path_array[3] = { NULL };             /* Only need three element for the ```module```, ```controller```, ```action``` */

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

    /* All registered module */
    zval *all_modules = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_APP_MODULES), 1, NULL);

    /* To judge the module is correctly register or not */
    if (path_array[2] != NULL) {
        /* First we must to check whether the module is registered or not */
        if (Z_ISNULL_P(all_modules)){
            php_error_docref(NULL, E_ERROR, "Registered module empty. You must register it first.");
            return ;
        }
        zend_bool is_exists = zend_hash_str_exists(Z_ARRVAL_P(all_modules), CSPEED_STRL(path_array[0]));
        if ( is_exists ) {
            php_error_docref(NULL, E_ERROR, "Module: %s not registered. You must register it first.", path_array[0]);
            return ;
        }
        /* path_array[0] : module, path_array[1]: controller, path_array[2] : action*/
        zend_string *module_path_exists = strpprintf(0, "%s/../%s", cspeed_get_cwd(), path_array[0]);
        if (access(ZSTR_VAL(module_path_exists), F_OK) == -1) {
            php_error_docref(NULL, E_ERROR, "Module registered. But not exists in project.");
            return ;
        }
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
        zend_string_release(module_path_exists);
    } else {
        /*path_array[0]: controller, path_array[1] : action*/
        if (path_array[0] != NULL) {
            title_upper_string(path_array[0]);
            default_controller = path_array[0];
        }
        if (path_array[1] != NULL) {
            default_action = ZSTR_VAL(strpprintf(0, "%sAction", path_array[1]));
        }
    }

    /* Combine the full path to include the file */
    zend_string *full_include_controller_path = strpprintf(0, "%s/../%s/controllers/%s.php", cspeed_get_cwd(), default_module, default_controller);
    if (access(ZSTR_VAL(full_include_controller_path), F_OK) == -1) {
        php_error_docref(NULL, E_ERROR, "Controller file :%s not found.", ZSTR_VAL(full_include_controller_path));
        return ;
    }
    cspeed_require_file(ZSTR_VAL(full_include_controller_path), NULL, NULL, NULL);
    zend_string_release(full_include_controller_path);
    /* After loading the controller file, find the loading file */
    zend_class_entry *controller_ptr = zend_hash_find_ptr(CG(class_table), zend_string_tolower(zend_string_init(CSPEED_STRL(default_controller), 0)));
    if (controller_ptr) {
        /*Add the view into the controller*/
        zval view_object;
        object_init_ex(&view_object, cspeed_view_ce);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_ROOT_DIR), default_module);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_CONTROLLER_MODULE_ID), default_module);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_CONTROLLER_CONTROLLER_ID), default_controller);
        zend_update_property_string(cspeed_view_ce, &view_object, CSPEED_STRL(CSPEED_VIEW_CONTROLLER_ACTION_ID), default_action);
        zend_declare_property_null(controller_ptr, CSPEED_STRL("view"), ZEND_ACC_PUBLIC);
        /*}}}*/

        zval controller_obj;
        object_init_ex(&controller_obj, controller_ptr);
        if (instanceof_function(controller_ptr, cspeed_controller_ce)) {
            zval *di_objects = zend_read_property(cspeed_app_ce, capp_object, CSPEED_STRL(CSPEED_APP_DI_OBJECT), 1, NULL); /* The di object */
            if (!Z_ISNULL_P(di_objects)) {
                zval *di_sets = zend_read_property(cspeed_di_ce, di_objects, CSPEED_STRL(CSPEED_DI_OBJECT), 1, NULL);
                zend_update_property(controller_ptr, &controller_obj, CSPEED_STRL(CSPEED_DI_OBJECT), di_sets);
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
