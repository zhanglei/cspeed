<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-22
 * Time: 下午4:38
 */

class Index extends  \Cs\mvc\Controller
{
    function indexAction()
    {
        echo "<div style='text-align: center'><h1>Hello CSpeed backend module.</h1></div>";

        $this->view->setVar('cspeed', 'Hello CSpeed v1.2.8');
        $this->view->render('index/index', ['version' => 'v1.2.8']);
    }
}