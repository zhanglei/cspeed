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
     * Dispatch the URL without redirect.
     * May used in the APIs.
     * [NOTE]: when in dispatching, current request code behind the dispatch operation will be discarded.
     * @param $url string To dispatch the url without redirection.
     *                    $url contains with 3 parts at least and parameters may be add to the URL too.
     *                    such as ``` dispatch('/stores/index/do');``` or ```dispatch(/stores/index/do/id/1')```
     */
    public function dispatch($url)
    {
    }
}