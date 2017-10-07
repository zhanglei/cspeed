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

#include "kernel/mvc/dispatch.h"
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
    
    char *default_module        = ".";
    char *default_controller    = "Index";
    char *default_action        = "indexAction";

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
    /* Now the path info were parsed into correctly form */
    /**
     *  If the path_array[0] didn't exists:
     *    The path_array[0] means controller, path_array[1] equals action
     *  Else if path_array[0] exists the directory: 
     *      path_array[0] means module, path_array[1] equals controller path_array[2] equal action
     *  if not exists, using the default settting
     */
    if (path_array[0] != NULL){
        zend_string *module_path_exists = strpprintf(0, "%s/../%s", cspeed_get_cwd(), path_array[0]);
        if (access(ZSTR_VAL(module_path_exists), F_OK) == -1) {
            /* Module not exists, path_array[0] means the controller */
            title_upper_string(path_array[0]);
            default_controller = path_array[0];
            if (path_array[1] != NULL) {
                default_action = ZSTR_VAL(strpprintf(0, "%sAction", path_array[1]));
            }
        } else {
            default_module = path_array[0];
            if (path_array[1] != NULL) {
                title_upper_string(path_array[1]);
                default_controller = path_array[1];
            }
            if (path_array[2] != NULL) {
                default_action = ZSTR_VAL(strpprintf(0, "%sAction", path_array[2]));
            }
        }
    }
    /* Combine the full path to include the file */
    zend_string *full_include_controller_path = strpprintf(0, "%s/../%s/controllers/%s.php", cspeed_get_cwd(), default_module, default_controller);
    if (access(ZSTR_VAL(full_include_controller_path), F_OK) == -1) {
        php_error_docref(NULL, E_ERROR, "Controller file :%s not found.", ZSTR_VAL(full_include_controller_path));
        return ;
    }
    cspeed_require_file(ZSTR_VAL(full_include_controller_path), NULL, NULL, NULL);
    /* After loading the controller file, find the loading file */
    zend_class_entry *controller_ptr = zend_hash_find_ptr(CG(class_table), zend_string_tolower(zend_string_init(CSPEED_STRL(default_controller), 0)));
    if (controller_ptr) {
        zval controller_obj;
        object_init_ex(&controller_obj, controller_ptr);
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
    /*zend_printf("module:[%s]<br>controller:[%s]<br>action:[%s]<br>", default_module, default_controller, default_action);*/

}/*}}}*/









/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
