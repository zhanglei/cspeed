<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午8:09
 */

namespace Cs\net;

class Request
{
    /**
     * Get the HTTP_HOST info
     * @return string
     */
    function getHttpHost()
    {
    }

    /**
     * Get the HTTP USER AGENT
     * @return string
     */
    function getHttpUserAgent()
    {

    }

    /**
     * Get the SERVER_NAME
     * @return string
     */
    function getServerName()
    {

    }

    /**
     * Get the SERVER_ADDR
     * @return string
     */
    function getServerAddr()
    {

    }

    /**
     * Get the REMOTE_PORT
     * @return string
     */
    function getRemotePort()
    {

    }

    /**
     * Get the REMOTE_ADDR
     * @return string
     */
    function getRemoteAddr()
    {

    }

    /**
     * Get the REQUEST_SCHEME
     * @return string
     */
    function getReqeustScheme()
    {

    }

    /**
     * Get the SERVER_PROTOCOL
     * @return string
     */
    function getServerProtocol()
    {

    }

    /**
     * Get the DOCUMENT_ROOT
     * @return string
     */
    function getDocumentRoot()
    {

    }

    /**
     * Get the REQUEST_URI
     * @return string
     */
    function getRequestUri()
    {

    }

    /**
     * GET the SCRIPT_NAME
     * @return string
     */
    function getScriptName()
    {

    }

    /**
     * GET the PATH_INFO
     * @return string
     */
    function getPathInfo()
    {

    }

    /**
     * GET THE QUERY_STRING
     * @return string
     */
    function getQueryString()
    {

    }

    /**
     * Whether is GET request or not
     * @return bool
     */
    function isGet()
    {

    }

    /**
     * Whether is PUT request or not
     * @return bool
     */
    function isPut()
    {

    }

    /**
     * Whether is PATCH request or not
     * @return bool
     */
    function isPatch()
    {

    }

    /**
     * Whether is DELETE request or not
     * @return bool
     */
    function isDelete()
    {

    }

    /**
     * Whether is HEAD request or not
     * @return bool
     */
    function isHead()
    {

    }

    /**
     * Whether is OPTIONS request or not
     * @return bool
     */
    function isOptions()
    {

    }

    /**
     * Get the $_GET parameters
     * @param $key string                    The $_GET key which you want to get the value
     * @param $filterClosure string|\Closure The filter which you want to filter the data, such as:
     * <pre>
     *  $request = new \Cs\net\Request();
     *  $data = $request->get('age', function($age){
     *   // TODO.
     * });
     *
     * $idCard = $request->get('idcard', 'strtoupper');
     * </pre>
     * @return mixed|string|array
     */
    function get($key, $filterClosure)
    {

    }

    /**
     * Get the $_POST parameters
     * @param $key              string          The $_POST key which you want to get the value from
     * @param $filterClosure    string|\Closure The filter which you want to filter the data
     * @return string|array|mixed
     */
    function getPost($key, $filterClosure)
    {

    }
}