<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 2018/4/9
 * Time: 10:45
 */

namespace app\models;

use Cs\mvc\Model;

/**
 * Class User
 * @package app\models
 */
class User extends Model
{
    /**
     * User constructor.
     */
    public function __construct()
    {
        parent::__construct();
    }

    function tableName()
    {
        return 'www_product';
    }
}