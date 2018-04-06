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

#include "kernel/CApp.h"
#include "kernel/tool/helper.h"
#include "kernel/tool/require.h"
#include "kernel/object_factory.h"

ZEND_BEGIN_ARG_INFO_EX(arg_info_objectf_init, 0, 0, 1)
    ZEND_ARG_INFO(0, config_file)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_info_objectf_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_info_objectf_get_object, 0, 0, 1)
    ZEND_ARG_INFO(0, object_id)
ZEND_END_ARG_INFO()

CSPEED_METHOD(ObjectFactory, __construct)
{
  zval *stores = zend_read_property(cspeed_object_factory_ce, getThis(), CSPEED_STRL(OBJECT_FACTORY_STORE), 1, NULL);
  if ( ZVAL_IS_NULL(stores) ){
      zval magic_datas;
      array_init(&magic_datas);
      zend_update_property(cspeed_object_factory_ce, getThis(), CSPEED_STRL(OBJECT_FACTORY_STORE), &magic_datas);
      zval_ptr_dtor(&magic_datas);
  }
}

CSPEED_METHOD(ObjectFactory, init)
{
  zend_string *file_path;
  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &file_path) == FAILURE) {
    return ;
  }
  check_file_exists(ZSTR_VAL(file_path));
  zval result;
  if (cspeed_require_php_file(ZSTR_VAL(file_path), &result) == FALSE) {
    return ;
  }

  /* To do the analyse job for the IOC factory */
  if ( (Z_TYPE(result) == IS_ARRAY) && 
    zend_hash_num_elements(Z_ARRVAL(result))
  ) {
      zend_string *val_name;
      zval *val_value;
      ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL(result), val_name, val_value) {
        
        /* TO DO.. */
        if (Z_TYPE_P(val_value) == IS_ARRAY) {
          zval *class_name = zend_hash_find(Z_ARRVAL_P(val_value), strpprintf(0, "%s", "class"));
          if (class_name && CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(class_name))) {
            /* To autoload the class file with the psr4 */
            zval *app_object = zend_read_static_property(cspeed_app_ce, CSPEED_STRL(CSPEED_APP_INSTANCE), 1);
            if ( !app_object || ZVAL_IS_NULL(app_object) || (Z_TYPE_P(app_object) != IS_OBJECT) ) {
              php_error_docref(NULL, E_ERROR, "Please do the IOC job in the \\Cs\\App inside.");
            }

            /* After include the file do the create job for each class */
            zend_class_entry *class_ce;
out_again:
            if ( *(Z_STRVAL_P(class_name)) == '\\' ){
              class_ce = zend_hash_find_ptr(EG(class_table), zend_string_tolower(strpprintf(0, "%s", Z_STRVAL_P(class_name) + 1)));
            } else {
              class_ce = zend_hash_find_ptr(EG(class_table), zend_string_tolower(Z_STR_P(class_name)));
            }

            if ( !class_ce ){
              /* Auto include the file */
              cspeed_autoload_file(Z_STR_P(class_name), app_object, CSPEED_APP_AUTOLOAD_ALIASES);
              goto out_again;
            }

            if ( class_ce && CSPEED_STRING_NOT_EMPTY( ZSTR_VAL(class_ce->name) ) ) {
              zval class_object;
              object_init_ex(&class_object, class_ce);
              /* After create the Object success, invoke the method with the `set` prefix. */
              zval *properties = zend_hash_find(Z_ARRVAL_P(val_value), strpprintf(0, "properties"));
              if ( properties ) {
                /* Do the invoke job */
                zend_string *key_name;
                zval *key_value;
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(properties), key_name, key_value) {
                  /* For each property do the `set` method */
                  if ( key_name ) {
                    zend_string *method_name = strpprintf(0, "set%s", ZSTR_VAL(key_name)); /* setName() */
                    /* Create the object value */
                    if ( key_value && CSPEED_STRING_NOT_EMPTY(Z_STRVAL_P(key_value)) && ( Z_TYPE_P(key_value) == IS_STRING ) ) {

                    zend_class_entry *class_ce_two;
nest_again:
                      if ( *(Z_STRVAL_P(key_value)) == '\\' ) {
                        class_ce_two = zend_hash_find_ptr(EG(class_table), zend_string_tolower(strpprintf(0, "%s", Z_STRVAL_P(key_value) + 1)));
                      } else {
                        class_ce_two = zend_hash_find_ptr(EG(class_table), zend_string_tolower(Z_STR_P(key_value)));
                      }
                      if ( !class_ce_two ) {
                          cspeed_autoload_file(Z_STR_P(key_value), app_object, CSPEED_APP_AUTOLOAD_ALIASES);
                          goto nest_again;
                      }
                      zval object_value;
                      object_init_ex(&object_value, class_ce_two);

                      /* Next do the method invoke */
                      if (CSPEED_METHOD_IN_OBJECT(&class_object, ZSTR_VAL(method_name))){
                          zval function_name, retval;
                          ZVAL_STRING(&function_name, ZSTR_VAL(method_name));
                          uint32_t param_count = 1;
                          zval params[] = {
                            object_value
                          };
                          call_user_function(NULL, &class_object, &function_name, &retval, param_count, params);
                          zval_ptr_dtor(&function_name);
                          zval_ptr_dtor(&retval);
                      }
                    }
                  }                
                } ZEND_HASH_FOREACH_END();
              }

              /* Value node */
              zval *values = zend_hash_find(Z_ARRVAL_P(val_value), strpprintf(0, "values"));
              if ( values ) {
                /* Do the invoke job */
                zend_string *key_name;
                zval *key_value;
                ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(values), key_name, key_value) {
                  if ( key_name ) {
                    if ( key_value ) {
                        int set_attr_or_not = 1;
                        zend_property_info *property_info = zend_hash_find_ptr(&class_ce->properties_info, key_name);
                        if ( property_info != NULL ) {
                            if ( (property_info->flags & ZEND_ACC_SHADOW) 
                                || (property_info->flags & ZEND_ACC_PUBLIC)
                                || (property_info->flags & ZEND_ACC_PRIVATE)
                                || (property_info->flags & ZEND_ACC_PROTECTED)
                                 ) {
                                /* Find the attr property */
                                zval *attrs = zend_hash_find(Z_ARRVAL_P(val_value), strpprintf(0, "attrs"));
                                if ( attrs && ( Z_TYPE_P(attrs) == IS_ARRAY ) ) {
                                    zend_string * attr_key;
                                    zval *attr_value;
                                    ZEND_HASH_FOREACH_STR_KEY_VAL( Z_ARRVAL_P(attrs), attr_key, attr_value) {
                                        if ( 
                                          (
                                            ( strncmp(ZSTR_VAL(attr_key), ("public"), strlen("public") ) == 0 )
                                             && (property_info->flags & ZEND_ACC_PUBLIC) )
                                          || ( 
                                            ( strncmp(ZSTR_VAL(attr_key), ("protected"), strlen("protected") ) == 0) 
                                             && (property_info->flags & ZEND_ACC_PROTECTED ) )
                                          || (
                                             ( (strncmp(ZSTR_VAL(attr_key), ("private"), strlen("private") ) == 0) 
                                             && (property_info->flags & ZEND_ACC_PRIVATE ) )
                                          )
                                        ) {
                                            if ( (Z_TYPE_INFO_P(attr_value) == IS_FALSE) ) {
                                                set_attr_or_not = 0;
                                                break;
                                            } else {
                                                set_attr_or_not = 1;
                                                break;
                                            }
                                        }
                                    } ZEND_HASH_FOREACH_END();
                                }
                            }
                        } else {
                            if (Z_TYPE(class_object) != IS_UNDEF && Z_OBJ_HANDLER(class_object, has_property)) {
                              zval property;
                              ZVAL_STR_COPY(&property, ZSTR_VAL(key_name));
                              if (!Z_OBJ_HANDLER(class_object, has_property)(&class_object, &property, 2, NULL)) {
                                zval_ptr_dtor(&property);
                                continue;
                              }
                              set_attr_or_not = 1;
                              zval_ptr_dtor(&property);
                            }
                        }
                        /* if exists, udpate the value */
                        if ( set_attr_or_not ){
                            zend_update_property(class_ce, &class_object, CSPEED_STRL(ZSTR_VAL(key_name)), key_value);
                        }                 
                    }
                  }                
                } ZEND_HASH_FOREACH_END();
              }
              /* Add to the IOC stores */
              zval *stores = zend_read_property(cspeed_object_factory_ce, getThis(), CSPEED_STRL(OBJECT_FACTORY_STORE), 1, NULL);
              Z_TRY_ADDREF(class_object);
              add_assoc_zval(stores, ZSTR_VAL(val_name), &class_object);
            } else {
              php_error_docref(NULL, E_ERROR, "Class `%s` not found.", Z_STRVAL_P(class_name));
            }
          }
        }
    } ZEND_HASH_FOREACH_END();
    RETURN_TRUE;
  }
  RETURN_FALSE;
}

CSPEED_METHOD(ObjectFactory, getObject)
{
    zend_string *object_id;
    if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S", &object_id) == FAILURE ) {
        return ;
    }
    if (CSPEED_STRING_NOT_EMPTY(ZSTR_VAL(object_id))) {
        zval *objects = zend_read_property(cspeed_object_factory_ce, getThis(), CSPEED_STRL(OBJECT_FACTORY_STORE), 1, NULL);
        if ( objects ) {
            zval *object_value = zend_hash_find( Z_ARRVAL_P(objects), object_id );
            RETURN_ZVAL(object_value, 1, 0);
        } else {
            RETURN_NULL();
        }
    }
}

static const zend_function_entry cspeed_object_factory_functions[] = {
  CSPEED_ME(ObjectFactory, init, arg_info_objectf_init, ZEND_ACC_PUBLIC)
  CSPEED_ME(ObjectFactory, __construct, arg_info_objectf_construct, ZEND_ACC_PUBLIC)
  CSPEED_ME(ObjectFactory, getObject, arg_info_objectf_get_object, ZEND_ACC_PUBLIC)

  PHP_FE_END
};

CSPEED_INIT(object_factory) /*{{{ */
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Cs", "ObjectFactory", cspeed_object_factory_functions);
    cspeed_object_factory_ce = zend_register_internal_class(&ce);

    /* The properties */
    zend_declare_property_null(cspeed_object_factory_ce, CSPEED_STRL(OBJECT_FACTORY_STORE), ZEND_ACC_PRIVATE);
}/*}}}*/










