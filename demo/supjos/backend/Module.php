<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-22
 * Time: 下午4:40
 */

class Module
{
    function initialise()
    {
        /* In this function, you can do the initialise job for the backend module */
        /* To access the global Di object and the variable-object, you can use the
         * \Cs\App::$app property to access
         * /
         * 您可以在本方法内进行后台模块backend的模块初始化工作，如果不需要初始化的话，请留空
         * 可以使用 \Cs\App::$app 方法来获取入口文件处设置的Ｄｉ对象或者新增对象至后台模块容器:
         * \Cs\App::$app->set('backendConfig', function(){
         *      $config = new \Cs\tool\Config();
         *      $config->loadConfig('../backend/config/backend.ini');
         *      return $config;
         * });
         */
    }
}