#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cspeed.h"

/* The class entry in the CSpeed */
zend_class_entry *cspeed_adapter_ce;
zend_class_entry *cspeed_task_ce;
zend_class_entry *cspeed_model_interface_ce;
zend_class_entry *cspeed_controller_ce;
zend_class_entry *cspeed_model_ce;
zend_class_entry *cspeed_router_ce;
zend_class_entry *cspeed_view_ce;
zend_class_entry *cspeed_request_ce;
zend_class_entry *cspeed_response_ce;
zend_class_entry *cspeed_rpc_client_ce;
zend_class_entry *cspeed_rpc_server_ce;
zend_class_entry *cspeed_app_ce;
zend_class_entry *cspeed_component_ce;
zend_class_entry *cspeed_config_ce;
zend_class_entry *cspeed_di_ce;
zend_class_entry *cspeed_bootinit_ce;