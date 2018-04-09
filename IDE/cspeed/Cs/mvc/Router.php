<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 17-10-30
 * Time: 下午9:17
 */

namespace Cs\mvc;

class Router
{
    /**
     * Router constructor.
     */
    function __construct()
    {
    }

    /**
     * To add a Router URL
     * @param $destUrl string The URL to match the browser's PATH_INFO
     * @param $toUrl   string The URL after matching, to redirect
     */
    function add($destUrl, $toUrl)
    {
    }

    /**
     * Adding the Router URL from the INI file
     * @param $iniFile string The ini file name
     */
    function addFromIni($iniFile)
    {
    }

    /**
     * To add the router from the given array
     * @param $arrayRouters array The Router's which you want to add to
     */
    function addFromArray($arrayRouters)
    {
    }
}