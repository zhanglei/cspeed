# CSpeed v2.0.0 高性能扩展框架－只为追求极致的速度

----------

#### CSpeed v2.0.0 新架构 ####

**一个简单的WEB应用入口：**
	
        $app = new \Cs\App("../app/config/core.ini", "dev");
        
        $app->bootstrap()->run();
        
通过INI配置文件初始化框架
第一个参数为配置文件的相对路径：ZTS下路径必须是绝对路径
第二个参数为配置文件的节点，详细的配置文件见下方

        $app = new \Cs\App("../app/config/core.ini", "dev");
        
运行初始化然后开始进行URL解析:

        $app->bootstrap()->run();
        
如果不需要初始化过程的话，可以只运行 run 方法：

        $app->run();
        
**配置文件详细如下：**

	[core]
	core.application                = '../app'               ; WEB目录
	core.bootstrap                  = '../app/bootstrap.php' ; 指定bootstrap 类目录
	core.bootstrap.method.string    = '__init'               ; 指定Bootstrap类的初始化方法的前缀 
	core.router.modules             =  index,home            ; 注册多模块
	core.router.default.module      =  index                 ; 默认模块
	core.router.default.controller  =  Index                 ; 默认控制器
	core.router.default.action      =  index                 ; 默认方法
	core.view.ext                   =  phtml                 ; 视图文件后缀
	core.view.auto.render           =  1                     ; 是否自动渲染视图，１：自动渲染、０：不渲染


	[db]
	db.master.type                   =  mysql                 ; 数据库类型，默认：mysql
	db.master.host                  =  localhost             ; 数据库主机地址
	db.master.port                   =  3306                  ; 数据库端口
	db.master.dbname                 =  supjos                ; 数据库名称
	db.master.username               =  root                  ; 数据库用户名
	db.master.password               =  3333                  ; 数据库密码

	[dev:core];这个地方表示dev环境下的core配置，　db的配置信息与此一致
	core.application                = '../app'               ; WEB目录
	core.bootstrap                  = '../app/bootstrap.php' ; 指定bootstrap 类目录
	core.bootstrap.method.string    = '__init'               ; 指定Bootstrap类的初始化方法的前缀 
	core.router.modules             =  index,home            ; 注册多模块
	core.router.default.module      =  index                 ; 默认模块
	core.router.default.controller  =  Index                 ; 默认控制器
	core.router.default.action      =  index                 ; 默认方法
	core.view.ext                   =  xhtml                 ; 视图文件后缀
	
**Bootstrap流程：**

当系统运行至 bootstrap() 方法的时候，会自动在引擎内部创建两个对象：\Cs\di\Di 注入容器和 \Cs\mvc\Router 路由对象，以便客户能够在初始化阶段完成路由配置与容器的复用注入：

Bootstrap类必须实现 \Cs\Bootstrap　接口，方便后续的功能扩展与统一：
典型的Bootstrap类如下：

    <?php

    class Bootstrap implements \Cs\Bootstrap
    {
        /**
         * 被引擎调用的方法的前缀默认为：__init开头的所有方法，可以通过配置文件进行更改
         */
	    function __initRouter($di, $router)　/* 初始化路由配置 */
	    {
               $router->add(				/* 路由的具体参数信息见下面介绍 */
                    '/back/:action:/:id:',
                    '/shop/list/$1'
                );
                $router->add(
                    '/shop/:controller:/:action:/:any:',
                    '/get/\1/\2/\3'
                 );
                
                $router->addFromIni('../app/router.ini');　/* 通过配置文件导入路由信息 */
	    }
	    
	    /* 初始化注入容器 */
	    function __InitDb($di, $router)
	    {
                $di->set('db', function(){
                    return new \Cs\db\pdo\MySql();
                });
	    }
	}

CSpeed引擎初始化的流程为：运行所有初始化类: Bootstrap　类的所有以特定前缀开头的方法，并且自带有两个参数分别为：\Cs\di\Di 对象　和 \Cs\mvc\Router对象。

**注意：**
	当在WEB应用中，如果设置的路由匹配成功的话，引擎将会进行重定向，后续的路由将不会处理，因为路由的匹配原则是优先原则。
	
	在URL中的PATH_INFO解析过后，剩余的参数将会保存到全局变量$_GET中，可以通过引擎的类 \Cs\net\Request 和\Cs\net\Response　来进行响应与应答请求，如：
	
	/usr/list/infos/hello/cspeed
	
如果匹配路由：
	
	/usr/list/infos
	
那么在上面匹配的路由的方法 infosAction()方法内可以通过：

 	$_GET['hello']　＝＝>  cspeed
 或者
 
	\Cs\net\Reqeust::get('hello');

方法来获取值。

**控制器：**

一个典型的WEB应用包含有三层：**模型**、**控制器**、**视图**。
CSpeed引擎的控制器继承自：\Cs\mvc\Controller 类，含有三个属性：
	**di**[注入容器对象]、**router**[路由]、**view**[视图引擎]，

属性是否存在：

　　前两个属性只有当调用了 **bootstrap()** 方法后才会自动进行实例化，直接调用 **run()**方法类解析路由的引擎，系统不自动进行创建。**view引擎**对象只有当开启自动渲染视图的时候，才会自动创建。

一个典型的控制器如下：


	<?php

	class Site extends \Cs\mvc\Controller
	{
	    /**
	     * 本方法在每个控制器方法调用前进行调用
	     */
	    function __beforeAction()
	    {

	    }

	    /**
	     * 需要执行的方法
	     */
	    function indexAction()
	    {
	        // 如果开启自动渲染视图，可以使用 $this->view->setVar()方法向模板传输数据
	    }

	    /**
	     * 在每个控制器方法调用执行后进行调用
	     */
	    function __afterAction()
	    {

	    }
	}

**视图：**

CSpeed框架的视图引擎由 \Cs\mvc\View类承担：负责视图渲染与视图布局功能。

视图类方法如下：

１、创建视图对象：
    
    $view = new \Cs\mvc\View();
    
２、设置需要渲染的模块需要使用的数据：

    $view->setVar('name', 'CSpeed');
    $view->setVar('version', 'v2.0.0');
    
3-1、渲染视图：

    $view->render('index/index');
    
3-2、渲染视图的同时传入数据：

    $view->render('index/index', [
        'name'    =>    'CSpeed',
        'version'  =>    'v2.0.0'
    ]);
    
４、获取视图渲染结果但是并不输出：

    $data = $view->getRender('index/index');
    
５、视图内渲染其余的视图布局：
    
    <!DOCTYPE html>
    <?php echo $this->partial('layouts/head');  ?>

    中间是需要渲染的重要内容
       
    <?php echo $this->partial('layouts/foot'); ?>

**模型：**

CSpeed框架提供两个基本类与一个接口供用户自己实现引擎无法满足的功能：

典型的模型使用流程：

用户在使用模型前必须先向系统注入一个数据库对象，目前CSeed引擎只实现了MySQL类，后续增加其余数据库、读写分离支持。

    $di->set('db', function(){
        return new \Cs\db\pdo\MySql(); /* 数据库的连接信息在配置文件中给出 */
    });

**User模型: User.php**

模型必须继承自类：\Cs\mvc\Model，来使用框架提供的抽象接口能力：

	<?php

	namespace app\models;

	class User extends \Cs\mvc\Model
	{
		function tableName()
		{
			return "www_product";
		}
	}

如果模型不重写：tableName方法的，模型操作的数据库就是模型类名小写，如上例：　user 表。


模型提供的方法详细请见源码的IDE目录。

**自动加载机制：**

CSpeed引擎在实例化一个 \Cs\App　类的对象后，自动注册一个命名别名：｀app｀指向WEB目录。

如下典型的CSpeed项目的目录结构：

	-shop
	+public
	|-----index.php                         --------------------------       入口文件
	+app                                      -------------------------　　　应用目录
	|----config                                --------------------------　　配置目录
	+----modules                               ----------------------　　　　模块目录
	|-------backend                          --------------------------　　　后台模块
	|--------+---controllers
	|--------+---models
	|--------+---views
	+------frontend                          --------------------------　　　前端模块
	|--------+--controllers                   --------------------------　　 前端控制器
	|--------+--models                       --------------------------　　　前端模型
	|--------+--views                          --------------------------　　前端视图
	|----models                               --------------------------     通用模型

那么当你创建了一个包含名字空间的\app\models模型类：
	
    namespace app\models;
    
    class User
    {
        function getUsers()
        {
            /* Your logic code. */
        }
    }

那么当你在CSpeed引擎中，使用本类的时候：

    $user = new \app\models\User();
    
那么CSpeed引擎将自动在app目录的models下面查找User.php文件，并加载。
如果客户需要自定义别名，可以使用：
	
	$app->setAlias('@server', '../service/');

第一个参数是需要设置的别名，必须以“@"开头，作为系统标识。第二个参数为：别名指向的路径。

**典型的API应用示例：**

CSpeed框架作为一款全功能型框架，除了支持常见的WEB应用项目开发外还支持API项目的开发，目前功能已完善。

典型的API示例：

    $app = \Cs\App::getApp();
    
    $app->get('/shop/:controller:/:id:', function($controller, $id){
    	/* Your logical code here. */
    });

方法支持两个参数，第一个参数支持使用正则表达式，或者替代符，支持的替代符如下：

	【:action:】：匹配字母、数字、横线、首字母不是数字与横线的字符串
	【:any:】　：匹配除了“/"外的任何字符
	【:controller:】同 【:action:】
	【:id:】　　：匹配任何数字
	【:module:】：同　【:action:】
	
如果第一个参数的正则表达式匹配后，会依次在第二个匿名回调函数的参数列表中，如上面的正则匹配如下的路由：

	/shop/list/33
	
那么方法包含有两个参数：$controller　和 $id:
	
	function ($controller = 'list', $id = 33) {
		echo $controller; // list;
		echo $id;           // 33;
	}

CSpeed框架除了支持GET请求外，还支持常见的POST、OPTIONS、HEAD等。具体请见API文档。


**特别感谢：ＱＱ：５１６１７２、网名：阿杰，给CSpeed提供了很多的建议与意见，当前版本绝大多数业务流程均有参考。**








	
