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

#ifndef CSPEED_TOOL_HELPER_H
#define CSPEED_TOOL_HELPER_H

/* Exit the CSpeed kernel engine. */
void cspeed_exit(char *exit_info);

/**
 * [NOTE]: 
 * In CSpeed, developers must invoke this method
 * to print the info instead of the php_error_decref. 
 */
void cspeed_print_info(int type, const char *format, ...);

/* Get the PATH_INFO data */
void cspeed_get_path_info(zval *ret_path_info);

/* Reverse all slashes to reslash */
char *cspeed_reverse_slash_char(char *src);                                      

/* Get current directory */
char *cspeed_get_cwd();                                                          

/* Upper case the first char */
char *title_upper_string(char *src);                                             

/* Lower case the first char */
char *title_lower_string(char *src);                                             

/* Parse the ini file with the given node and key */
void cspeed_parse_ini_file(char *file_name, char *node_name, char *node_key, zend_bool, zval *retval);             

/* Building the string */
void cspeed_build_equal_string(zval *array, char *begin_str, zval *result);      

/* Building the quote string */
void cspeed_build_quote_string(zval *array, zval *result);                       

/* Check whether the file is exists or not. */
int check_file_exists(char *file_path);                                          

/* Calling the method from the top class to this */
void recursive_call_parent_method(zend_class_entry *ce, char *method_name);      

/* Want obj usefull, use this instead before */
void recursive_call_parent_method_two(zval *obj, char *method_name);             

/* invoke object's method with params */
void call_method_with_object(zval *object, char *method_name, uint32_t param_counts, zval params[], zval *ret_val);

/* invoke objects's method with the array params */
void call_method_with_object_params(zval *object, char *method_name, zval *parameters, zval *ret_val);

/* load the kernel_setting */
void load_kernel_setting(zend_string *, zend_string *, char *);                  

/* load the file with the namespace name */
int cspeed_autoload_file(zend_string *, zval *, char *);

/* filter the parameter with the callable or valid function */
void parameter_filter(zval *filter, zval *parameter);

/* Add the property to the object */
void add_object_property(zval *object, zval *properties);

/* Add the properties to the object */
void add_multi_object_property(zval *object, zval *multi_properties, zval *ret_val);

/* Return the substirng of the str */
char *substr(char *str, int start, int length);

/* Return the given pattern exists in str or not. */
int stringstr(char *str, char *pattern);

/* Replace the fake name with ?!? */
void replace_fake_name(char *str, char *find_str, char *fake_name, char *dest);

/* To replace all name with ?!? */
void replace_all_name(char *src, char *subject, char *replace_subject, char *dest, int *length, int space_first);

extern int stringexists(char *, char *);

#endif







/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
