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

#ifndef CSPEED_PERSISTENT_DB_ADAPTER_H
#define CSPEED_PERSISTENT_DB_ADAPTER_H

extern zend_class_entry *cspeed_db_adapter_ce;
extern SQL_PARSER_RESULT *parse_sql(char *sql_statement, char *table_name);

/* The sharedKey for table field */
#define SHARED_KEY          "_sharedKey"

/* The horizontal size of the tables */
#define HORIZONTAL_SIZE     "_hSize"

/* The tableName which you want to h-splice */
#define TABLE_NAME          "_tableName"

/* The SQL that the PDO will execute */
#define RAW_SQL             "_raw_sql"

/* SQL type */
#define RAW_SQL_TYPE        "_raw_sql_type"

CSPEED_INIT(db_adapter);

#endif  