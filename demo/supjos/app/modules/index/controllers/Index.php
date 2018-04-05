<?php

namespace app\modules\index\controllers;

class Index extends \Cs\mvc\Controller
{
	function indexAction()
	{
	    $user = new \app\models\User();

	    echo '<pre>';
	    print_r($user);
	    $data = $user->one();
//
//	    echo '<pre>';
//	    print_r($data);

//        $client = new \Cs\rpc\Client("http://www.supjos.com/index/c");
//
//        $result = $client->list("aa", "bb");
//
//        $two = $client->two("hello");
//        echo '<pre>';
//        print_r($result);
//        print_r($two);
//        echo '</pre>';
	}
}