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

#ifndef CSPEED_DB_ADAPTER_H
#define CSPEED_DB_ADAPTER_H

extern zend_class_entry *cspeed_adapter_ce;

#define CSPEED_DB_PDO_OBJECT            "__cspeed_db_pdo"               /* The Db's pdo object */
#define CSPEED_DB_THIS_PDO              "__cspeed_adapter_this_pdo"     /* The Db's pdo object attach to the Adapter */
#define CSPEED_DB_THIS_ADAPTER          "__cspeed_adapter"              /* The current Adapter object */

#define CSPEED_ADAPTER_SELECT           "__cspeed_adapter_select"       /* The select fields */
#define CSPEED_ADAPTER_FROM             "__cspeed_adapter_from"         /* The from fields */
#define CSPEED_ADAPTER_WHERE            "__cspeed_adapter_where"        /* The where fields */
#define CSPEED_ADAPTER_GROUP_BY         "__cspeed_adapter_group_by"     /* Group By */
#define CSPEED_ADAPTER_ORDER_BY         "__cspeed_adapter_order_by"     /* Order By */
#define CSPEED_ADAPTER_HAVING           "__cspeed_adapter_having"       /* Having */
#define CSPEED_ADAPTER_LIMIT            "__cspeed_adapter_limit"        /* Limit */
#define CSPEED_ADAPTER_RAW_SQL          "__cspeed_adapter_raw_sql"      /* Raw Sql */
#define CSPEED_ADAPTER_BIND_PARAMS      "__cspeed_adapter_bind_params"  /* Raw Sql */

zend_bool output_sql_errors(zval *pdo_statement);               /* Output the SQL error */

CSPEED_INIT(adapter);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
