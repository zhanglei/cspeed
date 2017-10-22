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

#ifndef CSPEED_APP_H
#define CSPEED_APP_H

zend_class_entry *cspeed_app_ce;                                /* The global CApp class entry */

#define CSPEED_APP_AUTOLOAD_ALIASES   "__cspeed_app_aliases"    /* The aliases which let the system to locate the file */

#define CSPEED_APP_MODULES            "__cspeed_app_modules"    /* All modules, which cspeed to dispatch the URL */
#define CSPEED_APP_DI_OBJECT          "__cspeed_app_di_object"  /* The di object which passed to App class */
#define CSPEED_APP_PROPERTY           "app"                     /* The app property */

CSPEED_INIT(app);                                               /* The init function for the CApp class */


#endif









/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */




