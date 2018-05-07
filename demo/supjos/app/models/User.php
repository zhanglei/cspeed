<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 2018/3/24
 * Time: 下午7:46
 *
 * Hello developers, This is the model file to operate the table ```www_product```, you can change the tableName method
 * 's return string to the real table as you want. for example:
 *
 * function tableName()
 * {
 *     return 'users';
 * }
 *
 * if you do the before, the model Tools will to operate the table 'users'
 *
 * A simple model like this:
 *
 * namespace app\models;
 *
 * class Tools extends \Cs\mvc\Model
 * {
 *     function tableName()
 *     {
 *         return 'users';
 *     }
 * }
 *
 * NOTE THAT:
 *
 *     If you want to set the default adapter to the current model, you may to do it in the initialise function. such as:
 *
 * function initialise()
 * {
 *     $this->setDb('db');
 * }
 *
 */

namespace app\models;

class User extends \Cs\mvc\Model
{
    public $abc;
    private $linux;
    protected $macos;

    private $tools;
    private $tools_two;

    function setName($tools)
    {
        $this->tools = $tools;
    }

    function __construct()
    {
        parent::__construct();
    }

	function tableName()
	{
		return "www_product";
	}
}