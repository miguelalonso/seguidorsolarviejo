<?php

function microtime_float()
{
        list($usec, $sec) = explode(" ", microtime());
        return ((float)$usec + (float)$sec);
}

function read()
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


    //echo("Started..\r\n");

    $serial->deviceOpen();
    // Or to read from
    $lectura='';
    $reasssd = '';
    $read2 = '';
    $theResult = '';
    $start = microtime_float();
    // 3 second limit to read
    while (!preg_match('/FIN/', $theResult) && (microtime_float() <= $start + 5)) {

        $lectura = $serial->readPort();
        $theResult .= $lectura;
        $lectura= '';
    }
    $serial->deviceClose();

    $arraysplit=explode("INI:", str_replace(" ","",$theResult));
    $theResult=$arraysplit[1];

    $valores=explode(",", str_replace(" ","",$theResult));
    $count = count($valores);
   //var_dump($valores);

//    echo("\r\n");
   // echo("Lectura finalizada:");
//    echo("\r\n");
   // echo $valores[0];
   //echo("\r\n");
    return $valores;
   //echo("FIN");
//    echo("\r\n");
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

    $str=$comando;
    $serial->deviceOpen();
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
?>
