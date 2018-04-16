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

/* The global CApp class entry */
extern zend_class_entry *cspeed_app_ce;

/* The aliases which let the system to locate the file */
#define CSPEED_APP_AUTOLOAD_ALIASES     "__autoload_alias"          

/* Single App object */
#define CSPEED_APP_INSTANCE             "__app"                         

/* Event */
#define CSPEED_APP_EVENT_BEORE_REQUEST  "EVENT_BEFORE_REQUEST"

/* Event */
#define CSPEED_APP_EVENT_AFTER_REQUEST  "EVENT_AFTER_REQUEST"

/* The init function for the CApp class */
CSPEED_INIT(app);                                                       

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */




