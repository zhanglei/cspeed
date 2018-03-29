<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 2018/3/24
 * Time: 下午4:59
 */

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
            'private' => true,
            'public'  => true
        ]
    ],
    'aes' => [
        'class'      => 'Cs\tool\Config',
        'properties' => [
            'data' => 'app\models\User'
        ]
    ]
];
