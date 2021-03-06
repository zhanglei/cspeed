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

extern zend_class_entry *cspeed_model_ce;                               /* The Model class */

#define CSPEED_MODEL_MAGIC_DATAS        "__magic_datas"                 /* The Model magic datas */
#define CSPEED_MODEL_PDO_OBJECT         "__pdo_object"                  /* The PDO object */
#define CSPEED_MODEL_NEW_RECORD         "__new_record"                  /* Whether the record is new or not */
#define CSPEED_MODEL_SELECT             "__select"                      /* The SELECT */
#define CSPEED_MODEL_WHERE_COND         "__where_cond"                  /* WHERE conditions */
#define CSPEED_MODEL_ORDER_BY           "__order_by"                    /* ORDER BY */
#define CSPEED_MODEL_GROUP_BY           "__group_by"                    /* GROUP BY */
#define CSPEED_MODEL_TABLE_NAME         "__table_name"                  /* Model table name */
#define CSPEED_MODEL_ULTIMATE_PDO       "__ultimate_pdo"                /* The finaly pdo object to do the job */

#define CSPEED_MODEL_ADAPTER            "__adapter"                     /* The current adapter related to this model */
#define CSPEED_MODEL_RESULT             "__result"                      /* Columns Details */
#define CSPEED_MODEL_COLUMNS            "__columns"                     /* Columns */
#define CSPEED_MODEL_PRIMARY            "__primary_key"                 /* The primary key */
#define CSPEED_MODEL_EXECUTED_SQL       "__executed_sql"                /* The executed SQL */
#define CSPEED_MODEL_ARRAY_DATA         "__array_data"                  /* The array data */
#define CSPEED_MODEL_AS_ARRAY           "__as_array"                    /* The array data */

#define EVENT_BEFORE_SAVE               "EVENT_BEFORE_SAVE"             /* BEFORE_SAVE */
#define EVENT_AFTER_SAVE                "EVENT_AFTER_SAVE"              /* AFTER_SAVE */
#define EVENT_BEFORE_DELETE             "EVENT_BEFORE_DELETE"           /* BEFORE_DELETE */
#define EVENT_AFTER_DELETE              "EVENT_AFTER_DELETE"            /* AFTER_DELETE */

CSPEED_INIT(model);                                                     /* model_init() */

#endif



/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

