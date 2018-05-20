
/**
 * The Current SQL parseing result
 */
typedef struct _SQL_PARSER_RESULT
{
    char *sql_result;
    int  sql_type; /* 1: SELECT 2: UPDATE 3: DELETE 4: INSERT INTO */
} SQL_PARSER_RESULT;


