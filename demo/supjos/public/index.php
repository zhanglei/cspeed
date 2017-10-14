<?php

$di = new \Cs\di\Di();

$di->set('db', function(){
    return new \Cs\db\pdo\MySql([
        'dsn'      =>       'mysql:host=localhost;dbname=supjos',
        'username' =>       'root',
        'password' =>       '3333'
    ]);
});

$di->set('view', function(){
    $view = new \Cs\mvc\View();
    $view->setModuleDir('index');
    return $view;
});

$app = new \Cs\App($di);

$app->run();