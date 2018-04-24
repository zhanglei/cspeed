<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午7:59
 */

namespace Cs\di;

class Di
{
    /**
     * Di constructor.
     */
    function __construct()
    {
    }

    /**
     * @param  string $key      Which object-instance you want to get from the Di container
     * @return object|mixed
     */
    function get($key = '')
    {

    }

    /**
     * @param $key     string    The key to identify the object|mixed
     * @param $closure \Closure     The closure which to return the object|mixed
     */
    function set($key, $closure)
    {

    }
}