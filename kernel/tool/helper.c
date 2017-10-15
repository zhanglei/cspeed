/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
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
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_cspeed.h"
#include "kernel/tool/helper.h"

#include <string.h>

char *cspeed_get_cwd()                    /*{{{ Return the current directory */
{
    zval cwd, function_name;
    ZVAL_STRING(&function_name, "getcwd");
    call_user_function(CG(function_table), NULL, &function_name, &cwd, 0, NULL);
    return Z_STRVAL(cwd);
}/*}}}*/


char *cspeed_reverse_slash_char(char *src)/*{{{ proto Return the result of the reservsed slash char* */
{
    int i;
    for (i = 0; src[i] != '\0'; i++ ){
        if ( src[i] == '\\' ) {
            src[i] = '/';
        }
    }
    return src;
}/*}}}*/

char *title_upper_string(char *src) /*{{{ Upper case the first char */
{
    *src = toupper(src[0]);
    return src;
}/*}}}*/

char *title_lower_string(char *src) /*{{{ Upper case the first char */
{
    *src = tolower(src[0]);
    return src;
}/*}}}*/













/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
