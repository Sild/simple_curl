<?php
spl_autoload_register(function($class) {
    include_once "$class.php";
});
$controller = new Controller();
$controller->run_example();

