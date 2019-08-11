<?php
/**
 * Скрипт эмулирующий ответы устройства для быстрого тестирования html/css/javascript 
 * без необходимости перезаливать прошивку на устройство. 
 */
switch ($_SERVER['REQUEST_URI']) {
    case '/':
        echo file_get_contents(__DIR__ . '/index.html');
        break;
    case '/style.css':
        header('Content-Encoding: gzip');
        echo file_get_contents(__DIR__ . '/skeleton.min.css.tar.gz');
        break;
    case '/favicon.png':
        header('Content-Type: image/png');
        echo file_get_contents(__DIR__ . '/favicon.png');
        break;
    case '/aplist':
        echo 'one,two,three,for';
        break;
    default:
        echo '404 not found';
}