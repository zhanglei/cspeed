<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午8:02
 */

namespace Cs\mvc;

class View
{
    /**
     * View constructor.
     */
    function __construct()
    {
    }

    /**
     * Render the view file
     * @param $templateFileName     Which the file you want to render
     * @param $variables            The variables which you may use in template
     */
    function render($templateFileName, $variables)
    {

    }

    /**
     * Setting the suffix name, The default suffix is "phtml"
     * @param $suffixName           Which suffix you want to use
     */
    function setSuffix($suffixName)
    {

    }

    /**
     * Add one variables to the View template file
     * @param $varName              The var name you use in the view template file to equals the controller's $varValue
     * @param $varValue
     */
    function setVar($varName, $varValue)
    {

    }

    /**
     * Return the view template file rendering result
     * @param $templateFileName
     * @param $variables
     */
    function getRender($templateFileName, $variables)
    {

    }

    /**
     * Set the View Dir, The default value is '../views'
     * @param $viewDir
     */
    function setViewDir($viewDir)
    {

    }

    /**
     * Set the Module Dir
     * @param $moduleDir
     */
    function setModuleDir($moduleDir)
    {

    }

    /**
     * As same as the render function, and will changed in the kernel C code, but the PHP level work usually as before
     * @param $templateFileName
     * @param $variables
     */
    function partial($templateFileName, $variables)
    {

    }
}