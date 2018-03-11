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

#ifndef CSPEED_MVC_CONTROLLER_H
#define CSPEED_MVC_CONTROLLER_H

extern zend_class_entry *cspeed_controller_ce;

#define CSPEED_DI_INSTANCE      "di"                  /* To get the Di object */
#define CSPEED_VIEW_INSTANCE    "view"                /* To get the View object */
#define CSPEED_ROUTER_INSTANCE  "router"              /* To get the Router object */

#define EVENT_BEFORE_ACTION           "EVENT_BEFORE_ACTION"       /* Before Action */
#define EVENT_AFTER_ACTION            "EVENT_AFTER_ACTION"        /* After Action */

CSPEED_INIT(controller);

#endif


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
