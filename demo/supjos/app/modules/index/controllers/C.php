<?php

/**
 * Hello, The following is the RPC server example.
 *
 * NOTE THAT:
 *
 * This controllers' method must be POST requested. otherwise failure.
 *
 * recommend to use the curl command or Postman to do the job.
 */

namespace app\modules\index\controllers;

class C extends \Cs\rpc\Server
{
    /**
     * Initialise job.
     *
     * RPC server must kown that: in initialise method to bind the object to do the request.
     *
     * $this->handle($this)
     *
     * or
     *
     * $this->handle($xxx)
     *
     * To bind the object to do the RPC request.
     */
    function initialise()
    {
        $this->handle($this);
    }

    /**
     * RPC method end with 'Rpc' suffix. with a parameter to receive the request data.
     * @param $param
     * @return array
     */
    function listRpc($param)
    {
        if ($param['data'] == 22) {
            return ['status' => '0000', 'info' => 'SUCCESS'];
        }

        return $param;
    }

    /**
     * RPC method end with 'Rpc' suffix. with a parameter to receive the request data.
     * @param $data
     * @return array
     */
    function twoRpc($data)
    {
        return $data;
    }
}