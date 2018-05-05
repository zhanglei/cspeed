<?php

class BootInit implements \Cs\BootInit
{
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

    function __initIOC($di, $router)
    {
        $di->set('factory', function(){
            return new \Cs\ObjectFactory("config.php");
        });
    }
}






