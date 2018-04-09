<?php
/**
 * Created by PhpStorm.
 * User: josin
 * Date: 17-11-3
 * Time: 下午9:29
 */

namespace Cs\rpc;

class Client
{
    /**
     * Client constructor.
     * @param $url string The url which you want to send the RPC service
     * Note that: the url must begin with: [http://] or [https://], such as :
     * <pre>
     *    $client = new Client('https://www.supjos.cn/fronted/goods');
     * </pre>
     */
    public function __construct($url)
    {
    }
}