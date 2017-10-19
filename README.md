# CSpeed micro framework v1.2.1 #

----------

## 最新版本特性  ##

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
	
	自本版本开始README文件不提供API索引，开发者可以通过源码里面的IDE目录提供的对应的函数进行查阅。
	
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

	扩展只支持PHP7.x以上版本，低于PHP7.x一下的版本请先升级PHP版本

	在WEB应用模式下，扩展通过解析 PATH_INFO 参数信息进行路由转发，请先确保 WEB服务器支持 PATH_INFO 模式，并且需要隐藏index.php
	否则系统不生效,无法完成路由解析。

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

## 简单的 WEB **示例** ##
	
WEB 目录设置为如下：

	+public
	|----index.php  		入口文件
	+controllers
	|---Index.php			Index默认控制器
	+admin				新增admin模块
	|---controllers 		admin模块下的控制器目录
	      |-----Index 		admin模块的Index控制器
	+fronted			fronted模块
	|---controllers		        fronted模块控制器目录
		  |-----Index		fronted模块Index控制器


public 目录下 index.php 内容如下：
	
	$app = new \Cs\App();

	$app->run();
	/* 超简单的框架已经完成 */

在 public 同级的目录下创建一个 Index.php 文件,内容如下：
	
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
	
	+--cspeed							网站目录
		+--public
			|---------index.php				入口文件
		+--controllers						默认控制器加载目录
			|---------Index.php
			|---------Goods.php
		+--backend						新增模块backend
			|---------controllers				backend模块的控制器目录
				  |--------Index.php			
				  |--------Goods.php							
		

	如果目录结构如上所示：
	
	那么 默认的 ```app``` 别名指向 cspeed 目录。
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


## CSpeed 框架的视图引擎 ##
	
	$view = new \Cs\mvc\View();

	/* 方法包含两个参数，
	 * 第一个参数为：需要渲染的视图的文件名
	 * 第二个参数为：需要在视图中使用的变量，传入一个数据或者不传入
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
	
	
