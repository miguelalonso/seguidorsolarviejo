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
 $valores=read();

$data = array(
        'year' => date("Y-m-j G:i:s"),
        'ano' => floatval(round($valores[0],0)),
        'mes' => floatval(round($valores[1],0)),
        'dia' => floatval(round($valores[2], 1)),
        'hora' => floatval(round($valores[3], 1)),
        'min' => floatval(round($valores[4], 1)),
        'seg' => floatval(round($valores[5], 1)),
        'acimut_sol' => floatval(round($valores[6], 2)),
        'cenit_sol' => floatval(round($valores[7], 2)),
        'acimut_actual' => floatval(round($valores[8], 2)),
        'cenit_actual' => floatval(round($valores[9], 2)),
        'manual' => floatval(round($valores[10],0)),
        'manual_giraEste' => floatval(round($valores[11], 0)),
        'manual_giraOeste' => floatval(round($valores[12], 0)),
        'manual_giraNorte' => floatval(round($valcsores[13], 0)),
        'manual_giraSur' => floatval(round($valores[14], 0)),
        'FC_este' => floatval(round($valores[15], 0)),
    'FC_oeste' => floatval(round($valores[16], 0)),
    'FC_norte' => floatval(round($valores[17], 0)),
    'FC_sur' => floatval(round($valores[18], 0)),
    'error' => floatval(round($valores[19], 0)),
    'iniciado' => floatval(round($valores[20], 0)),
    'iniciado_Este' => floatval(round($valores[21], 0)),
    'iniciado_Norte' => floatval(round($valores[22], 0)),
    'estado_giroE' => floatval(round($valores[23], 0)),
    'estado_giroO' => floatval(round($valores[24], 0)),
    'estado_giroN' => floatval(round($valores[25], 0)),
    'estado_giroS' => floatval(round($valores[26], 0))
    );

    header("Content-type: text/json");
    //echo json_encode($data);

echo json_encode($data);
?>
