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

#ifndef CSPEED_MODEL_H
#define CSPEED_MODEL_H

zend_class_entry *cspeed_model_ce;                                      /* The Model class */

#define CSPEED_MODEL_MAGIC_DATAS        "__cspeed_model_magic_datas"    /* The Model magic datas */
#define CSPEED_MODEL_PDO_OBJECT         "__cspeed_model_pdo_object"     /* The PDO object */
#define CSPEED_MODEL_NEW_RECORD         "__cspeed_model_new_record"     /* Whether the record is new or not */
#define CSPEED_MODEL_WHERE_COND         "__cspeed_model_where_cond"     /* Where conditions */
#define CSPEED_MODEL_ORDER_BY           "__cspeed_model_order_by"       /* Order By */
#define CSPEED_MODEL_GROUP_BY           "__cspeed_model_group_by"       /* Group By */
#define CSPEED_MODEL_TABLE_NAME         "__cspeed_model_table_name"     /* Model table name */

CSPEED_INIT(model);                                                     /* model_init() */

#endif