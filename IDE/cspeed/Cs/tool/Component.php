<?php
/**
 * Created by PhpStorm.
 * User: Josin
 * Date: 17-11-11
 * Time: 上午8:10
 */

namespace Cs\tool;

class Component
{
    /**
     * Component constructor.
     * To construct the Component object.
     */
    function __construct()
    {
    }

    /**
     * To add an event to the object
     * @param $eventName      string    The EVENT_NAME which you want to add to the event_queue_lists
     * @param $eventCallBack  \Closure  The EVENT callback invoked when the event is triggered.
     */
    function on($eventName, $eventCallBack)
    {
    }

    /**
     * To detach the event with the given event_name
     * @param string        $eventName      The event which you want to detach
     * @param \Closure|NULL $eventCallBack  The handler associated with the event_name,NULL means to remove all
     */
    function off($eventName, $eventCallBack = NULL)
    {
    }

    /**
     * To trigger the event named `$eventName`
     * @param string $eventName  The event which you want to trigger
     */
    function trigger($eventName)
    {
    }
}