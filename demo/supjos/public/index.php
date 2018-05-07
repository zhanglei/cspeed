<?php

/**
 * Hello Everyone, Welcome to use the CSpeed framework based on PHP7.x written in C code.
 *
 * A simple, fast PHP framework to simplify the developer's job.
 *
 * An open source extension, every one can modify, copy it by yourself, but must to notify the author: Josin
 * email: 774542602@qq.com or liqiongfanck@163.com
 *
 * Thank you, wish you have a nice tour in CSpeed.
 */

$app = new \Cs\App("../app/config/core.ini");

/**
 * Run the router dispatch job.
 */
$app->bootstrap()->run();