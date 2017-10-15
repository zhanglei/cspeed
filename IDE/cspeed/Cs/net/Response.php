<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-10-15
 * Time: 下午8:16
 */

namespace Cs\net;

class Response
{
    /**
     * Response constructor.
     */
    function __construct()
    {

    }

    /**
     * Set the HTTP_HEADERS
     * For example setHeader('Content-Type', 'application/json;charset=UTF-8');
     * @param $headerKey        THE HTTP HEADER KEY
     * @param $headerValue      THE HTTP HEADER VALUE
     */
    function setHeader($headerKey, $headerValue)
    {

    }

    /**
     * Unset the HTTP HEADERS
     * @param $headerKey        Delete the HTTP HEADER from the setHeader function before calling the send function
     */
    function unsetHeader($headerKey)
    {

    }

    /**
     * Send the HTTP header and response data to the client
     */
    function send()
    {

    }

    /**
     * Set the response data to the JSON format
     * @param array $jsonArray
     */
    function setJsonContent(array $jsonArray)
    {

    }

    /**
     * Set the response data to the raw format
     * @param $rawContent   The raw format of the response data
     */
    function setRawContent($rawContent)
    {

    }

    /**
     * Redirect the request to the $url
     * for example, redirect('/index/lists'); which will redirect the url to the http://host/index/lists
     * @param $url          Which url you want to redirect
     */
    function redirect($url)
    {

    }
}