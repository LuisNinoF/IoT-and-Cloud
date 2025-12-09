<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <!-- Include Bootstrap CSS -->
    <link href="https://stackpath.bootstrapcdn.com/bootstrap/5.0.0-alpha1/css/bootstrap.min.css" rel="stylesheet">
    <!-- Add favicon -->
    <link rel="icon" href="favicon.ico" type="image/x-icon" />

    <style>
        body
        {
            background: #000000;
            color: #fff;
        }

       .main-header{
        text-align :  center;
        padding : 10px 0;
       } 

       .logo{
         height: 80px;
       }

       .container{
        // background-color: #ADD8E6;
        background-color: #000000;
        border-radius: 10px;
        padding: 1px !important;
        margin-top:1px !important;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1)
       }

       .btn-primary {
        background-color: #00BFFF;
        border-color: #00BFFF;
        transition: all 1s ease;
        }

        .btn-primary:focus {
        background-color: #99E6FF ;
        border-color: #99E6FF ;
        }

       .btn:hover{
        background-color: #99E6FF;
        border-color: #99E6FF ;
       }
     
       .form-control{
        border-radius: 0.25rem;
        background-color: #fff;
        color: #000;
       }
             /* Make native date inputs match button radius */
             input[type="date"].form-control{
                border-radius: 0.25rem !important;
                overflow: hidden;
                -webkit-appearance: none;
                appearance: none;
             }
             /* Ensure the calendar indicator doesn't break the rounded corners (WebKit) */
             input[type="date"].form-control::-webkit-calendar-picker-indicator{
                padding: 0 8px;
                background: transparent;
             }

       .chart-container{
        height:  400px;
        width: 100%;
        margin: 20px auto;
        padding: 20px;
        background-color: #fff;
        border-radius: 10px;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1)
       }

       .table{
        background-color: #fff;
        box-shadow: 0 2px 10px rgba(0,0,0,0.1)
        color :#000;
        width: 100%;
        margin: 0 auto 20 px;       /*margin to the bottom only */
        overflow: hidden;       
        border-radius: 10px;        /* radius for whole table */
       }

       .table th,
       .table td{
         vertical-align: middle;
         text-align: center;
       }

       .table thead{
        background-color: #00BFFF;
        color:#fff
       }

       .table tbody tr:nth-child(even){
        background-color:#99E6FF;
       }

    </style>
    <title>Sensor Readings</title>
</head>

<body>
    <div class="main-header">
    <img src="logo.png" alt="Company Logo" class="logo">
    </div>

    <!--Container with dates Row, Chart and Table-->
    <div class= "container mt-4 p-4">
        <!--Add Row with field and button to either download or display historical data*-->
        <div class="row">
            <!--form to the left*-->
            <div class="col-md-6">
                <form action="/records.php" class="mb-3">
                    <div class="form-group">
                        <label for="selectDay1">Download Historical Data</label>
                        <div class="d-flex">
                            <input type="date" id="selectDay1" name= "selectDay1" class="form-control mr-2">
                            <input type="submit" value="Download" class="btn btn-primary">
                        </div>
                    </div>
                 </form>
            </div>

            <!--form to the right*-->
            <div class="col-md-6">
             <form action="/index.php">
                <div class="form-group">
                    <label for="selectDay2">Display Historical Data</label>
                        <div class="d-flex">
                            <input type="date" id="selectDay2" name= "selectDay2" class="form-control mr-2">
                            <input type="submit" value="Display" class="btn btn-primary">
                        </div>
                    </div>
             </form>
            </div>
        </div>

        <!--Add chart container to display data*-->
        <div class = "chart-container">
             <canvas id="myChart"></canvas>
        </div>

        <!--Add table below the chart*-->
        <table class="table table-stripped">
            <thead>
                <tr>
                    <th>Date</th>
                    <th>Sensor Value 1</th>
                    <th>Sensor Value 2</th>
                </tr>
            </thead>
            <!--Add table body with sensor data-->
            <tbody>
                <?php
                    $selectDay = 0;

                    //Check if 'selectDay2' is set and not empty in the GET parameters
                    if(isset($_GET['selectDay2']) && $_GET['selectDay2'] != ""){

                        //Convert 'selectDay2' from GET parameters to a timestamp
                        $selectDay =  strtotime($_GET['selectDay2']);

                    }
                    else{
                        //If 'selectDay2' is not provided, set default time to 12 hours ago
                        $selectDay =  time()  - 43200; //43200seconds = 12hours
                    }

                     //Calculate the timestamp for the next day (24hours later) 
                     $nextDay =  $selectDay + 86400; //86400 seconds = 24hours
                    
                     //Create a new PDO object to connect to SQLite database 'SensorDBmulti.sqlite'
                     $db  = new PDO('sqlite:SensorDBmulti.sqlite');
                    //Create SQL query to select readings between the chosen date and the next day
                     $query = "SELECT * FROM readings WHERE date BETWEEN " . $selectDay .  " AND " . $nextDay . " ORDER BY date DESC LIMIT 30";
                    
                     //Execute the query
                     $result = $db->query($query);

                     //Loop through each row of the result
                     foreach ($result as $row){
                        echo "<tr><td>". date('d/m/Y H:i:s', $row['date'])."</td>";
                        echo "<td>". $row['sensorvalue1'] . "</td>";
                        echo "<td>". $row['sensorvalue2'] . "</td></tr>";
                     }
               ?>
            </tbody>
        </table>
    </div>

    <!--Populate or fill in the chart with the sensor data-->
    <?php 
        //Create query to select readings between two specific dates
        $query =  "SELECT * FROM readings WHERE date BETWEEN ". $selectDay . " AND " . $nextDay . " ORDER BY date ASC";
        
        //Execute query
        $result = $db->query($query);

        //Create a two-dimensional array called rows
        $rows[][] =  array();

        $rows[0][0] = "Date";
        $rows[0][1] = "Sensor Value 1";
        $rows[0][1] = "Sensor Value 2";

        $count =  1;

        //Iterate over each record in the $result set
        foreach($result as $row){
                //For each record store the date(formatted as hour:minute) in the first column
                $rows[$count][0] =  date('H:i', $row['date']);

                //Store the sensor value  in the second column and thrid column
                $rows[$count][1] =  intval( $row["sensorvalue1"]);
                $rows[$count][2] =  intval( $row["sensorvalue2"]);

                $count++;
        }
        $jsonTable =  json_encode($rows)
    ?>

    <!--Import Chart from CDN-->
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
    <script>
            setInterval(function () {
            window.location.reload();
            }, 10000); // 10000 ms = 10 seconds

        var ctx = document.getElementById('myChart').getContext('2d');
        
        //Create a new line chart
        var myChart =  new Chart(ctx,{
            //Set type as line
            type: 'line',
            
            //Data for the chart
            data: {

                //Get labels(dates) for the chart from the first column of the $rows array
                labels: <?php echo json_encode(array_column($rows,0)); ?>,

                //Define the dataset for the chart
                datasets: [
                    {
                    label: 'Sensor Value 1',

                    //Get sensor values for the chart from the second column of $rows array
                    data: <?php echo json_encode(array_column($rows,1)); ?>,
                    backgroundColor: '#00BFFF',
                    borderColor: '#00BFFF',
                    borderWidth: 1
                    },
                    {
                    label: 'Sensor Value 2',
                    //Get sensor values for the chart from the third column of $rows array
                    data: <?php echo json_encode(array_column($rows,2)); ?>,
                    backgroundColor: '#FF5733',
                    borderColor: '#FF5733',
                    borderWidth: 1
                    }]
            },

            //Options for customising appearance
            options:{

                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        display: true
                    }
                },
                scales:{
                    x: {
                        title: {
                            display: true,
                            text: 'Time',
                            font: {
                                weight: 'bold'
                            }
                        },
                        ticks: {
                            autoSkip: true,     
                            maxTicksLimit: 12  
                       }
                    },
                    y: {
                        beginAtZero: true,
                        title: {
                            display: true,
                            text: 'Sensor Value',
                            font: {
                                weight: 'bold'
                            }
                        }

                    }
                }
            }
        });
    </script>

</body>
</html>
