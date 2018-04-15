# CSpeed 基于C语言的扩展框架 v2.1.11手册 #

## 快速上手 ##

开发者只需要下载源码，源码目录包含有一个 ”**demo**“ 目录，即可快速上手框架来完成开发工作

## 环境 ##

**CSpeed** 基于 **PHP7.x** 开发，使用环境要求：

**1、Linux/mac OSX**

**2、PHP7.x**

## 安装 CSpeed v2.1.11 ##

**CSpeed** 扩展目前在 **Github** 与 **码云** 平台均有代码存储库，用户只需下载源码然后按照如下方法安装即可：

Github:

	https://github.com/liqiongfan/cspeed
码云：

	https://gitee.com/josinli/cspeed
安装步骤：

```php
1、/usr/local/php_to_path/bin/phpize

2、./configure --with-php-config=/usr/local/php_to_path/bin/php-config

3、make install 
```

编译完成后在　php.ini　配置文件添加如下内容：

```php
extension_dir = "/usr/local/php-7.1.8-nts/lib/php/extensions/no-debug-non-zts-20160303/"

extension=cspeed.so
```
然后重启 Nginx 的 PHP-FPM 或者 Apache：

```php
4、systemctl restart php-fpm 或者 systemctl restart httpd
```

## 框架特性 ##

### 1、JSON-RPC 分布式支持 ###

**服务端代码：**

```php
namespace app\modules\rpc\controllers;

class Goods extends \Cs\rpc\Server
{
	
	function initialise()
	{
		// handle函数绑定 RPC 服务端处理类
		$this->handle($this);
	}
	
	// ----- 所有的对外的 RPC 方法
	
	/**
	 * 每一个 RPC 方法都包含一个 参数（本方法使用：$params）用来接收客户端的发送的请求信息
	 * $params是一个数组，数组的每一项对应于请求参数的每一项
	 * 如果需要将数据返回给客户端，只需要将所需数据 return 即可.
	 */
	function listsRpc($params)
	{
		return [ 'version' => '2.1.11', 'kernel' => 'CSpeed' ];
	}
	
	/**
	 * 添加商品服务
	 */
	function addRpc($params)
	{
		// TODO ..
	}
	
	/**
	 * 删除商品服务
	 */
	function deleteRpc($params)
	{
		// TODO...
	}
	
	// 等等服务代码
}

```

**客户端代码：**

```php

// 绑定RPC的服务端地址，注意地址必须是有效的地址，且携带 http 或者 https 前缀
$goodsServer = new \Cs\rpc\Client("http://www.xxx.com/index/goods");

// $lists就是服务器端返回的符合RPC规范的数据，开发者需要自行解析，如下示例：
// {"jsonrpc":"2.0","id":1,"result":{"version":"2.1.11","name":"CSpeed"}}
$lists = $goodsServer->lists();
```
**传入单个参数，如：**

```php

$goodsServer = new \Cs\rpc\Client("http://www.xxx.com/index/goods");
$lists = $goodsServer->lists(['id' => 11, 'category' => 'fruit']);

```

在服务端的方法内参数就是一个包含如下变量的数组：

```php

namespace app\modules\rpc\controllers;

class Goods extends \Cs\rpc\Server
{
	
	function initialise()
	{
		// handle函数绑定 RPC 服务端处理类
		$this->handle($this);
	}
	
	/**
	 * 参数 $params 就是一个数组，数组的内容如下：
	 * $params = [
	 *     [
	 *         'id'       => 11,
	 *         'category' => 'fruit
	 *     ]
	 * ]
	 */
	function listsRpc($params)
	{
	
	}
	
}
```

**也可以传入多个参数：**


```php

$goodsServer = new \Cs\rpc\Client("http://www.xxx.com/index/goods");
$lists = $goodsServer->lists(11, 'cspeed');

```

在服务端的方法内参数就是一个包含如下变量的数组：

```php

namespace app\modules\rpc\controllers;

class Goods extends \Cs\rpc\Server
{
	
	function initialise()
	{
		// handle函数绑定 RPC 服务端处理类
		$this->handle($this);
	}
	
	/**
	 * 参数 $params 就是一个数组，数组的内容如下：
	 * $params = [
	 *     11,
	 *     'cspeed'
	 * ]
	 */
	function listsRpc($params)
	{
	
	}
	
}
```

### 2、解耦工厂： Cs\ObjectFactory ###

当一个系统越来越大、复杂的时候，这时候面对一个庞大复杂的系统，更改一个小功能都可能会导致需要进行无数次的代码更换或者重构，如果这时候使用 **文件解耦** 来进行处理，则后续工作都可以不用操作，全部交给 **CSpeed** 系统来完成，既方便有简单，并且性能也高。

Cs\ObjectFactory类只有两个方法:

```php
function __construct($factoryFilePath)
{
}

// $key 就表示下方的 config.php 中的 rsa 与 aes 
function getObject($key)
{
}
```

**CSpeed** 解耦的配置文件配置如下所示[ **config.php** ]：

```php

return [
    'rsa' => [
        'class'      => '\app\models\User',
        'properties' => [
            'name' => '\app\models\Tools',
            'two'  => '\app\models\Tools'
        ],
        'values'     => [
            'abc'     => new stdClass(),
            'linux'   => 'Linux',
            'windows' => 'Windows',
            'macos'   => 'macOSX'
        ],
        'attrs'       => [
            'private' => false,
        ]
    ],
    'aes' => [
        'class'      => 'Cs\tool\Config',
        'properties' => [
            'data' => 'app\models\User'
        ]
    ]
];
```
**用户只需要在 配置文件里面 返回一个指定格式的数组，即可**

具体的格式如下：

数组的每一项是一个**关联键值对，键对应于类表示ID，后续可以用来从CSpeed中获取对应的类对象，值表示一个数组，此数组对应于让CSpeed怎么初始化类，此数组格式如下：**

```php
class 表示：类对象，是一个包含命名空间的类名称
properties 表示：使用IOC setter方法注入的对象属性，是一个数组，数组的每一项对应于一个 setter 方法。
values 表示：初始化类对象的属性值
attrs 表示：哪种属性的类对象可以初始化，可选值：private、protected、private
```

**properties数组**

```php
'properties' => [
    'name' => '\app\models\Tools',
    'two'  => '\app\models\Tools'
]
```

**数组的每一项对应于一个 setter 方法，setter方法名为数组中每一项的键名，每一项的值表示setter方法注入的对象的类名称(包含命名空间)，如上面所示，两个setter方法分别对应如下的PHP代码：**


```php
function setName(new \app\models\Tools());

function setTwo(new \app\models\Tools());
```

**attrs数组**

对应于怎么初始化类对象的属性

```php
'attrs'       => [
    'private' => false,
]
```
上面就表示类对象的 **private** 属性不使用指定值初始化(不影响内部的private属性：OOP的封装特性)，**protected、public**属性使用指定值进行初始化


### 3、观察者事件机制 Cs\tool\Component ###

当在开发一个拥有很多功能的系统或者模块的时候，可能需要处理很多相同的业务逻辑，通常的处理手法是： 

**1、使用继承，父类进行处理**

```php

class Commmon
{
	function __construct()
	{
		// TODO...
	}
}

class User extends Common
{
	function __construct()
	{
		parent::__construct();
	}
}

class Manager extnds Common
{
	function __construct()
	{
		parent::__construct();
	}
}
```

**2、使用事件机制**

```php

class Test
{
	function __construct()
	{
		$this->on('before', funtion($obj){
			// TODO...
		});
	}
}
```

**CSpeed** 支持观察者事件机制，可以通过 **Cs\tool\Component** 来进行支持

**Cs\tool\Component** 包含如下三个方法：

```php
function on($eventName, $eventClosure);

function off($eventName, $eventClosure = NULL);

function trigger($eventName);
```

CSpeed框架继承 **Cs\tool\Component** 的类如下三个：

```php
Cs\mvc\Controller	
Cs\mvc\Model				
Cs\rpc\Server			
```

开发者只需要在如上三个类中的 **initialise** 方法内绑定事件即可, **注意：initialise方法在 CSpeed框架中，当存在此方法时，框架会从顶级父类开始执行到本类的initialise方法来完成初始化,也就是会先执行父类的initialise，然后执行子类initialise，直到本类的initialise方法**，如：

```php
namesapce app\modules\admin\controllers;

class User extends \Cs\mvc\Controller
{
	function initialise()
	{
		$this->on(self::EVENT_BEFORE_ACTION, function($object){
			// TODO...
		});
	}
}
```

### 4、统一入口初始化： Cs\BootInit接口 ###

用户只需要定义一个 **BootInit** 的类，不适用命名空间，并且实现 **Cs\BootInit** 接口，然后把 **BootInit** 类的文件放入配置文件，即可让系统自动完成整套系统的初始化工作：

```php
class BootInit implements Cs\BootInit
{
	// TODO...
	// 在此类中的以指定的前缀开头的方法都会被依次执行，如在配置文件指定方法前缀为：__init
	// 那么本类中所有的以 __init 开头的方法都会被执行
	function __initDi($di, $router)
	{
		// TODO... 
	}
	
	function __initRouter($di, $router)
	{
		// TODO...
	}
	
	function __initIOC($di, $router)
	{
		// TODO...
	}
}
```

### 5、模型支持切换数据库 ###

在复杂的系统开发中，定制化的开发对应自身的系统开发具备非常优秀的开发效率，也便于自身的团队协作。
有时候针对于数据库会进行简单化的读写分离，那么分离后的代码结构可能会针对不同的模块进行二级分组等来提高系统的开发效率。如：

目录结构如下：

```php
+app
  ++models
    ++read
      ++User
      ++Info
    ++write
      ++User
      ++Info
```

在 **CSpeed** 中，如果需要达到上面的代码架构，很简单，用户只需要在模型的构造函数里面，绑定相应的数据库即可。如：

```php
namesapce app\models\read;

class User extends \Cs\mvc\Model
{
	function __construct()
	{
		// 构造函数内，必须显式执行父类的构造函数来完成初始化
		parent::__construct();
		
		// 设置当前模型使用 read 数据库连接
		$this->setDb('read');
	}
}
```

在使用 **setDb** 注入数据库的时候，需要先事先注入相应的数据库连接：

```php

$di->set('read', function(){
	return new \Cs\db\pdo\Adapter([
		'dsn' => 'mysql:host=localhost;port=3306;dbname=supjos',
		'username' => 'root',
		'password' => 'root',
		'options'  => [                   // options参数非必填
           \PDO::ATTR_PERSISTENT => true, // 持久连接
           \PDO::ATTR_AUTOCOMMIT => 0     // 默认不自动提交
       ]
	]);
});
```

### 6、强悍的路由 ###

任何一个框架都应该有一套路由规则，以便用户可以自定义路由的走向来完成特定的功能。

CSpeed 框架支持 **配置文件加载路由** 与直接 **添加路由** 来完成

**1、配置文件加载路由[ini文件]**

如 [ **router.ini** 文件]：

```php
/a/b/c=/b/c/a

/d/e/:controller:=/aa/bb/$1

/cspeed/version/:action:=/cspeed/{$1}/do

/index/index/index = /say/index/index
```
路由文件中的每一行代表一个路由规则，注意每一条路由规则不能包含空格，等号的左右分别代表 **来源路由** 与 **重定向路由**， 来源路由表示URL地址栏输入的路由，重定向路由表示：当来源路由匹配的时候，需要重定向到的路由URL。如第一条表示：
当来源路由是：```/a/b/c```的时候，重定向到 ```/b/c/a```路由，第二条规则，表示：
当来源路由是：```/d/e/hello```的时候，重定向到：```/cspeed/hello/do```，其中的```:controller:```表示 **预定义正则**，含义与正则表达式```([^0-9-][a-zA-Z0-9-]*)```一样，```$1```就表示上面正则```([^0-9-][a-zA-Z0-9-]*)```匹配的url

**CSpeed** 预定义的正则如下所示：

```php
:action:、:controller:、:module: 			表示 ([^0-9-][a-zA-Z0-9-]*)
:any:									      表示 ([^/]+)
:id:									      表示 ([0-9]+)
```

### 7、命令行模式支持 ###

注意：**CSpeed** 框架支持的命令行模式的控制器类是不支持使用命名空间的控制器类。

一个命令行模式的CSpeed应用只需要如下两步即可：

**1、编写入口文件[index.php]**

```php
$task = new \Cs\console\Task();

// 或者也可以使用参数形式初始化系统，参数格式与 Cs\App 一致
$task = new \Cs\console\Task("xxx/xxx.ini", "dev");

// 运行命令行
$task->run($argv[1]);
```

**2、在bash目录运行如下命令**

```php
php -f index.php hello/world/cspeed
```
来执行 **hello**模块 **world**控制器 **cspeed**方法

**注意：在CSpeed中方法以Action或者Rpc结尾，以Action结尾的方法表示路由方法，Rpc结尾的方法是RPC服务端的暴露方法**

### 8、完善的ActiveRecord特性 ###

在应用的开发过程中，都会遇到不想手工编写SQL的烦恼，以及面临SQL注入的危险，这时候使用框架封装的方法来执行SQL以牺牲性能来换取便利与安全。

如,假设数据有 www_product 表，表中有四个字段：time, name, version, id，如下建立模型：

```php
namespace app\models;

class Good extends \Cs\mvc\Model
{
	// 不建议用户使用构造函数，务必记住，如果定义构造函数，一定需要调用父类的
	// 构造函数
	function __construct()
	{
		parent::__construct();
	}
	
	// 使用本方法完成一些初始化工作
	function initialise($object)
	{
	    // TODO...
	    // 此方法只要存在就一定执行，不管是new模型还是find获取模型
	    // [注意]：构造函数只有在new模型的时候执行。
	    // 方法传入了本类的对象(此处是$object代表Good对象)作为参数
	}
	
	function tableName()
	{
		return 'www_product';
	}
}
```

在 **CSpeed框架中，默认情况下：one() 函数返回类对象，all() 函数返回类对象数组，数组中的每一项代表一个类对象，方便用户进行 AR 操作。如果需要获取数组格式的查询数组，请调用类的 asArray()方法后调用相应的 one()或者 all()方法。**

**CURD**

**添加一条记录[C]：**

```php
$good = new \app\models\Good();

$good->time = date('Y-m-d H:i:s');
$good->name = 'CSpeed';
$good->version = '2.1.11';
$good->id = 1;
$good->has = 1;

if ($good->save()){
	// TODO...
	// 成功添加记录
} else {
   // TODO...
   // 添加记录失败
}
```

**更新记录[U]：**

```php
$good = \app\models\Good::find();

if ($good->where([ 'id' => 1 ])->save([
	'name'   => 'CSpeed-V2.1.11',
	'version => 'v2.1.11' 
])){
	// TODO...
} else {
	// TODO...
}

```

**查找记录[R]**

```php
$good = \app\models\Good::find();

// 返回对象
$lists = $good->orderBy(['id DESC', 'age ASC'])->all();

// 返回数组
$lists = $good->orderBy("id DESC")->asArray()->all();

// 查询一条记录
$lists = $good->orderBy(['id DESC', 'age ASC'])->one();

//返回数组
$lists = $good->orderBy(['id DESC', 'age ASC'])->asArray()->one();
```

**删除记录[D]：**

```php
$good = \app\models\Good::find();

if ($good->where([ 'id' => 1 ])->delete()){
	// TODO...
} else {
	// TODO...
}

```

### 9、最简单的MVC框架 ###

在 **B/S** 架构开发中， **MVC** 开发模式无疑最快、效率最高。现代的软件开发中：提高代码重用、增加开发速度和减少维护修改量无疑作为了一个重点。

## CSpeed 性能 ##

**测试命令：**

```php
/usr/local/httpd-2.4.29/bin/ab -c100 -n100000 http://www.supjos.com/hello/cspeed/
```
**测试结果：**

```php
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
```

## PhpStorm 代码提示 ##

如果用户需要使用 **PhpStorm** 的代码提示功能，只需要下载源码，然后根据 **IDE** 中的 **README.md** 文档要求操作即可

## QQ群交流 ##

https://jq.qq.com/?_wv=1027&k=5kSunAR

欢迎 star 或者给与宝贵的意见与建议，您的建议是 **CSpeed** 前进的动力。

**CSpeed** 扩展官方QQ群号： **605383362**





















































