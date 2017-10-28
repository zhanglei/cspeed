<?php

class Index extends \Cs\mvc\Controller
{
	function indexAction()
	{
        $view = new \Cs\mvc\View();
    	echo "<pre>";
        print_r($view->module_id);
        print_r($view->controller_id);
    	print_r($view->action_id);
    	print_r($_GET);
    	echo "</pre>";
		echo "Yes, ab'indexActoin";
        
        $view->render('b/index');
	}
}