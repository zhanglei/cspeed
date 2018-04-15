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
  | Author: Josin<774542602@qq.com|www.supjos.cn>                        |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef CSPEED_DB_H
#define CSPEED_DB_H

/* To connect the PDO SQL */
void cspeed_pdo_construct(zval *pdo_object, char *dsn, char *username, char *password, zval *options);

/* To let the current PDO connection begin a transaction */
void cspeed_pdo_begin_transaction(zval *pdo_object, zval *retval);

/* Commit the current transaction */
void cspeed_pdo_commit(zval *pdo_object, zval *retval);

/* Rollback the current transaction */
void cspeed_pdo_roll_back(zval *pdo_object, zval *retval);

/* Commit the current sql error code */
void cspeed_pdo_error_code(zval *pdostatement_obj, zval *retval);

/* Commit the current sql error info */
void cspeed_pdo_error_info(zval *pdostatement_obj, zval *retval);

/* Exec the current sql */
void cspeed_pdo_exec(zval *pdo_object, char *sql, zval *retval);

/* Check the current pdo is in transaction or not */
void cspeed_pdo_in_transaction(zval *pdo_object, zval *retval);

/* Get the current insert id name */
void cspeed_pdo_last_insert_id(zval *pdo_object, char *name, zval *retval);

/* Get a prepared SQLSTATEMENT */
void cspeed_pdo_prepare(zval *pdo_object, char *sql, zval *retval);


/* The flowing is the PDOStatement functions */
/* Binding the params */
void cspeed_pdo_statement_bind_param(zval *pdostatement_obj, zval *retval);

/* Fetch one record from the result */
void cspeed_pdo_statement_fetch(zval *pdostatement_obj, zval *retval);

/* Fetch all satisfied result from the result */
void cspeed_pdo_statement_fetch_all(zval *pdostatement_obj, zval *retval);

/* Fetch the result as the format of Object stdClass */
void cspeed_pdo_statement_fetch_object(zval *pdostatement_obj, zval *retval);

/* Return the row count of the effected */
void cspeed_pdo_statement_row_count(zval *pdostatement_obj, zval *retval);

/* Set the fetch mode */
void cspeed_pdo_statement_set_fetch_mode(zval *pdostatement_obj, int fetch_style, zval *retval);


#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
