<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午7:50
 */

namespace Cs\db\pdo;

use Cs\db\ModelInterface;

class Adapter implements ModelInterface
{
    /**
     * MySql constructor.
     * @param array $connectionOptions  The Official PDO optional config such as \PDO::ATTR_PERSISTENT and so on.
     */
    function __construct(array $connectionOptions = [])
    {
    }

    /**
     * @param array|string $fields
     * @return $this|Adapter
     */
    function select($fields)
    {
        return $this;
    }

    /**
     * @param string $table
     * @return $this|Adapter
     */
    function from($table)
    {
        return $this;
    }

    /**
     * @param array|string $whereCondition
     * @return $this
     */
    function where( $whereCondition)
    {
        return $this;
    }

    /**
     * @param array|string $groupBy
     * @return $this
     */
    function groupBy($groupBy)
    {
        return $this;
    }

    /**
     * @param array|string $having
     * @return $this|Adapter
     */
    function having($having)
    {
        return $this;
    }

    /**
     * @param array|string $orderBy, such as orderBy(['id DESC', 't ASC'])
     * @return $this|Adapter
     */
    function orderBy($orderBy)
    {
        return $this;
    }

    /**
     * @param int $count
     * @param int $offset
     * @return $this|Adapter
     */
    function limit($count, $offset)
    {
        return $this;
    }

    /**
     * @param string $rawSql
     * @param array $bindParams
     * @return $this|Adapter
     */
    function createCommand($rawSql, $bindParams = [])
    {
        return $this;
    }

    /**
     * Return the execution result
     * @return mixed
     */
    function execute()
    {
    }

    /**
     * @param  array|string $whereCondition The Where condition which can be add the where condition to the current WHERE condition
     * @return $this|Adapter
     */
    function andWhere($whereCondition)
    {
       return $this;
    }

    /**
     * Begin a transaction
     */
    function begin()
    {
    }

    /**
     * Rollback the transaction to the previous status
     */
    function rollback()
    {
    }

    /**
     * Commit the current transaction
     */
    function commit()
    {
    }

    /**
     * Return the last insert id
     * @return mixed
     */
    function lastInsertId()
    {
    }

    /**
     * Return the adapter is in-transaction-mode
     * @return mixed
     */
    function isInTransaction()
    {
    }

    /**
     * Return the effected row counts
     */
    function rowCount()
    {
    }

    /**
     * Return one result from the database
     */
    function find()
    {
    }

    /**
     * Return all data-sets which satisfied the given conditon
     */
    function findAll()
    {
    }

}