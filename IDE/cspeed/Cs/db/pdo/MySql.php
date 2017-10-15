<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午7:50
 */

namespace Cs\db\pdo;

use Cs\db\ModelInterface;

class MySql implements ModelInterface
{
    /**
     * MySql constructor.
     * @param array $connectionOptions  The parameter must container `dsn` | `username` | `password` and  a optional option key
     */
    function __construct(array $connectionOptions)
    {
    }

    /**
     * @param array $fields
     * @return $this|MySql
     */
    function select(array $fields)
    {
        return $this;
    }

    /**
     * @param string $table
     * @return $this|MySql
     */
    function from($table)
    {
        return $this;
    }

    /**
     * @param array $whereCondition
     * @return $this
     */
    function where(array $whereCondition)
    {
        return $this;
    }

    /**
     * @param array $groupBy
     * @return $this
     */
    function groupBy(array $groupBy)
    {
        return $this;
    }

    /**
     * @param array $having
     * @return $this|MySql
     */
    function having(array $having)
    {
        return $this;
    }

    /**
     * @param array $orderBy
     * @return $this|MySql
     */
    function orderBy(array $orderBy)
    {
        return $this;
    }

    /**
     * @param int $count
     * @param int $offset
     * @return $this|MySql
     */
    function limit($count, $offset)
    {
        return $this;
    }

    /**
     * @param \Cs\db\The $rawSql
     * @param array $bindParams
     * @return $this|MySql
     */
    function query($rawSql, array $bindParams)
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
     * @param $whereCondition    The Where condition which can be add the where condition to the current WHERE condition
     * @return $this|MySql
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
     * Return the effected row count
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