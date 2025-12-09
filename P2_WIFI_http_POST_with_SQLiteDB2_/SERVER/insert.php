<?php
    $key ="";
    $sensorvalue1 = "0";
    $sensorvalue2 = "0";
    //Check if 'key' is provided in the POST request
    //if so assign value to $key
    if(isset($_POST['key'])) $key = $_POST['key'];

    //Check if 'sensorvalue1' is provided in the POST request
    //if so assign value to $sensorvalue1
    if(isset($_POST['sensorvalue1'])) $sensorvalue1 = $_POST['sensorvalue1'];

    //Check if 'sensorvalue2' is provided in the POST request
    //if so assign value to $sensorvalue2
    if(isset($_POST['sensorvalue2'])) $sensorvalue2 = $_POST['sensorvalue2'];

    //Check if key provided is equal to '4326', as defined in the device HTTP POST request
    if($key == '4326'){

        //Get current unix timestamp
        $date =  time();

        //convert sensorvalue to integer
        $intsensorvalue1 = intval($sensorvalue1);
        $intsensorvalue2 = intval($sensorvalue2);

        //Create a new PDO instance that connects to an SQLite 
        //database called SensorDBmulti.sqlite
        $db =  new PDO('sqlite:SensorDBmulti.sqlite');

        //Create a table called 'readings' if it doesn't exist already
        //The table should have two columns; date and sensorvalue
        $db->exec("CREATE TABLE IF NOT EXISTS readings(date INTEGER, sensorvalue1 INTEGER, sensorvalue2 INTEGER);");

        //Insert a new row into the 'readings' table 
        //The current timestamp and integer sensor value are inserted
        $db->exec("INSERT INTO readings(date, sensorvalue1, sensorvalue2) VALUES('$date','$intsensorvalue1', '$intsensorvalue2');");
        
        //Ok to indicate script executed successfully.
        echo "ok";
    }

?>
