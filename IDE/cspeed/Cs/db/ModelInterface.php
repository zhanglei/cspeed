<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午7:40
 */

namespace Cs\db;


interface ModelInterface
{
    /**
     * SELECT
     * @param array|string $fields   The fields which you want to SELECT from the DB
     * @return \Cs\db\ModelInterface
     */
    function select($fields);

    /**
     * FROM
     * @param string $table    The table which you want to Query data
     * @return ModelInterface
     */
    function from($table);

    /**
     * WHERE
     * @param array|string $whereCondition    The WHERE condition you may filter data
     * @return ModelInterface
     */
    function where( $whereCondition);

    /**
     * GROUP BY
     * @param array|string $groupBy          Which columns you want to GROUP BY
     * @return ModelInterface
     */
    function groupBy( $groupBy);

    /**
     * HAVING
     * @param array|string $having           THE HAVING conditon you may want to filter
     * @return ModelInterface
     */
    function having($having);

    /**
     * ORDER BY
     * @param array|string $orderBy           The ORDER BY condition
     * @return ModelInterface
     */
    function orderBy($orderBy);

    /**
     * LIMIT
     * @param int $count        The number of the data you want to GET
     * @param int $offset       The offset which the data to obtain
     * @return ModelInterface
     */
    function limit($count, $offset);

    /**
     * @param string $rawSql        The raw SQL you want to Query|Execute
     * @param array $bindParams     The binding parameters you want to bind to the SQL
     * @return ModelInterface
     */
    function createCommand($rawSql, array $bindParams);

    /**
     * Execute the SQL and return the result of the SQL execution data-sets
     * @return mixed
     */
    function execute();
}