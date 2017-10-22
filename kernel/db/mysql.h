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

#ifndef CSPEED_DB_MYSQL_H
#define CSPEED_DB_MYSQL_H

zend_class_entry *cspeed_mysql_ce;

#define CSPEED_MYSQL_PDO_OBJECT       "__cspeed_mysql_pdo"      /* The mysql's pdo object */

#define CSPEED_MYSQL_SELECT           "__cspeed_mysql_select"   /* The select fields */
#define CSPEED_MYSQL_FROM             "__cspeed_mysql_from"     /* The from fields */
#define CSPEED_MYSQL_WHERE            "__cspeed_mysql_where"    /* The where fields */
#define CSPEED_MYSQL_GROUP_BY         "__cspeed_mysql_group_by" /* Group By */
#define CSPEED_MYSQL_ORDER_BY         "__cspeed_mysql_order_by" /* Order By */
#define CSPEED_MYSQL_HAVING           "__cspeed_mysql_having"   /* Having */
#define CSPEED_MYSQL_LIMIT            "__cspeed_mysql_limit"    /* Limit */
#define CSPEED_MYSQL_RAW_SQL          "__cspeed_mysql_raw_sql"  /* Raw Sql */
#define CSPEED_MYSQL_BIND_PARAMS      "__cspeed_mysql_bind_params"/* Raw Sql */

zend_bool output_sql_errors(zval *pdo_statement);               /* Output the SQL error */

CSPEED_INIT(mysql);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
