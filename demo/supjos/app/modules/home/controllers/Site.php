<?php

class Site extends \Cs\mvc\Controller
{
    /**
     * 
     */
    function __beforeAction()
    {
        echo "Before<br>";
    }

    /**
     * 
     */
    function indexAction()
    {
        echo "Doing<br>";
    }

    /**
     * 
     */
    function __afterAction()
    {
        echo "After<br>";
    }

}