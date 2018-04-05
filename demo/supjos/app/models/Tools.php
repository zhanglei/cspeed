<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 2018/3/24
 * Time: 下午7:46
 */

namespace app\models;

class Tools extends \Cs\mvc\Model
{
    private $data;

    function setData($data)
    {
        $this->data = $data;
    }

    function tableName()
    {
        return "www_product";
    }
}