<?php

namespace app\modules\home\controllers;

class Index extends \Cs\mvc\Controller
{
	function indexAction()
	{
		$this->view->setVar('hello', 'CSpeed');
		$this->view->render('index/index', ['version'=>'1.5.0']);
	}
}