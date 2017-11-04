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


/*{{{ To require the file as the function in PHP: require
    for the variables which were come from user-input
 */
int cspeed_require_file(const char * file_name, zval *variables, zval *called_object_ptr, zval *return_view)
{
    zend_file_handle file_handle;

    file_handle.free_filename = 0;
    file_handle.type          = ZEND_HANDLE_FILENAME;
    file_handle.opened_path   = NULL;
    file_handle.filename      = file_name;
    file_handle.handle.fp     = NULL;
    zend_op_array *op_array = NULL;
    zend_try {
        op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);
    }zend_end_try();
    if (EG(exception)) {
        zend_exception_error(EG(exception), E_ERROR);
        return FALSE;
    }
    zend_execute_data *require_call = zend_vm_stack_push_call_frame(
          ZEND_CALL_NESTED_CODE | ZEND_CALL_HAS_SYMBOL_TABLE,
          (zend_function *)op_array, 
          0, 
          ( called_object_ptr != NULL ) ? Z_OBJCE_P(called_object_ptr) : NULL, 
          ( called_object_ptr != NULL ) ? Z_OBJ_P(called_object_ptr) : NULL
    );
    /* To extract the variables into the view file */
    if (variables == NULL) {
        zval empty_variables;
        array_init(&empty_variables);
        variables = &empty_variables;
    }
    if ( Z_TYPE_P(variables) == IS_ARRAY ){
        zend_array *symbol_tables = emalloc(sizeof(zend_array));
        zend_hash_init(symbol_tables, 8, NULL, ZVAL_PTR_DTOR, 0);
        zend_hash_real_init(symbol_tables, 0);
        zend_string *var_name;
        zval *var_value;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(variables), var_name, var_value){
            if (EXPECTED(zend_hash_add_new(symbol_tables, var_name, var_value))) {
                Z_TRY_ADDREF_P(var_value);
            }
        } ZEND_HASH_FOREACH_END();
        require_call->symbol_table = symbol_tables;
    }
    if (return_view && php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS) == FAILURE) {
        php_error_docref(NULL, E_WARNING, "Failed to call ob_start().");
        return FALSE;
    }
    zval result;
    zend_init_execute_data(require_call, op_array, &result);
    ZEND_ADD_CALL_FLAG(require_call, ZEND_CALL_TOP);
    zend_execute_ex(require_call);
    zend_vm_stack_free_call_frame(require_call);
    destroy_op_array(op_array);
    efree_size(op_array, sizeof(zend_op_array));
    if (EG(exception)) {
        zend_exception_error(EG(exception), E_ERROR);
        return FALSE;
    }
    zend_destroy_file_handle(&file_handle);
    if (return_view) { /* Store the data into the return_view zval struct and discard the data in the output */
        if (php_output_get_contents(return_view) == FAILURE) {
            php_output_end();
            php_error_docref(NULL, E_WARNING, "Can't fetch the ob_data.");
            return FALSE;
        }
        if (php_output_discard() != SUCCESS ) {
            return FALSE;
        }
    }
    return TRUE;
}
/*}}}*/















/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
