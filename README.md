# CSpeed v2.1.10手册 #

## QQ群交流 ##

https://jq.qq.com/?_wv=1027&k=5kSunAR

CSpeed扩展官方QQ群号： **605383362**

## 框架特性 ##

1、内置JSON-RPC支持

2、支持BootInit框架统一初始化入口，需要实现 \Cs\BootInit 接口

3、支持多重数据库连接并切换，数据库使用 DSN 连接方式，原始方式支持多种数据库：PgSQL、MySQL等，数据库连接池正在开发。

4、模型AR特性支持[目前不完善]

5、配置文件[ini]按需加载：\Cs\tool\Config 类

6、事件支持： \Cs\tool\Component 类

7、MVC三层开发方式

8、命令行模式支持并支持命令行参数传递

9、原生C语言开发，极致性能，目前已经在 Linux、macOSX 上测试通过

## 安装指南 ##

CSpeed扩展目前在 **Github** 与 **码云** 平台均有代码存储库，用户只需下载源码然后按照如下方法安装即可：

Github:
	
	https://github.com/liqiongfan/cspeed
	
码云：

	https://gitee.com/josinli/cspeed

安装步骤：

	1、/usr/local/php_to_path/bin/phpize
	
	２、./configure --with-php-config=/usr/local/php_to_path/bin/php-config
	
	３、make install 
	
编译完成后在　**php.ini**　配置文件添加如下内容：
	
	extension_dir = "/usr/local/php-7.1.8-nts/lib/php/extensions/no-debug-non-zts-20160303/"
	
	extension=cspeed.so

然后重启 Nginx 的 **PHP-FPM** 或者 **Apache**：
	
	４、systemctl restart php-fpm 或者 systemctl restart httpd

## 测试性能 ##

测试命令：
	
	/usr/local/httpd-2.4.29/bin/ab -c100 -n100000 http://www.supjos.com/hello/cspeed/

测试机器[特意开启旧的笔记本进行测试，未开启工作站]：

	CPU: Intel(R) Core(TM) i5-2430M CPU @ 2.40GHz
	硬盘：HDD 750GB 2009年
	内存：4G Sansung
	
测试环境：
	
	nginx 1.12.1
	php 7.1.5
	php-fpm

测试结果：

	Concurrency Level:      100
	Time taken for tests:   9.781 seconds
	Complete requests:      100000
	Failed requests:        0
	Total transferred:      18500000 bytes
	HTML transferred:       0 bytes
	Requests per second:    10223.94 [#/sec] (mean)
	Time per request:       9.781 [ms] (mean)
	Time per request:       0.098 [ms] (mean, across all concurrent requests)
	Transfer rate:          1847.10 [Kbytes/sec] received

	Connection Times (ms)
		      min  mean[+/-sd] median   max
	Connect:        0    1   0.9      0       9
	Processing:     1    9   1.8      9      25
	Waiting:        1    9   1.8      9      24
	Total:          4   10   1.3     10      28
	WARNING: The median and mean for the initial connection time are not within a normal deviation
		These results are probably not that reliable.

	Percentage of the requests served within a certain time (ms)
	  50%     10
	  66%     10
	  75%     10
	  80%     10
	  90%     11
	  95%     12
	  98%     13
	  99%     13
	 100%     28 (longest request)

测试后单次，不产生任何IO的情况下可以达到 10223reqs/s，持平于1万左右吞吐量。解析ini文件的情况下，可达到将近9千的吞吐量.

## 典型项目结构 ##

     +--cspeed                                      入口文件
        +--app                                      项目目录
            +---config                              配置目录
                |---core.ini                        配置文件
            +---models                              通用模型目录
                |---User.php                        User模型
                |---Goods.php                       Goods模型
            +---modules                             模块目录
                +---backend                         后台模块
                    +---controllers                 backend模块控制器目录
                        |---Index.php               Index控制器
                        |---About.php               About控制器
                    +---views                       视图目录
                        +---index                   Index控制器的视图目录
                            |---index.phtml         Index控制器的index方法的视图文件
                        +---about                   About控制器的视图目录
                            |---index.phtml         About控制器的index方法的视图文件
                +---home                            Home模块
                    +---controllers                 Home模块控制器目录
                        |---Index.php               Index控制器
                    +---views                       视图目录
                        +---index                   Index控制器的视图目录
                            |---index.phtml         Index控制器的index方法的视图文件
            +---bootstrap.php                       框架的初始化类Bootstrap
        +--public                                   入口目录
            |---index.php                           入口文件
            +---assets                              资源目录
                +---css                             CSS资源
                +---js                              JS资源
                +---img                             图片资源

## 简单的示例 ##

１、WEB示例

    $app = new \Cs\App("../app/config/core.ini", "dev");
    
    $app->bootstrap()->run();

Cs\App类的构造函数支持传入绝对路径或者相对路径的INI文件，第二个参数是INI配置文件的解析节点，构造函数的两个参数都可以省略来进行API框架的配置达到减少IO操作的目的.

一个复杂的**WEB应用**就只需以上两行代码就可搞定。

２、API示例

    $app = new \Cs\App();
    
    $app->get('/hello/cspeed/:any:', function($any){
        echo "<div style='text-align:center;'>Hello CSpeed User, The any value is : $any.</div>";
    });
    
**RESTful API** 项目只需要添加对应的请求方法即可。

## CSpeed引擎INI配置项 ##

	[core]
	core.application                = '../app'               ; WEB目录
	core.bootstrap                  = '../app/bootstrap.php' ; 指定bootstrap 类目录
	core.bootstrap.method.string    = '__init'               ; 指定Bootstrap类的初始化方法的前缀 
	core.router.modules             =  index,home,back       ; 注册多模块
	core.router.default.module      =  index                 ; 默认模块
	core.router.default.controller  =  Index                 ; 默认控制器
	core.router.default.action      =  index                 ; 默认方法
	core.view.ext                   =  phtml                 ; 视图文件后缀
	core.view.auto.render           =  0                     ; 是否自动渲染视图，１：自动渲染、０：不渲染
	core.url.pattern				= '.html'	 			 ; url 模型的伪静态设置

	[db]
	db.master.type                   =  mysql                 ; 数据库类型，默认：mysql
	db.master.host                   =  localhost             ; 数据库主机地址
	db.master.port                   =  3306                  ; 数据库端口
	db.master.dbname                 =  supjos                ; 数据库名称
	db.master.username               =  root                  ; 数据库用户名
	db.master.password               =  3333                  ; 数据库密码

	[dev:core]
	core.application                = '../app'               ; WEB目录
	core.bootstrap                  = '../app/bootstrap.php' ; 指定bootstrap 类目录
	core.bootstrap.method.string    = '__init'               ; 指定Bootstrap类的初始化方法的前缀 
	core.router.modules             =  index,home            ; 注册多模块
	core.router.default.module      =  index                 ; 默认模块
	core.router.default.controller  =  Index                 ; 默认控制器
	core.router.default.action      =  index                 ; 默认方法
	core.view.ext                   =  xhtml                 ; 视图文件后缀
	core.view.auto.render           =  0                     ; 是否自动渲染视图，１：自动渲染、０：不渲染
	core.url.pattern				= '.html'	 			 ; url 模型的伪静态设置

## 典型的Bootstrap初始化类 ##

	class Bootstrap implements \Cs\BootInit
	{
	    /* 初始化路由与视图 */
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
	    }
	    
            /* 初始化数据库连接 */
            function __initDb($di, $router)
            {
                $di->set('db', function(){
                    return new \Cs\db\pdo\Adapter();
                });
            }
	}

## 典型的控制器结构 ##

    <?php
    
    namespace app\modules\hello\controllers;
    
    class Cspeed extends \Cs\mvc\Controllers
    {
    	/**
    	 * 初始化方法，如果实例化本方法，则 initialise 方法会从顶级父类开始执行到本方法
    	 * 来完成初始化
    	 */
    	function initialise()
    	{
    	
    	}
    	
    	/**
    	 * 具体的方法
    	 * CSpeed 框架内，URL的方法以 Action 结束
    	 */
    	function newsActon()
    	{
    	
    	}
    }

## 典型的模型结构 ##

	<?php

	namespace app\models;

	class User extends \Cs\mvc\Model
	{
		/**
		 * 如果存在构造函数，务必显式调用父类构造函数
		 * 可以在构造函数内使用事件绑定特性
		 */
		function __construct()
		{
			// 如果存在构造函数方法，则务必显式调用父类的构造器
			parent::__construct();
		}
		
		function tableName()
		{
			return "www_product";
		}
	}


## API索引 ##

### Cs\App ###

    /**
     * 构造函数.
     * @param string $iniConfigPath 需要加载到引擎的配置文件路径.
     * @param string $iniNodeName  　INI配置文件节点名称
     */
    function __construct($iniConfigPath, $iniNodeName)

    /**
     * 运行框架
     */
    function run()

    /**
     * 初始化引擎
     */
    function bootstrap()
    
    /**
     * 返回App对象
     */
    function getApp()
    
    /**
     * 设置系统加载别名
     * @param $aliasKey         需要加载的别名
     * @param $aliasFullPath    别名表示的绝对路径
     */
    function setAlias($aliasKey, $aliasFullPath)

    /**
     * 匹配 GET 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function get($url, $closure)

    /**
     * 匹配 POST 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function post($url, $closure)

    /**
     * 匹配 PUT 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function put($url, $closure)

    /**
     * 匹配 PATCH 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function patch($url, $closure)

    /**
     * 匹配 DELETE 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function delete($url, $closure)
    
    /**
     * 匹配 HEAD 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function head($url, $closure)

    /**
     * 匹配 OPTIONS 请求
     * @param $url          待匹配的URL
     * @param $closure      匹配成功后需要执行的回调函数
     */
    function options($url, $closure)

### Cs\BootInit ###

    预留接口
    
### Cs\db\ModelInterface ###

    /**
     * SELECT
     * @param array|string $fields   SELECT查询的字段
     * @return \Cs\db\ModelInterface
     */
    function select($fields);

    /**
     * FROM
     * @param string $table    查询的数据表名
     * @return ModelInterface
     */
    function from($table);

    /**
     * WHERE
     * @param array|string $whereCondition    WHERE条件
     * @return ModelInterface
     */
    function where( $whereCondition);

    /**
     * GROUP BY
     * @param array|string $groupBy          GROUP BY条件
     * @return ModelInterface
     */
    function groupBy( $groupBy);

    /**
     * HAVING
     * @param array|string $having           HAVING条件
     * @return ModelInterface
     */
    function having($having);

    /**
     * ORDER BY
     * @param array|string $orderBy           ORDER BY 条件
     * @return ModelInterface
     */
    function orderBy($orderBy);

    /**
     * LIMIT
     * @param int $count        条数
     * @param int $offset       偏移量
     * @return ModelInterface
     */
    function limit($count, $offset);

    /**
     * @param $rawSql               原生SQL语句
     * @param array $bindParams     SQL语句绑定的预处理变量
     * @return ModelInterface
     */
    function query($rawSql, array $bindParams);

    /**
     * 执行query方法的语句，并返回结果
     * @return mixed
     */
    function execute();
    
### Cs\db\pdo\Adapter ###

    /**
     * PDO构造函数.
     * @param array $connectionOptions  可选连接参数,如： \PDO::ATTR_PERSISTENT.
     */
    function __construct(array $connectionOptions = [])

    /**
     * @param array|string 查询的字段
     * @return $this|Adapter
     */
    function select($fields)

    /**
     * @param string $table 表名
     * @return $this|Adapter
     */
    function from($table)
    
    /**
     * @param array|string $whereCondition　WHERE 条件
     * @return $this
     */
    function where( $whereCondition)

    /**
     * @param array|string $groupBy GROUP BY 条件
     * @return $this
     */
    function groupBy($groupBy)

    /**
     * @param array|string $having HAVING 条件
     * @return $this|Adapter
     */
    function having($having)

    /**
     * @param array|string $orderBy ORDER BY 条件，如： orderBy(['id DESC', 't ASC'])
     * @return $this|Adapter
     */
    function orderBy($orderBy)

    /**
     * @param int $count    数目
     * @param int $offset   偏移量
     * @return $this|Adapter
     */
    function limit($count, $offset) 

    /**
     * @param string $rawSql       执行的SQL原生语句
     * @param array $bindParams    绑定的预处理变量
     * @return $this|Adapter
     */
    function query($rawSql, array $bindParams)
    
    /**
     * 返回query方法的执行结果
     * @return mixed
     */
    function execute()

    /**
     * @param $whereCondition    WHERE 条件
     * @return $this|Adapter
     */
    function andWhere($whereCondition)

    /**
     * 开启SQL事务
     */
    function begin()

    /**
     * 回滚事务
     */
    function rollback()

    /**
     * 提交事务
     */
    function commit()

    /**
     * 返回最后的插入记录 ID
     * @return mixed
     */
    function lastInsertId()

    /**
     * 返回影响的行数
     */
    function rowCount()

    /**
     * 从结果集中返回一条记录
     */
    function find()

    /**
     * 返回所有的结果集
     */
    function findAll()

### Cs\di\Di ###

    /**
     * Di 构造函数
     */
    function __construct()

    /**
     * @param $key      返回注入容器的对象的关联key
     * @return object|mixed
     */
    function get($key)

    /**
     * @param $key      存入注入容器的对象对应的key索引
     * @param $closure  回调函数，回调函数的返回值存入注入容器
     */
    function set($key, $closure)

### Cs\mvc\Controller ###

    /**
     * @var $di \Cs\di\Di　Di容器对象
     */
    public $di;

    /**
     * @var $view \Cs\mvc\View　View视图对象
     */
    public $view;

    /**
     * @var $router \Cs\mvc\Router    Router路由对象
     */
    public $router;
    
    /**
     * 路由的初始化方法
     * 从父类初始化到本类
     */
    function initialise()
    
    /**
     * 在执行具体的方法之前调用
     * 不会执行父类的本方法
     */
    function __beforeAction()
    
    /**
     * 在执行完具体的方法之后调用
     * 不会执行父类的本方法
     */
    function __afterAction()

### Cs\mvc\Model ###

     * Model 模型构造函数.
     */
    public function __construct()

    /**
     * 返回需要执行 UPDATE 操作的Model对象
     * @return Model
     */
    static function find()

    /**
     * 返回模型需要操作的数据表名，默认操作当前模型对应的表名
     */
    function tableName()

    /**
     * @param array|string $fields　SELECT 条件
     * @return $this
     */
    function select($fields)

    /**
     * @param array|string $where WHERE 条件
     * @return $this
     */
    function where($where)

    /**
     * @param array|string $where    WHERE条件
     * @return $this
     */
    function andWhere($where)
    
    /**
     * @param array|string $orderBy    ORDER BY 条件
     * @return $this
     */
    function orderBy($orderBy)

    /**
     * @param array|string $groupBy    GROUP BY条件
     * @return $this
     */
    function groupBy($groupBy)

    /**
     *返回一条结果
     */
    function one()

    /**
     * 返回结果集中的所有记录
     */
    function all()

    /**
     * 执行 UPDATE|INSERT 操作，如果是new模型执行 INSERT 操作, find模型执行 UPDATE 操作
     */
    function save()

    /**
     * 执行 DELETE 操作
     */
    function delete()

### Cs\mvc\View ###

    /**
     * View 构造函数.
     */
    function __construct()

    /**
     * 渲染视图模板
     * @param $templateFileName     需要渲染的文件名称，不带后缀
     * @param $variables           需要传入模板的数据
     */
    function render($templateFileName, $variables)

    /**
     * 设置模板的后缀，默认后缀 phtml
     * @param $suffixName           需要设置的模板后缀
     */
    function setSuffix($suffixName)

    /**
     * 向模板中添加数据
     * @param $varName           变量名称
     * @param $varValue            变量值
     */
    function setVar($varName, $varValue)

    /**
     * 获取渲染的结果
     * @param $templateFileName     需要渲染的文件名称，不带后缀
     * @param $variables           需要传入模板的数据
     */
    function getRender($templateFileName, $variables)

    /**
     * 设置视图引擎渲染的模板存储目录
     * @param $viewDir            视图存储的目录
     */
    function setViewDir($viewDir)

    /**
     * 布局渲染
     * @param $templateFileName     需要渲染的文件名称，不带后缀
     * @param $variables           需要传入模板的数据
     */
    function partial($templateFileName, $variables)

### Cs\net\Request ###

    /**
     *获取 HTTP_HOST 信息
     */
    function getHttpHost()

    /**
     * 获取 HTTP USER AGENT
     */
    function getHttpUserAgent()

    /**
     * 获取 SERVER_NAME
     */
    function getServerName()

    /**
     * 获取 SERVER_ADDR
     */
    function getServerAddr()

    /**
     * 获取 REMOTE_PORT
     */
    function getRemotePort()

    /**
     * 获取 REMOTE_ADDR
     */
    function getRemoteAddr()

    /**
     * 获取 REQUEST_SCHEME
     */
    function getReqeustScheme()

    /**
     * 获取 SERVER_PROTOCOL
     */
    function getServerProtocol()

    /**
     * 获取 DOCUMENT_ROOT
     */
    function getDocumentRoot()

    /**
     * 获取 REQUEST_URI
     */
    function getRequestUri()

    /**
     * 获取 SCRIPT_NAME
     */
    function getScriptName()
    
    /**
     * 获取 PATH_INFO
     */
    function getPathInfo()

    /**
     * 获取 QUERY_STRING
     */
    function getQueryString()

    /**
     * 是否 GET 请求
     */
    function isGet()

    /**
     * 是否 PUT 请求
     */
    function isPut()

    /**
     * 是否 PATCH 请求
     */
    function isPatch()

    /**
     * 是否 DELETE 请求
     */
    function isDelete()

    /**
     * 是否 HEAD 请求
     */
    function isHead()

    /**
     * 是否 OPTIONS 请求
     */
    function isOptions()

    /**
     * 获取 $_GET 数据
     */
    function get()

    /**
     * 获取 $_POST 数据
     */
    function getPost()

### Cs\net\Response ###

    /**
     * Response 构造函数.
     */
    function __construct()

    /**
     * 设置 HTTP 头
     * 如： setHeader('Content-Type', 'application/json;charset=UTF-8');
     * @param $headerKey        HTTP 头名称
     * @param $headerValue      HTTP 头值
     */
    function setHeader($headerKey, $headerValue)

    /**
     * 删除设置的 HTTP 头信息
     * @param $headerKey       需要删除的HTTP头的名称，函数必须在调用send()前使用
     */
    function unsetHeader($headerKey)
    
    /**
     * 发送 HTTP 头信息与相应内容
     */
    function send()

    /**
     * 设置返回的响应内容为JSON格式数据
     * @param array $jsonArray
     */
    function setJsonContent(array $jsonArray)

    /**
     * 原始数据格式
     * @param $rawContent   原始数据格式
     */
    function setRawContent($rawContent)

    /**
     * 重定向到指定的URL
     * 如：redirect('/index/lists') 重定向到： http://host/index/lists
     * @param $url          要重定向的 URL
     */
    function redirect($url)

### Cs\tool\Config ###

    /**
     * 构造函数.
     */
    function __construct()

    /**
     *加载INI数据
     * @param string $configFilePath    INI文件的相对路径
     */
    function loadConfig($configFilePath)

    /**
     * 返回INI数据，解析节点
     */
    function getConfigs()

    /**
     * 返回INI数据中，对应的节点数据
     */
    function getConfig($configKey)

### Cs\mvc\Router ###

    /**
     * Router 构造函数.
     */
    function __construct()

    /**
     * 添加一条记录到Router路由
     * @param $destUrl 支持正则匹配的 URL 路由
     * @param $toUrl   匹配路由后的重定向路由
     */
    function add($destUrl, $toUrl)

    /**
     * 从 INI 配置文件添加路由
     * @param $iniFile 包含有路由的 INI 配置文件
     */
    function addFromIni($iniFile)

    /**
     * 从数组中添加路由
     * @param $arrayRouters 包含有数组的路由，数组的键是待匹配的 URL，值是匹配成功后重定向的URL
     */
    function addFromArray($arrayRouters)
    {

    }

### Cs\tool\Component ###

	/**
	 * 构造函数
	
	/**
	 * 绑定事件
	 * @param $eventName 需要绑定的事件名称
	 * @param $callBack     事件发生时调用的函数
	 */
	function on($eventName, $callBack);
	
	/**
	 * 解除已经绑定的事件
	 * @param $eventName 需要解除绑定的事件名称
	 * @param $callBack     事件绑定的函数句柄, NULL会解除所有的事件名称一致的事件消息
	 */
	function off($eventName, $callBack = NULL);
	
	/**
	 * 触发事件
	 * @param $eventName 	需要触发的事件的名称
	 */
	function trigger($eventName);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	