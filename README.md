# CSpeed PHP Micro Framework v1.2.2 #

----------

## 最新版本特性  ##

**CSpeed v1.2.2 特性**
	
	1、新增全局静态变量：\Cs\App::$app 获取注入容器Di的示例：
	
	在入口文件index.php 文件里：
	
	$di = new \Cs\di\Di();
	
	$di->set('config', function(){
		$config = new \Cs\tool\Config();
		$config->loadConfig('./cspeed.ini');
		return $config;
	});
	
	$app = new \Cs\App($di);
	$app->run();
	
	/* 经过上面的设置后，可以在CSpeed引擎的任何地方
	使用如下的方法获取Di容器注入的对象 */
	$config = \Cs\App::$app->get('config');
	
	/* 用户可以使用在多模块的系统架构中使用本特性 */
	
	2、新增ini配置文件解析类：\Cs\tool\Config
	
		Config类含有三个方法：
			
			1、loadConfig($iniFile);
				加载ini配置文件
				
			2、getConfigs();
				获取ini配置文件解析后的数据
			
			3、getConfig($configKey);
				获取ini配置文件解析后对应的关联数组的数据值.
	
	3、优化Model、MySql性能与BUG修复。

**CSpeed v1.2.1** 修复路由bug：

默认的CSpeed引擎在1.2.1之前的版本，系统路由的规则如下：

默认的PATH_INFO信息的第一段自动作为模块优先处理，所以导致如下的bug：

	/user/index与/user/index/index的路由规则是一致的问题。
	
在新的CSpeed引擎v1.2.1版本中已经予以修复。请及时更新。
	
	v1.2.1新增方法：
	Cs\App 类：
		
		function registerModules(['admin', 'fronted']);
			参数说明：
				方法的参数为需要CSpeed引擎允许执行的模块，如果在路由规则中的模块不在此注册函数内，CSpeed引擎不予以执行。
		
**CSpeed v1.2.0** 发布特性：
	
	1、新增数据模型类：Cs\mvc\Model
	2、模型支持ActiveRecord
	
使用示例：
	
	在应用的入口文件 index.php 中添加一条MySql类至Di注入容器：
	
	$di -> set('db', function(){
		return new \Cs\db\pdo\MySql([
			'dsn' => 'mysql:host=locahost;dbname=cspeed',
			'usrname' => 'root',
			'password'=>'cspeed'
		]);
	});
	/* 经过上面的设置后，在CSpeed框架中就可以使用模型操作数据库啦 */
	
	/* 具体的使用示例如下： */
	/* 模型文件 */
	User.php
	
	class User extends \Cs\mvc\Model
	{
		/* 开发者如果需要操作的不是模型对应的数据库的话，请重写本方法返回需要操作的数据库表的名称即可
		* 默认不重写本方法的话，对应的数据库是模型的名称小写，如本例： user 表 */
		function tableName()
		{
			return 'www_user';
		}
	}
	
	/* 控制器Index.php的indexAction方法中： */
	
	// 1、新增记录
	$user = new User();
	$user->age = 25;
	$user->name = "Josin";
	$user->mobile = '10086';
	$user->email = '774542602@qq.com';
	
	$user->save(); // 返回的结果是影响的行数
	
	// 2、修改记录
	$user = User::find()->where(['id' => 2]);
	
	$user->age = 24;
	$user->mobile = '10000';
	$user->save(); // 返回影响的行数
	
	// 3、删除记录
	$user = User::find()->where(['id' => 2]);
	$user->delete(); // 返回影响的行数
	
	// 4、查询记录
	$info = User::find()->where(['price' => '8888])->all();



## 开发环境 ##
	
	Linux kernel 4.4.x、PHP7.1.8、Nginx1.12.1

	扩展只支持PHP7.x以上版本，低于PHP7.x以下的版本请先升级PHP版本

	在WEB应用模式下，扩展通过解析 PATH_INFO 参数信息进行路由转发，请先确保 WEB服务器支持 PATH_INFO 模式；
	并且需要隐藏index.php，否则系统不生效,无法完成路由解析。

	推荐的Nginx配置：
	
    location / {
        try_files $uri $uri/ /index.php$uri$args;
    }

    location ~ \.php {
	    fastcgi_pass   127.0.0.1:9000;
	    fastcgi_index  index.php;
	    fastcgi_split_path_info ^(.+\.php)(.*)$;     
	    fastcgi_param  PATH_INFO $fastcgi_path_info;    
	    fastcgi_param  SCRIPT_FILENAME  $document_root$fastcgi_script_name;
	    include        fastcgi_params;
    }
    

----------

## CSpeed安装 ##
   
下载源码后，进入cspeed目录，按照以下三个步骤进行即可：

	1、/usr/path_to_php/bin/phpize
	
	2、./congure --with-php-config=/usr/path_to_php/bin/php-config
	
	3、make -j8 install
	
	4、在编译安装成功的命令行界面会提示“Installed .... /usr/local/path_to_php/lib/php/extensions/non-debug-non-zts-2xxxx”,
	   用户仅需要在您的 PHP 配置文件 php.ini 添加一行：extension_dir = "xxxx", 其中的xxxx就表示上面的”/usr/local/path_to_php/xxx“目录
	5、在php.ini文件中另外新增一行： extension = cspeed.so
	
	6、重启服务器Apache或者Nginx的PHP-FPM
	
经过以上步骤后，可以通过在phpinfo()中查看cspeed扩展
或者使用如下函数检测：
	extension_loaded('cspeed');
    如果函数返回true则表示安装成功.
    
----------

## 简单的 WEB **示例** ##
	
WEB 目录设置为如下：

	+public
	|--index.php          入口文件
	+controllers
	|--Index.php          Index默认控制器
	+admin                新增admin模块
	|--controllers         admin模块下的控制器目录
	     |--Index.php     admin模块的Index控制器
	+fronted               fronted模块
	|--controllers         fronted模块控制器目录
	    |--Index.php      fronted模块Index控制器


public 目录下 index.php 内容如下：
	
	$app = new \Cs\App();

	$app->run();
	/* 超简单的框架已经完成，只需要上面两行代码就可完成一个MVC框架 */
	
	/* 注意App类构造函数可选参数Di类对象，如： */
	$di = new \Cs\di\Di();
	
	$di->set('view', function(){
		$view = new \Cs\mvc\View();
		$view->setModuleDir('index');
		return $view;
	});
	/*那么可以将此对象传入App构造函数中*/
	$app = new \Cs\App($di);
	
	/* 当进行了上面的步骤后，在控制器中可以使用$this-get('view')来获取设置的对象，以达到对象复用的目的 */
	

在 public 同级的controllers目录下创建一个 Index.php 文件,内容如下：
	
	Index.php:
		
	class Index {
	    function indexAction(){
		// 当需要渲染视图的时候，请使用 $this->view 获得 View 引擎然后使用引擎具有的方法进行视图渲染 
		echo '<h1>Hello CSpeed</h1>';
	    }
	}
	
配置好 Nginx 路由，打开浏览器，输入配置好的网站地址就会看到刚刚输入的内容：

	http://path_to_cspeed


WEB应用路由规则：
	
	假设配置的Nginx如上，配置网站 www.supjos.cn 指向：public目录下的index.php文件

	那么，路由规则如下三种情况所示：

	1)、www.supjos.cn
		不带PATH_INFO的路由指向与public目录同级的controllers下的Index.php控制器的indexAction方法
	2)、www.supjos.cn/backend/goods/lists
		路由到backend模块的goods控制器下的listsAction方法
	3)、www.supjos.cn/sys/info
		路由到与public同级的controllers目录下Sys.php下的infoAction方法

## 注意 ##
	
	控制器文件首字母必须大写
	
	如下路由所示：
		www.supjos.cn/web/index/lists
		模　块: web		一个模块对应一个目录，此处对应web目录
		控制器: index		控制器文件命名为 Index.php
		方　法: listsAction  	CSpeed系统方法名都以Action结尾
	

## 简单的 **API** 应用 ##

public 目录下的 index.php 内容如下：

	index.php
	
	/* 当实例化一个 \Cs\App 类后，系统自动进行 autoload， autoload的机制参见下面介绍 */
	$app = new \Cs\App();	

	/**
	 * 方法的第一个参数支持正则匹配，第二个参数是一个Closure闭包函数
	 * 第一个参数支持使用替代符 ： {name} 与 {id}, 其中 {name} 表示匹配字母数字横线并且首字母不是数字的字符串；
	 *  {id}表示匹配任何数字，注意当使用了替代符的时候，匿名函数包含有一个参数：
	 *  $match 来一一对应与匹配的替代符，如下面的正则匹配 URL “/index/cspeed-v1/18”的话：
	 *  $app->get('/index/{name}/{id}$', function($match){
	 * 
	 *  });
	 *  那么 $match[1] 则表示 cspeed-v1 $match[2]表示为18，依次类推
	 */
	$app->get('/index$', function(){
		echo '<h1>Hello CSpeed</h1>';
	});

	/* CSpeeed支持常见的请求方法，除了上面的 GET 外，还支持 POST、PUT、DELETE、OPTIONS、HEAD，具体见 API 文档 */
	$app->post('/goods/index/2$', function(){
		/* Your code here. */
	});


## CSpeed自动加载机制 ##


	当实例化一个 \Cs\App 类后，系统自动进行未引用文件的加载, 加载机制采用 “别名引用”机制，具体的原理如下：

	系统自动内置一个 app 命名别名，指向 index.php 的上级目录，如下目录所示：
	
	+--cspeed                          网站目录
	   +--public
	       |--index.php                入口文件
	   +--controllers                  默认控制器加载目录
	       |--Index.php
	       |--Goods.php
	   +--backend                    新增模块backend
	       |--controllers              backend模块的控制器目录
		   |--Index.php			
		   |--Goods.php							
		

	如果目录结构如上所示：
	
	那么 默认的 `app` 别名指向 cspeed 目录。
	开发者可以通过 $app->setAlias()来设置别名，具体的设置方法如下：

	假设需要设置一个 backend 的别名 指向目录 /data/supjos/backend ，那么调用方法如下：
		$app->setAlias('@backend', '/data/supjos/backend');
	
	用户可以自己创建一个如下的文件：

	<?php

	namespace app\controllers;

	class Index {
	    function getVersion()
	    {
		return 'v2.1.8';
	    }
	}

	那么当你在CSpeed框架中需要使用 \app\controllers\Index类的时候，可以使用如下两种方法使用：

	方法1：
	    use app\controllers\Index;

	    $index = new Index();

	方法2：
	    $index = new \app\controllers\Index();
	
	以上两种方法都可以使得 CSpeed 框架自动加载您所编写的 Index.php 文件

## CSpeed 框架的视图引擎 ##
	
	$view = new \Cs\mvc\View();

	/* 方法包含两个参数，
	 * 第一个参数为：需要渲染的视图的文件名
	 * 第二个参数为：需要在视图中使用的变量，传入一个数组或者不传入
	 */
	$view->render('index', ['name'=>'CSpeed', 'version'=>'v2.1.8']);
	
	/* 如果需要添加单个变量到视图模块中，可以使用 setVar 方法 */
	$view->setVar('addVar', ['a', 'b' ,'c', 'd']);

	/* 如果需要或者视图的渲染效果但是并不输入使用 getRender方法，参数与 render 方法一致 */
	$viewResult = $view->getRender('index', ['name'=>'CSpeed', 'version'=>'v2.1.8']);

	/* 默认的渲染视图后缀为 phtml，可以通过 setSuffix 方法进行更改 */
	$view->setSuffix('ppht');

	/* 默认视图文件夹保存在 public 目录同级的 views 目录下, 可以通过 setViewDir 进行更改, 目录不能以 "/" 结尾 */
	$view->setViewDir('../views');
	
	/*视图内渲染*/
	$view->partial('layouts/head', ['a', 'b', 'c']);

## 数据库连接 ##
	
	CSpeed框架仅提供一个简单的MySql类以及一个ModelInterface接口供用户自己实现模型或者增强现有的模型：
	
	$mysql = new \Cs\db\pdo\MySql([
		'dsn'          =>    'mysql:host=localhost;dbname=cspeed',
		'username' =>	 'root',
		'password'  =>    '3333'
	]);
	
	/* 查询一条记录 */
	$mysql->select(['id', 'price', 'name'])->from('www_product')
  		  ->where(['id' => 88'])->find();
	 
	/* 查询满足条件的所有记录 */
	$mysql->select(['id', 'price', 'name'])->from('www_product')->findAll();
        
	/* 执行原生SQL查询 */
	$mysql->query(" SELECT * FROM www_product  ");
	$results = $mysql->execute(); 
        
	/* 执行预处理 */
	$mysql->query(' INSERT INTO www_product (id, price, name) VALUES (:id, :price, :name) ',
 	               [':id' => 33, ':price'=>3.33, ":name"=>"Apple"]);
	$mysql->exeucte();
	/* 获取刚刚插入的数据ID */
	$id = $mysql->lastInsertId();

## 模型 ##
	
	/* 使用模型前需要先向CSpeed引擎注入对应的模块 */
	index.php 入口文件:
	
	$di -> set('db', function(){
		return new \Cs\db\pdo\MySql([
			'dsn'		=>	'mysql:host=localhost;dbname=cspeed',
			'username'	=>	'root',
			'password'	=>	'cspeed'
		]);
	});
	
	/* 在控制器中或者CSpeed项目的其余地方 */
	User模型：
		class User extends \Cs\mvc\Model
		{
			// 此处没有重写tableName方法，故操作的数据表是： user	
		}
		
	【增加记录】：
		$user = new User();
		$user->name = 'cspeed';
		$user->version = 'v1.2.1';
		$user->save();
		
	【更新记录】:
		$user = User::find()->where('id = 1');
		$user->name = 'CSpeed';
		$user->version = 'v1.2.2';
		$user->save();
		
	【删除记录】：
		$user = User::find()->where(['id'=>1]);
		$user->delete();
		
	【查询记录】：
		// 一条记录
		$info = User::find()->where(['id'=>3])
					  ->andWhere('name="CSpeed"')->one();
		// 所有的记录：
		$infos = User::find()->all();
	
	如果需要启用事务：请使用：
		$this->get('db')->begin(); // 启用事务
		$this->get('db')->rollback(); // 回滚事务
		$this->get('db')->commit(); // 提交事务
	上面示例：
		$this->get('db')：
	是使用的注入容器的方法，其中的db对应于$di->set('db', function(){}); 中的 db。

## 测试结果 ##
	
	测试机器：	
		1、SSD 240GB
		2、Intel Core i7-4790k
		3、16GB 1866GHZ内存 
		4、Linux Debian 8.x kernel 4.4.x
		5、PHP 7.1.8
		6、Nginx 1.12.1

	siege 3.0.8
	
		测试命令: siege -c100 -t5m -b http://localhost/web

		共完成一百九十多万请求，每次请求4.4kb数据，零错误,CPU占用23%左右内存占用极低。
	
## API 索引 ##

### Cs\App 类 ###
	
	/* 构造函数 */
	public function __construct($di);
	参数：
		$di:	
			Cs\di\Di类的对象
	
	/* 框架的自动加载函数 */
	public function autoload();
	
	/* 匹配GET请求 */
	public function get($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
					
	/* 匹配POST请求 */
	public function post($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
			
	/* 匹配PUT请求 */
	public function put($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
	
		
	/* 匹配PATCH请求 */
	public function patch($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
	
		
	/* 匹配DELETE请求 */
	public function delete($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
			
	/* 匹配 HEAD 请求 */
	public function head($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
			
	/* 匹配 OPTIONS 请求 */
	public function options($url, $closure);
	参数：
		$url:	
			可选正则URL匹配表达式，替代符{name}:字符数字横线，且首字母不是数字
		$closure:
			闭包函数，当URL匹配成功后执行的函数
	
	/* 设置别名，系统凭借别名导入文件 */
	public function setAlias($aliasKey, $aliasFullPath);
		参数:
			$aliasKey:
				别名名称
			$aliasFullPath:
				别名指向的绝对路径
	
	/* 注册模块，只有已注册过的模块，路由才能通过 */
	public function registerModules($modules);
		参数:
			$modules:
				类型：数组类型数据，表示需要注册的模块，如：
			registerModules(['admin', 'fronted']);
	
	/* 获取注入的容器对象 */
	public static $app;
	
	/* CSpeed 框架执行URL分析 */
	public function run();
	
### Cs\db\ModelInterface 接口 ###
	
	/* SQL SELECT 字段 */
	public function select( array $fields);
		参数:
			$fields:
				数组类型，SELECT查询的字段内容如：['id', 'price', 'name']
				
	/* FROM */
	public function from($from);
		参数：
			$from:
				SELECT查询的表名，字符串类型
				
	/* WHERE */
	public function where(array $where);
		参数：
			$where:
				数组类型，WHERE条件，如：['id'=>22, 'name'=>'cspeed]
		
	/* GROUP BY */
	public function groupBy($groupBy);
		参数：
			$groupBy:
				数组类型，GROUP BY条件， 如：[ 'name', 'price' ]
		
	/* HAVING */
	public function having($having);
		参数：
			$having:
				数组类型，HAVING条件，如：[ 'id' => 22 ]
		
	/* ORDER BY */
	public function orderBy($orderBy);
		参数：
			$orderBy:
				数组类型, ORDER BY条件，如：[ 'price DESC', 'create_time DESC' ]
		
	/* LIMIT */
	public function limit($num, $offset);
		参数：
			$num : 
				查询的数量，如：20
			$offset:
				查询偏移量，如：2
	
	/* 执行原生的SQL语句，支持预处理绑定变量 */
	public function query($rawSql, $bindParms);
		参数：
			$rawSql:
				字符串类型，执行的SQL语句，如：
					SELECT * FROM www_product
			$bindParams:
				数组类型，绑定变量，如果SQL中使用了绑定变量的，那么此参数对应，如：query('INSERT INTO www_product (name, price) VALUES(:name, :price)', [ ':name' => 'Apple', ':price' => '8.8' ]);
	
	/* 执行原生查询的结果 */
	public function execute();
	
### Cs\db\pdo\MySql 类 ###

	继承自 : Cs\db\ModelInterface
	
	自增的方法如下，其余方法同ModelInterface类
	
	/* WHERE */
	public function andWhere($andWhere);
		参数：
			$andWhere:
				数组类型， 同where方法，可以多次调用本方法，来组合多个WHERE条件
	
	/* 启用事务 */
	public function begin();
	
	/* 回退事务 */
	public function rollback();
	
	/* 提交事务 */
	public function commit();
	
	/* 最新的一条记录的ID */
	public function lastInsertId();
	
	/* 受影响的记录数 */
	public function rowCount();
	
	/* 查询一条满足条件的SQL，并返回结果 */
	public function find();
	
	/* 查询满足条件的SQL，返回结果集 */
	public function findAll();
	
### Cs\di\Di 类 ###

	/* 执行构造函数 */
	public function __construct();
	
	/* 容器注入 */
	public function set($key, $closure);
		参数：
			$key:
				容器对象的字符串索引，如：'view'
			$closure:
				闭包函数，返回一个相应的对象实例，如：
		$di = new Cs\di\Di();
		$di->set('view', function(){
			return new \Cs\mvc\View();
		})
		
	/* 容器的对象取出 */
	public function get($key);
		参数：
			$key:
				字符串类型，需要从容器中取出的对象的字符串索引,如：
		$di = new Cs\di\Di();
		$view = $di->get('view');

### Cs\mvc\Controller ###
	继承自 Cs\di\Di 类
	
	CSpeed框架的默认控制器， 用户控制器一般需要继承自本控制器

### Cs\mvc\View 类 ###

	CSpeed框架的默认视图引擎
	
	/* 构造函数 */
	public function __construct();
	
	/* 渲染视图模板 */
	public function render($template, $variables);
		参数：
			$template:
				待渲染的视图模板名称，不需要带后缀，默认的文件后缀为 phtml
			$variables:
				数组类型，模板中需要使用的变量
				
	/* 设置模板的后缀 */
	public function setSuffix($suffixName);
		参数：
			$suffixName:	模板的后缀名称，默认为 phtml，如需替换为html，则：
		setSuffix('html')， 即可。
	
	/* 设置模板中需要使用的变量，可以多次调用 */
	public function setVar($varName, $varValue);
		参数：
			$varName:	变量名称
			$varValue:		变量内容
	
	/* 获取模板的渲染结果，并返回不进行输出操作 */
	public function getRender($template, $variables);
		参数：同render，这里不在重复讲解
	
	/* 设置视图目录路径名 */
	public function setViewDir($dirPath);
		参数：
			$dirPath:
				设置视图的路径目录名，默认为 ../views，表示上一级目录的views文件夹内，如需更改为 上一级目录的 view 目录下，则如下调用：
		setViewDir('../view');
	
	/* 设置模板的模板名路径 */
	public function setModuleDir($moduleName);
		参数：
			$moduleName:	默认为 "." 表示当前目录，除非有需要请勿更改。
	
	/* 渲染视图 */
	public function partial($template, $variables);
		参数：
			$template: 模板路径名称，相对于当前的模块而言
			$variables: 在模板中使用的变量集合，数组类型
	
### Cs\net\Request 类 ###

	/* 获取 host 信息 */
	public function getHttpHost();
			
	/* 获取 User Agent 信息 */
	public function getHttpUserAgent();
	
	/* 获取 Server Name 信息 */
	public function getServerName();
			
	/* 获取 Server Addr 信息 */
	public function getServerAddr();
			
	/* 获取 Remote Port 信息 */
	public function getRemotePort();
			
	/* 获取 Remote Addr 信息 */
	public function getRemoteAddr();
			
	/* 获取 Request Scheme 信息 */
	public function getReqeustScheme();
			
	/* 获取 Server Protocol 信息 */
	public function getServerProtocol();
			
	/* 获取 Document Root 信息 */
	public function getDocumentRoot();
			
	/* 获取 Request Uri 信息 */
	public function getRequestUri();
			
	/* 获取 Script Name 信息 */
	public function getScriptName();
			
	/* 获取 PATH-INFO 信息 */
	public function getPathInfo();
			
	/* 获取Query String 信息 */
	public function getQueryString();
			
	/* 是否GET请求 */
	public function isGet();
				
	/* 是否 PUT 请求 */
	public function isPut();
						
	/* 是否PATCH请求 */
	public function isPatch();
						
	/* 是否DELETE请求 */
	public function isDelete();
						
	/* 是否HEAD请求 */
	public function isHead();
						
	/* 是否OPTIONS请求 */
	public function isOptions();
						
	/* 获取 $_GET 参数 */
	public function get();
		
	/* 获取 $_POST 参数 */
	public function getPost();
				
### Cs\net\Response 类 ###

	/* 构造函数 */
	public function __construct();
	
	/* 设置RESPONSE HEADER */
	public function setHeader($headerName, $headerValue);
		参数：
			$headerName:	HTTP header 信息
			$headerValue:    HTTP header值，如：
		setHeader('Content-Type', 'application/json;charset=UTF8');
	
	/* 取消设置的HTTP header */
	public function unsetHeader($heaerName);
		参数：
			$headerName : 通过setHeader设置的 $headerName 值
	
	/* 发送HTTP内容与http header */
	public function send();
	
	/* 设置响应内容为JSON */
	public function setJsonContent($content);
		参数：
			$content:	需要响应的JSON内容， 如：
		setJsonContent(['status' => 'OK', 'data' => ['a', 'b', 'c'], 'code' => '200]);
		
	/* 设置原始响应内容 */
	public function setRawContent($content);
		参数：
			$content:	需要响应的内容， 如：
		setRawContent('<h1>Hello CSpeed</h1>');

	/* 进行URL跳转 */
	public function redirect($url);
		参数：
			$url:	需要跳转的URL地址
		
### \Cs\mvc\Model类 ###

	/* 构造函数 */
	public function __construct();
	
	/* 魔术方法 */
	public function _-set($key, $value)；
	
	/* 获取更新模型的Model对象 */
	public static function find();
	
	/* 设置需要操作的数据表 */
	public function tableName();

	/* 设置WHERE条件 */
	public function where($where);

	/* 设置WHERE条件 */
	public function andWhere($where);

	/* 设置ORDER BY条件 */
	public function orderBy($orderBy);

	/* 设置GROUPBY条件 */
	public function groupBy($groupBy);

	/* 获取一条记录 */
	public function one();

	/* 获取所有记录 */
	public function all();

	/* 更新或者新增记录 */
	public function save();

	/* 删除记录*/
	public function delete();

### \Cs\tool\Config 类 ###
	
	/* 构造函数 */
	public funtoin _-construct();

	/* 加载INI文件解析成数组 */
	public function loadConfig($configFile);

	/* 获取所有ini文件解析后的数组数据 */
	public function getConfigs();

	/* 获取所有ini文件解析后的数组数据对应的关联数据的数据值 */
	public function getConfig($configKey);













































	
