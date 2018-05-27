/**
 * The Bison & Flex structs or other types  
 * which will be used in the SQL grammer analyse job.
 */
struct _select_statement
{
	int field_as;
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
	char *sharding_key_value;
	char *update;
	char *table_name;
	char *set;
	char *update_opts;
	char *where_opts;
};

struct _delete_statement
{
	char *sharding_key_value;
	char *delete;
	char *from;
	char *table_name;
	char *where_opts;
};

struct _insert_statement
{
	char *sharding_key_value;
	char *insert_into;
	char *table_name;
	char *fields;
	char *values;
};

typedef struct _SQL_PARSER_RESULT
{
	char *sql_result;
	int  sql_type; 			/* 1: SELECT 2: UPDATE 3: DELETE 4: INSERT INTO */
	int  join_query; 		/* JOIN Query or not. */
	struct _select_statement select_statement;
	struct _update_statement update_statement;
	struct _delete_statement delete_statement;
	struct _insert_statement insert_statement;
} SQL_PARSER_RESULT;

