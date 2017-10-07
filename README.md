# CSpeed micro framework written in C Code #

----------

## 开发环境 ##
	
	Linux kernel 4.4.x、PHP7.1.8、Nginx1.12.1

	扩展只支持PHP7.x以上版本，低于PHP7.x以下的版本请先升级PHP版本

	在WEB应用模式下，扩展通过解析 PATH_INFO 参数信息进行路由转发，请先确保 WEB服务器支持 PATH_INFO 模式，并且需要隐藏index.php
	否则系统不生效,无法完成路由解析。

	推荐的Nginx配置：
	
    location / {
	# try_files $uri $uri/ /index.php$uri;
        if (!-e $request_filename) {
            rewrite ^/(.*)$ /index.php/$1 last;
        }
	# 如果是二级目录请设置为二级目录形式
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
	|--index.php  	入口文件
	+controllers
	|--Index.php	Index默认控制器
	+admin		新增admin模块
	|--controllers 	admin模块下的控制器目录
	     |--Index 	admin模块的Index控制器
	+fronted	fronted模块
	|--controllers	fronted模块控制器目录
	     |--Index	fronted模块Index控制器


public 目录下 index.php 内容如下：
	
	$app = new \Cs\App();

	$app->run();
	/* 超简单的框架已经完成 */

在 public 同级的目录controllers下创建一个 Index.php 文件,内容如下：
	
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
	
	+--cspeed		网站目录
	    +--public
	       |--index.php	入口文件
	    +--controllers	默认控制器加载目录
	       |--Index.php
	       |--Goods.php
	    +--backend		新增模块backend
	       |--controllers	backend模块的控制器目录
	       |--Index.php			
	       |--Goods.php							
		

	如果目录结构如上所示：
	
	那么 默认的别名 `app` 指向 cspeed 目录。
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
	
	鉴于上一个版本中的数据库性能有影响，本版本正处于重构状态。开发中~敬请期待.

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
	
	public get($urlPattern, $closure);
	正则匹配 $urlPattern 的GET请求

	public post($urlPattern, $closure);
	正则匹配 $urlPattern 的POST请求

	public put($urlPattern, $closure);
	正则匹配 $urlPattern 的PUT请求

	public patch($urlPattern, $closure);
	正则匹配 $urlPattern 的PATCH请求

	public delete($urlPattern, $closure);
	正则匹配 $urlPattern 的DELETE请求

	public head($urlPattern, $closure);
	正则匹配 $urlPattern 的HEAD请求

	public options($urlPattern, $closure);
	正则匹配 $urlPattern 的OPTIONS请求

	public autoload();
	CSpeed框架的自动加载函数，切勿私自调用

	public setAlias($aliasName, $aliasPath);
	设置自动加载别名

	public run();
	CSpeed框架处理URL请求

### Cs\mvc\View 类 ###
	
	public __construct();
	构造函数
	
	public render($templateFile, $templateVariables);
	渲染视图
	
	public setSuffix($suffixName);
	设置视图的后缀
	
	public setVar($varName, $varValue);
	设置视图中需要使用的变量，本方法可以多次调用

	public getRender($templateFile, $templateVariables);
	获取渲染的视图内容

	public setViewDir($dir);
	设置视图渲染路路径
	
	public partial($templateFile, $templateVariables);
	视图内进行渲染视图,如：布局layouts

### Cs\net\Request 类 ###

	public getHttpHost();
	获取HTTP HOST信息

	public getHttpUserAgent();
	获取HTTP User Agent信息

	public getServerName();
	获取Server Name 信息

	public getServerAddr();
	获取Server Addr 信息

	public getRemotePort();
	获取 Remote Port 信息
	
	public getReqeustScheme();
	获取Request Scheme 信息

	public getServerProtocol();
	获取Server Protocol 信息

	public getDocumentRoot();
	获取Document Root 信息

	public getRequestUri();
	获取Request Uri 信息

	public getScriptName();
	获取Script Name 信息

	public getPathInfo();
	获取Path Info 信息
	
	public getQueryString();
	获取Query String 信息

	public isGet();
	是否GET请求

	public isPut();
	是否Put请求

	public isPatch();
	是否Patch请求

	public isDelete();
	是否DELETE请求

	public isHead();
	是否Head请求

	public isOptions();
	是否Options请求

	public get();
	获取$_GET参数

	public getPost();
	获取$_POST参数

### Cs\net\Response 类 ###

	public __construct();
	构造函数

	public setHeader($headerName, $headerValue);
	设置响应的HTTP HEADER

	public unsetHeader($headerKey);
	删除刚刚设置的HTTP HEADER, 此操作需在调用send方法前使用才有效

	public send();
	响应设置的HTTP HEADER与内容
	
	public setJsonContent(array $data);
	设置响应的内容为JSON格式

	public setRawContent($mixed);
	设置响应的内容为原始数据，不进行转换
