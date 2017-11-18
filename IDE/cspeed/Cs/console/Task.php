<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 17-11-18
 * Time: 下午1:55
 */

namespace Cs\console;

class Task
{
    /**
     * Task constructor.
     * @param string $iniFilePath       The INI file which you want to load to replace the default value
     * @param string $iniNodeName       The INI file node you may to used
     */
    function __construct($iniFilePath = "", $iniNodeName = "")
    {
    }

    /**
     * To dispatch the URL
     * @param string $routePath The Router which you want to CLI CSpeed to load the Module & Controller & Action
     */
    function run($routePath)
    {
    }
}