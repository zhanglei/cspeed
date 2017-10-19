<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-19
 * Time: 下午8:45
 */

namespace Cs\mvc;

class Model
{
    /**
     * Model constructor.
     */
    public function __construct()
    {
    }

    /**
     * Use the magic to realise the ActiveRecord
     * @param $name     The field which you want to map with the SQL field
     * @param $value    The value which you want to store to the DB engine
     */
    public function __set($name, $value)
    {

    }

    /**
     * To realise the Update feature
     * @return Model
     */
    static function find() : Model
    {
    }

    /**
     * Setting the table which you want to operate
     * NOTE TAHT:
     *  You must return the database table name, if you want to operate the other table not the model name table
     */
    function tableName()
    {

    }

    /**
     * Do the WHERE operation
     * @param array $where
     * @return $this
     */
    function where(array $where)
    {
        return $this;
    }

    /**
     * Do the APPEND WHERE
     * @param array $where
     * @return $this
     */
    function andWhere(array $where)
    {
        return $this;
    }

    /**
     * ORDER BY
     * @param array $orderBy
     * @return $this
     */
    function orderBy(array $orderBy)
    {
       return $this;
    }

    /**
     * GROUP BY
     * @param array $groupBy
     * @return $this
     */
    function groupBy(array $groupBy)
    {
        return $this;
    }

    /**
     * ONE record in the ResultSet
     */
    function one()
    {

    }

    /**
     * ALL record in the ResultSet
     */
    function all()
    {

    }

    /**
     * DO THE UPDATE|INSERT operation
     */
    function save()
    {

    }

    /**
     * DO THE DELETE operation
     */
    function delete()
    {

    }
}