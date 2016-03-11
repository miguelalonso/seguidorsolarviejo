<?php

function microtime_float()
{
        list($usec, $sec) = explode(" ", microtime());
        return ((float)$usec + (float)$sec);
}




function write($comando)
{
    include "php_serial.class.php";
// Let's start the class
    $serial = new phpSerial;
    $serial->deviceSet("/dev/ttyUSB0");
    $serial->confBaudRate(9600);
    $serial->confParity("none");
    $serial->confCharacterLength(8);
    $serial->confStopBits(1);
    $serial->confFlowControl("none");

    $str='T';
    $serial->deviceOpen();
    $serial->sendMessage($str,1);
    sleep(3);
    $date=date("Y,m,d,h,i,s,");
    $str=$date;
    $serial->sendMessage($str,1);
    $serial->deviceClose();
    echo 'OK';
    return 0;
}

if (!empty($_GET['comando'])){
    $comando=$_GET['comando'];
}else{
    $comando='';
}
 //$valores=read();
$v=write($comando);


    header("Content-type: text/json");
    //echo json_encode($data);

echo json_encode($v);
sleep(2);
header("Location:serial_arduino_home.php")
?>
