<?php

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