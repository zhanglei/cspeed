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

#ifndef CSPEED_MVC_ROUTER_H
#define CSPEED_MVC_ROUTER_H

extern zend_class_entry *cspeed_router_ce;                                           /* The \Cs\mvc\Router Class */

#define CSPEED_ROUTER_ALL_ROUTINES              "__routines"       /* All routines for the Router */

void initialise_router_object_properties(zval *router_object);                /* Initialise the Router propertes */

CSPEED_INIT(router);                                                          /* The module init class */

#endif