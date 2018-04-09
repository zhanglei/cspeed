<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 2018/4/9
 * Time: 10:49
 */
 
namespace app\modules\frontend\controllers;

use Cs\mvc\Controller;

class Index extends Controller
{
    /**
     * Initialise method to do the initialise job.
     */
    function initialise()
    {
        $this->on(self::EVENT_BEFORE_ACTION, function($objectIndex){
            // TODO: Add the action-before code
            echo 'before indexAction';
        });

        $this->on(self::EVENT_AFTER_ACTION, function ($objectIndex){
            // TODO: Add the action-after code
            echo 'after indexAction';
        });
    }

    /*------Begin URL action ------*/

    function indexAction()
    {
        echo 'Hello World';
        $this->view->render('index');
    }

    /*------End URL action --------*/
}