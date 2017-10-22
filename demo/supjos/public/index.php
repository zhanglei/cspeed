<?php

/**
 *  CSpeed框架开发ＷＥＢ应用的一个典型示例，示例中的每一个代码文件夹都经过精心的设计
 * 可以从本ＤＥＭＯ文件中得到CSpeed框架开发一个WEB应用的方法
 * Josin ２０１７年１０月２２日１６：３０：１６｜www.supjos.cn
 */


$di = new \Cs\di\Di();

/**
 * 如果您需要使用数据库功能，请配置好相应的参数信息，并且删除注释
 */
/*$di->set('db', function(){
    return new \Cs\db\pdo\MySql([
        'dsn'      =>       'mysql:host=localhost;dbname=supjos',
        'username' =>       'root',
        'password' =>       '3333'
    ]);
});*/

/* 配置通用的配置信息 */
$di->set('mainConfig', function(){
    $config = new \Cs\tool\Config();
    $config->loadConfig('./cspeed.ini');
    return $config;
});

/* 设置独立的View渲染引擎，注意这里设置的View引擎信息，并不会被默认的ＵＲＬ处理机制使用，而是设置后，用户可以自行调用 */
$di->set('view', function(){
    $view = new \Cs\mvc\View();
    $view->setModuleDir('index');
    return $view;
});

$app = new \Cs\App($di);

/* 注册一个backend后台模块, ../backend是后台模块的路径 */
$app->registerModules([
    'backend' => '../backend'
]);

$app->run();