<?php

/**
 * This is the Index controller
 */

namespace app\modules\index\controllers;

use Cs\mvc\View;

class Index extends \Cs\mvc\Controller
{
    /**
     * To view the affect, enter the url in web browser:
     *
     * http://xxx.com/index/index/index
     *
     * or
     *
     * http://xxx.com
     */
	function indexAction()
	{
        /**
         * @var $view View
         */
        $view = $this->di->get('view');

        $view->setVar('name', "您好 CSpeed 用户 ");

        $view->render('index');
	}

    /**
     * To run the method in web browser, enter:
     *
     * http://xxx.com/index/index/view.html
     *
     */
	function viewAction()
    {
        echo '<h1>Hello World</h1>';
    }
}