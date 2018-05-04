<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午8:01
 */

namespace Cs\mvc;

class Controller extends \Cs\tool\Component
{
    /**
     * Some events the Controller supports
     */
    const EVENT_BEFORE_ACTION = "EVENT_BEFORE_ACTION";
    const EVENT_AFTER_ACTION  = "EVENT_AFTER_ACTION";

    /**
     * @var $di \Cs\di\Di
     */
    public $di;

    /**
     * @var $view \Cs\mvc\View
     * [NOTE] $view object only exists when using the auto-rendering mode. otherwise null set.
     */
    public $view;

    /**
     * @var $router \Cs\mvc\Router
     */
    public $router;

    /**
     * @var $url string The router url to dispatch, contains with 3 parts: 
     * module/controller/action,  Get parameters in url path-infoss
     */
    function dispatch($url)
    {
    }
}