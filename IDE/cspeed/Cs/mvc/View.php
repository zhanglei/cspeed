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
     * @param $templateFileName string          Which the file you want to render
     * @param $variables        array|mixed     The variables which you may use in template
     */
    function render($templateFileName, $variables = [])
    {

    }

    /**
     * Setting the suffix name, The default suffix is "phtml"
     * @param $suffixName  string  Which suffix you want to use
     */
    function setSuffix($suffixName)
    {

    }

    /**
     * Add one variables to the View template file
     * @param $varName    string  The var name you use in the view template file to equals the controller's $varValue
     * @param $varValue   mixed
     */
    function setVar($varName, $varValue)
    {
    }

    /**
     * Return the view template file rendering result
     * @param $templateFileName string
     * @param $variables        string|array|mixed
     */
    function getRender($templateFileName, $variables = [])
    {
    }

    /**
     * Set the View Dir, The default value is '../views'
     * @param $viewDir string
     */
    function setViewDir($viewDir)
    {
    }

    /**
     * As same as the render function, and will changed in the kernel C code, but the PHP level work usually as before
     * @param $templateFileName string
     * @param $variables        string|array|mixed
     */
    function partial($templateFileName, $variables = [])
    {
    }
}