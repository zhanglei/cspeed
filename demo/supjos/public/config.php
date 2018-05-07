<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 2018/3/24
 * Time: 下午4:59
 *
 *
 * This file was a IOC object configs, an array contains some rules to initialise the Object.
 *
 * if the config file is the following:
 *
 * return [
 *     'ras' => [
 *         'class' => 'app\models\User'
 *     ]
 * ];
 *
 * it means you want to create an object it class name was : app\models\User, NOTICE: the class name
 * must contains the namespace.
 *
 * ```php
 * 'properties' => [
 *      'name' => '\app\models\Tools',
 *      'two'  => '\app\models\Tools'
 *  ]
 * ```
 *
 * properties means to use the setter method to inject the object to the current object, with the setter function
 *
 * ```php
 * 'values'     => [
 *      'abc'     => new stdClass(),
 *      'linux'   => 'Linux',
 *      'windows' => 'Windows',
 *      'macos'   => 'macOSX'
 *  ],
 *  'attrs'       => [
 *      'private' => false,
 *      'public'  => true
 *  ]
 * ```
 *
 * The values & attrs array always appears the same time. it's use was to set the properties to the current object.
 * with the given rules, if private properties, not set it, public properties to set it with the given value.
 *
 * so, is easy to use? wish you have a nice tour in CSpeed. Have fun!
 *
 * -- Josin<liqiongfanck@163.com>
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
