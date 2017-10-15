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
     * @param array $fields   The fields which you want to SELECT from the DB
     * @return \Cs\db\ModelInterface
     */
    function select(array $fields);

    /**
     * FROM
     * @param string $table    The table which you want to Query data
     * @return ModelInterface
     */
    function from($table);

    /**
     * WHERE
     * @param array $whereCondition    The WHERE condition you may filter data
     * @return ModelInterface
     */
    function where(array $whereCondition);

    /**
     * GROUP BY
     * @param array $groupBy          Which columns you want to GROUP BY
     * @return ModelInterface
     */
    function groupBy(array $groupBy);

    /**
     * HAVING
     * @param array $having           THE HAVING conditon you may want to filter
     * @return ModelInterface
     */
    function having(array $having);

    /**
     * ORDER BY
     * @param array $orderBy           The ORDER BY condition
     * @return ModelInterface
     */
    function orderBy(array $orderBy);

    /**
     * LIMIT
     * @param int $count        The number of the data you want to GET
     * @param int $offset       The offset which the data to obtain
     * @return ModelInterface
     */
    function limit($count, $offset);

    /**
     * @param $rawSql               The raw SQL you want to Query|Execute
     * @param array $bindParams     The binding parameters you want to bind to the SQL
     * @return ModelInterface
     */
    function query($rawSql, array $bindParams);

    /**
     * Execute the SQL and return the result of the SQL execution data-sets
     * @return mixed
     */
    function execute();
}