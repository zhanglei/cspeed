<?php
/**
 * CSpeed is a easy-famous WEB-API framework based on PHP7.x or later.
 * You can use CSpeed to develop the projects, such as the Website、E-Shop and so on...
 */
 
 $app = new \Cs\App('../app/config/core.ini');
 $app->bootstrap()->run();