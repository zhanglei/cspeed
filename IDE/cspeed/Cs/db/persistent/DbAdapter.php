<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 2018/5/20
 * Time: 下午5:45
 */

namespace Cs\db\persistent;

class DbAdapter
{
    function __construct($options = [
        'sharedKey' => 'id',
        'hSize'     => 20,
        'tableName' => 'chat'
    ]) {

    }

    function createCommand($sql = '')
    {
        return '';
    }
}