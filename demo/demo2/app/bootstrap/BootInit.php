<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 2018/4/9
 * Time: 10:44
 */

class BootInit implements \Cs\BootInit
{
    /**
     * @param $di       Di
     * @param $router   Router
     */
    function __initDb($di, $router)
    {
        $di->set('db', function(){
            return new Adapter([
                'dsn'      => 'mysql:host=localhost;port=3306;dbname=supjos',
                'username' => 'root',
                'password' => '3333'
            ]);
        });
    }

    /**
     * @param $di       Di
     * @param $router   Router
     */
    function __initRouter($di, $router)
    {
        $router->addFromArray([
            '/home/:controller:/:action:' => '/get/$1/$2',
            '/index/:any:'                => '/index/world/$1'
        ]);
    }

    /**
     * @param $di       Di
     * @param $router   Router
     */
    function __initFactory($di, $router)
    {
        $di->set('factory', function (){
            return new ObjectFactory('factory.php');
        });
    }
}