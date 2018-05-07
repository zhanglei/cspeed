<?php

/**
 * Class BootInit
 *
 * The CSpeed's bootstrap file, Each bootstrap class must implements the interface Cs\BootInit
 * as a reservable interface to the engine job.
 *
 * The class name BootInit can be set in the INI config file.
 */

class BootInit implements \Cs\BootInit
{
    /**
     * @param $di \Cs\di\Di
     * @param $router \Cs\mvc\Router
     */
    function __initRouter($di, $router)
    {
        $di->set('view', function(){
            return new \Cs\mvc\View();
        });
        $router->add(
            '/back/:action:/:id:',
            '/shop/list/$1'
        );
        $router->add(
            '/shop/:controller:/:action:/:any:',
            '/get/\1/\2/\3'
        );
        $router->addFromIni('../app/router.ini');
    }

    /**
     * @param $di \Cs\di\Di
     * @param $router \Cs\mvc\Router
     */
    function __InitDb($di, $router)
    {
        $di->set('db', function(){
            return new \Cs\db\pdo\Adapter();
        });

        $di->set('slave', function(){
            return new \Cs\db\pdo\Adapter([
                'dsn' => 'mysql:host=localhost;port=3306;dbname=slave',
                'username' => 'root',
                'password' => '3333'
            ]);
        });
    }

    /**
     * @param $di \Cs\di\Di
     * @param $router \Cs\mvc\Router
     */
    function __initIOC($di, $router)
    {
        $di->set('factory', function(){
            return new \Cs\ObjectFactory("config.php");
        });
    }
}






