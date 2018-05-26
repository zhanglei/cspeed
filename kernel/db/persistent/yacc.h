/**
 * The Bison & Flex structs or other types  
 * which will be used in the SQL grammer analyse job.
 */
struct _select_statement
{
	char *select;
	char *fields;
	char *from;
	char *table_name;
	char *where_opts;
	char *groupby_opts;
	char *orderby_opts;
	char *limit_opts;
};

struct _update_statement
{
	char *update;
	char *table_name;
	char *set;
	char *update_opts;
	char *where_opts;
};

struct _delete_statement
{
	char *delete;
	char *from;
	char *table_name;
	char *where_opts;
};

struct _insert_statement
{
	char *insert_into;
	char *table_name;
	char *fields;
	char *values;
};

typedef struct _SQL_PARSER_RESULT
{
    /** The result of the SQL parsing result. */
	char *sql_result;
    
    /* 1: SELECT 2: UPDATE 3: DELETE 4: INSERT INTO, >=1064: SQL grammer error. */
	int  sql_type;

    /* JOIN Query or not. */
	int  join_query;

    /* The select query statement */
	struct _select_statement select_statement;

    /* The update statement */
	struct _update_statement update_statement;

    /* The delete statement */
	struct _delete_statement delete_statement;

    /* The insert into statement */
	struct _insert_statement insert_statement;
} SQL_PARSER_RESULT;

