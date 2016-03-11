<head>
<meta charset = "utf-8">
      <meta http-equiv = "X-UA-Compatible" content = "IE = edge">
      <meta name = "viewport" content = "width = device-width, initial-scale = 1">
      
      <title>Seguidor Solar Ciemat</title>
      
      <!-- Bootstrap -->
      <link href = "//maxcdn.bootstrapcdn.com/bootstrap/3.3.1/css/bootstrap.min.css" rel = "stylesheet">
      
      <!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
      <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
      
      <!--[if lt IE 9]>
      <script src = "https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src = "https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
      <![endif]-->

<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.0/jquery.min.js"></script>
<style>
    .custom{
        width: 125px !important;
    }

</style>
</head>

<?php
/**
 * User: Miguel Alonso Abella
 * Date: 21/10/2015
 * Time: 16:24
 */

date_default_timezone_set('Europe/Madrid');

$startdate = (substr($ollog, -12, 4)) . "," . (substr($ollog, -8, 2)) . "-1," . (substr($ollog, -6, 2));
$stopdate  = (substr($lstlog, -12, 4)) . "," . (substr($lstlog, -8, 2)) . "-1," . (substr($lstlog, -6, 2));

if (!empty($_POST['date1'])) {
    $date1 = $_POST['date1'];
    list($dd, $mm, $yyyy) = explode('/', $date1);
    if (!checkdate($mm, $dd, $yyyy)) {
        $date1 = '';
    }
} else {
    $date1 = (substr($lstlog, -6, 2)) . "/" . (substr($lstlog, -8, 2)) . "/" . (substr($lstlog, -12, 4));
}

$titledate = substr($date1, 0, 10);
$csvdate1  = (substr($date1, 6, 4)) . (substr($date1, 3, 2)) . (substr($date1, 0, 2)) . ".csv";

$year = date("Y");
$day = date("d");

$month = date("m");
$monthname = date("M");
$date=date("d-M-Y  h:i:s");

//echo $date;
//<button type = 'button' class = 'btn btn-primary'> <a href='index_multi.php'></a>Button 7</button>
echo "
<div  class='container'><table>
<tr><td rowspan='4'><h2><span class = 'label label-primary lg'>$date</span></h2></td></tr>
<tr><td >: <span id = 'GG30' ></span> </td></tr>
<tr><td >. : <span id = 'GGh' ></span> </td></tr>
<tr><td > : <span id = 'TTa' ></span> </td></tr>
</table>

Local:<div id='horalocal' class = 'label label-info'>hora</div>
Rpi server:<div id='horapi' class = 'label label-info'>hora</div>
Arduino control:<div id='horaard' class = 'label label-info'>hora</div>
Actualizado cada 30s
</div>";
?>
<br>
<div class='container'>
    <p>
        <a href='serial_arduino_write.php?comando=A' class='btn btn-info custom' role='button'>Automático</a>
        <a href='serial_arduino_write.php?comando=M' class='btn btn-info custom' role='button'>Manual</a>
        <a href='serial_arduino_write.php?comando=E' class='btn btn-info custom' role='button'>Giro Este</a>
        <a href='serial_arduino_write.php?comando=O' class='btn btn-info custom' role='button'>Giro Oeste</a>
    </p>
    <p>
        <a href='serial_arduino_setTime.php' class='btn btn-primary custom' role='button'>Sincronizar Hora</a>
        <button type='button' class='btn btn-success custom'></button>
        <a href='serial_arduino_write.php?comando=N' class='btn btn-info custom' role='button'>Giro Norte</a>
        <a href='serial_arduino_write.php?comando=S' class='btn btn-info custom' role='button'>Giro Sur</a>
    </p>
</div>

<div class="container">
<table class='table' data-height="19" >
    <thead>
    <tr>
        <th  class='text-center' data-sortable='true'>Acimut Sol.</th>
        <th class='text-center' data-sortable="true">Cenit Sol</th>
        <th class='text-center' data-sortable="true">Acimut Actual</th>
        <th class='text-center' data-sortable="true">Cenit Actual</th>
    </tr>
    </thead>
    <tbody>
    <tr align='center'>
         <td><span id='acimutsol'>--</span> º</td>
        <td><span id='cenitsol'>--</span> º</td>
        <td><span id='acimutactual'>--</span> º</td>
        <td><span id='cenitactual'>--</span> º</td>
    </tr>
    </tbody>
</table>
Finales de carrera (0=abierto, 1=cerrado Normalmente cerrados)
    <table class='table' data-height="19" >
        <thead>
        <tr>
            <th  class='text-center' data-sortable='true'>FC Este</th>
            <th class='text-center' data-sortable="true">FC_Oeste</th>
            <th class='text-center' data-sortable="true">FC_Norte</th>
            <th class='text-center' data-sortable="true">FC_Sur</th>
        </tr>
        </thead>
        <tbody>
        <tr align='center'>
            <td><span id='FC_este'>--</span> </td>
            <td><span id='FC_oeste'>--</span> </td>
            <td><span id='FC_norte'>--</span> </td>
            <td><span id='FC_sur'>--</span> </td>
        </tr>
        </tbody>
    </table>

Estados de giro (0=reposo, 1=girando)
    <table class='table' data-height="10" >
        <thead>
        <tr>
            <th  class='text-center' data-sortable='true'>Giro Este.</th>
            <th class='text-center' data-sortable="true">Giro_Oeste</th>
            <th class='text-center' data-sortable="true">Giro_Norte</th>
            <th class='text-center' data-sortable="true">Giro_Sur</th>
        </tr>
        </thead>
        <tbody>
        <tr align='center'>
            <td><span id='Giro_este'>--</span> </td>
            <td><span id='Giro_oeste'>--</span> </td>
            <td><span id='Giro_norte'>--</span> </td>
            <td><span id='Giro_sur'>--</span> </td>
        </tr>
        </tbody>
    </table>

Movimiento Manual (0=reposo, 1=girando)
    <table class='table' data-height="10" >
        <thead>
        <tr>
            <th  class='text-center' data-sortable='true'>Manual_Este</th>
            <th class='text-center' data-sortable="true">Manual_Oeste</th>
            <th class='text-center' data-sortable="true">Manual_Norte</th>
            <th class='text-center' data-sortable="true">Manual_Sur</th>
        </tr>
        </thead>
        <tbody>
        <tr align='center'>
            <td><span id='Manual_este'>--</span> </td>
            <td><span id='Manual_oeste'>--</span> </td>
            <td><span id='Manual_norte'>--</span> </td>
            <td><span id='Manual_sur'>--</span> </td>
        </tr>
        </tbody>
    </table>

Errores y estado manual(1)/auto(0)
    <table class='table' data-height="10" >
        <thead>
        <tr>
            <th  class='text-center' data-sortable='true'>Error</th>
            <th class='text-center' data-sortable="true">Iniciado</th>
            <th class='text-center' data-sortable="true">Iniciado_este</th>
            <th class='text-center' data-sortable="true">Iniciado_Sur</th>
	    <th class='text-center' data-sortable="true">Manual</th>

        </tr>
        </thead>
        <tbody>
        <tr align='center'>
            <td><span id='error'>--</span> </td>
            <td><span id='iniciado'>--</span> </td>
            <td><span id='iniciado_Este'>--</span> </td>
            <td><span id='iniciado_Norte'>--</span> </td>
	    <td><span id='manual'>--</span> </td>
        </tr>
        </tbody>
    </table>



</div>



<?php

echo "
<script type='text/javascript'>

function tabla(){

            var i=1;
            var j=0;

             $.getJSON('serial_arduino.php', { invtnum : i }, function(rdata)
             {
            json = eval(rdata);


                 document.getElementById('horapi').innerHTML = json.year;
                 document.getElementById('acimutsol').innerHTML = json.acimut_sol;
                 document.getElementById('cenitsol').innerHTML = json.cenit_sol;
                 document.getElementById('acimutactual').innerHTML = json.acimut_actual;
                 document.getElementById('cenitactual').innerHTML = json.cenit_actual;
                 document.getElementById('FC_este').innerHTML = json.FC_este;
                 document.getElementById('FC_oeste').innerHTML = json.FC_oeste;
                 document.getElementById('FC_norte').innerHTML = json.FC_norte;
                 document.getElementById('FC_sur').innerHTML = json.FC_sur;

                 document.getElementById('Giro_este').innerHTML = json.estado_giroE;
                 document.getElementById('Giro_oeste').innerHTML = json.estado_giroO;
                 document.getElementById('Giro_norte').innerHTML = json.estado_giroN;
                 document.getElementById('Giro_sur').innerHTML = json.estado_giroS;

         		 document.getElementById('Manual_este').innerHTML = json.manual_giraEste;
                 document.getElementById('Manual_oeste').innerHTML = json.manual_giraOeste;
                 document.getElementById('Manual_norte').innerHTML = json.manual_giraNorte;
                 document.getElementById('Manual_sur').innerHTML = json.manual_giraSur;

		         document.getElementById('error').innerHTML = json.error;
		         document.getElementById('iniciado').innerHTML = json.iniciado;
                 document.getElementById('iniciado_Este').innerHTML = json.iniciado_Este;
                 document.getElementById('iniciado_Norte').innerHTML = json.iniciado_Norte;
                 document.getElementById('manual').innerHTML = json.manual;


    var date = [ json.dia,json.mes,  json.ano ];
    var time = [ json.hora, json.min, json.seg];
    for ( var k = 1; k < 3; k++ ) {
        if ( time[k] < 10 ) {
            time[k] = \"0\" + time[k];
        }
    }
   var varfecha=date.join(\"/\") + \" \" + time.join(\":\");
    document.getElementById('horaard').innerHTML= varfecha;


        });

}
</script>";
?>


<script type='text/javascript'>
    $(document).ready(function() {
        tabla();
        timeStamp();
        setInterval(tabla, 30000);
        setInterval(timeStamp, 30000);
    });

function timeStamp() {
    var now = new Date();
    var date = [ now.getDate(),now.getMonth() + 1,  now.getFullYear() ];
    var time = [ now.getHours(), now.getMinutes(), now.getSeconds() ];
    for ( var i = 1; i < 3; i++ ) {
        if ( time[i] < 10 ) {
            time[i] = "0" + time[i];
        }
    }
   var varfecha=date.join("/") + " " + time.join(":");
    document.getElementById('horalocal').innerHTML= varfecha;
    return date.join("/") + " " + time.join(":");
}
</script>








