<?php

namespace app\modules\index\controllers;

class C extends \Cs\mvc\Controller
//class C extends \Cs\rpc\Server
{

//    function initialise()
//    {
//        $this->handle($this);
//    }

    function listRpc($param)
    {
        if ($param['data'] == 22) {
            return ['status' => '0000', 'info' => 'SUCCESS'];
        }

        return $param;
    }

    function twoRpc($data)
    {
        return $data;
    }


	function indexAction()
	{
	    //$this->output($this->di);

        $user = new \app\models\User();

        //$user->setDb('db');

        $responseData = $user->one();

        $resp = new \Cs\net\Response();
        $resp->setJsonContent($responseData);
        $resp->send();
	}

	function output($data)
    {
        echo '<pre>';
        var_dump($data);
        echo '</pre>';
    }
}