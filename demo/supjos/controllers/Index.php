<?php

class Index extends \Cs\mvc\Controller
{

    function indexAction()
    {
        echo "<div style='text-align:center'><h1>Hello CSpeed</h1></div>";

        $this->view->render('index/index', ['version' => '22']);
    }
}