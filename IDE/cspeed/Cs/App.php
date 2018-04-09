<?php

namespace Cs;


class App extends tool\Component
{
    /**
     * Some event which the App class supports
     */
    const EVENT_BEFORE_REQUEST = "EVENT_BEFORE_REQUEST";
    const EVENT_AFTER_REQUEST = "EVENT_AFTER_REQUEST";

    /**
     * @var App $app
     */
    public static $app;

    /**
     * App constructor.
     * @param string $iniFilePathName   The INI file which you want to initialise the App class core
     * @param string $iniFileNodeName   The INI file node name which the CSpeed to load the configs
     */
    function __construct($iniFilePathName = '', $iniFileNodeName = '')
    {
    }

    /**
     * Match GET request
     * @param string   $url     The url which you want to match from the request
     * @param \Closure $closure The Closure will be invoked when the matching process success
     */
    function get($url, $closure)
    {
    }

    /**
     * Match POST request
     * @param string   $url     The url which you want to match from the request
     * @param \Closure $closure The Closure will be invoked when the matching process success
     */
    function post($url, $closure)
    {
    }

    /**
     * Match PUT request
     * @param string   $url          The url which you want to match from the request
     * @param \Closure $closure      The Closure will be invoked when the matching process success
     */
    function put($url, $closure)
    {
    }

    /**
     * Match PATCH request
     * @param string   $url          The url which you want to match from the request
     * @param \Closure $closure      The Closure will be invoked when the matching process success
     */
    function patch($url, $closure)
    {
    }

    /**
     * Match DELETE request
     * @param string   $url          The url which you want to match from the request
     * @param \Closure $closure      The Closure will be invoked when the matching process success
     */
    function delete($url, $closure)
    {
    }

    /**
     * Match HEAD request
     * @param string   $url          The url which you want to match from the request
     * @param \Closure $closure      The Closure will be invoked when the matching process success
     */
    function head($url, $closure)
    {
    }

    /**
     * Match OPTIONS request
     * @param string   $url          The url which you want to match from the request
     * @param \Closure $closure      The Closure will be invoked when the matching process success
     */
    function options($url, $closure)
    {
    }

    /**
     * Autoload function, invoked by CSpeed extension
     * @param string $className WHICH file you want to load
     */
    private function autoload($className)
    {
    }

    /**
     * Setting the alias which you want let the CSpeed to load the class file automatically
     * @param string $aliasKey         The alias key, must be begin with '@' character.
     * @param string $aliasFullPath    The full path assigned by the alias key
     */
    function setAlias($aliasKey, $aliasFullPath)
    {
    }

    /**
     * Run the WEB MVC process
     */
    function run()
    {
    }

    /**
     * @return App
     */
    function getApp(): \Cs\App
    {
    }

    /**
     * @return $this
     */
    function bootstrap()
    {
        return $this;
    }

    /**
     * To use the Composer loade
     * @param string $file
     */
    function setComposerLoader($file)
    {
    }
}