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

#ifndef CSPEED_VIEW_H
#define CSPEED_VIEW_H

extern zend_class_entry *cspeed_view_ce;                                         /* The class View */

void initialise_view_object_properties(zval *view_object);                /* Initialise the need properties */

void render_view_file(zval *, zend_string *, zval *, zval *);             /* Render the view file */

#define CSPEED_VIEW_DIRS_V                    "views"                     /* The view directory */
#define CSPEED_VIEW_CONTROLLER_MODULE_ID      "module_id"                 /* The current Module name */
#define CSPEED_VIEW_CONTROLLER_CONTROLLER_ID  "controller_id"             /* The current Controller name */
#define CSPEED_VIEW_CONTROLLER_ACTION_ID      "action_id"                 /* The current Action Id */
#define CSPEED_VIEW_DIRS                      "__dir"         /* The view directory to find the view file */
#define CSPEED_VIEW_SUFFIX                    "__suffix"      /* The view template file's suffix */
#define CSPEED_VIEW_VARIABLES                 "__variables"   /* The variables the view has */
#define CSPEED_VIEW_ROOT_DIR                  "__root_dir"    /* The parent root dir of the view file location */
CSPEED_INIT(view);                                                        /* Init function declare */

#endif










/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
