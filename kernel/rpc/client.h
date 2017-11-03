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

#ifndef CSPEED_RPC_CLIENT_H
#define CSPEED_RPC_CLIENT_H

zend_class_entry *cspeed_rpc_client_ce;                               /* The RPC Client class */

#define CSPEED_CLIENT_URL                    "__url"                  /* The URL */
#define CSPEED_CLIENT_POST_DATA              "__data"                 /* The value */
#define CSPEED_CLIENT_POST_ID                "__id"                   /* The JSON-RPC id */

CSPEED_INIT(client);                                                  /* The RPC Client init function */

#endif