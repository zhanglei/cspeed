<?php

namespace app\models;

class User extends \Cs\mvc\Model
{
	/**
	 * 如果使用本构造函数，请务必显式调用父类构造函数，以免发生一些特殊Bug
	 */
	function __construct()
	{
		parent::__construct();
	}

	/**
	 * 方法返回模型操作的数据库表名称
	 * 如果此方法不存在的话，默认的数据库表的名称是 模型 名称的小写
	 * 如本模型 User 则 对应的数据表是 user 表
	 */
	function tableName()
	{
		return "www_product";
	}
}