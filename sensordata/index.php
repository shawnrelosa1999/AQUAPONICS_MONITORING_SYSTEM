<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="5" >
    <link rel="stylesheet" type="text/css" href="style.css" media="screen"/>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <title>AQUAPONICS MONITORING SYSTEM</title>
    <style>
      * {
        margin: 0;
        padding: 0;
        font-family: sans-serif;
    }
        
        .container {
      max-width: 600px;
      margin: 0 auto;
      padding: 20px;
      text-align: center;
    }
    
    .sensor-data {
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      margin-top: 20px;
    }
    
    .sensor-data h2 {
      margin-top: 20px;
    }
    
    .sensor-data p {
      font-size: 24px;
      font-weight: bold;
      margin: 10px 0;
    }
    .chartContainer {
      display:flex;
      flex-direction: row;
      justify-content: center;
      height: 100%;
    }
    
    .container1, .container2, .container3 {
      width: 400px;
      height: 200px;
      margin: 10px;
      border: 5px solid #07add6;
      border-radius: 10px;
    }
    
    footer {
      position: fixed;
      bottom: 0;
      left: 0;
      width: 100%;
      background-color: #00a8f7;
      color: #fff;
      text-align: center;
      padding: 10px 0;
    }


html, body { height: 100%; }
body {
  background:radial-gradient(ellipse at center, rgba(255,254,234,1) 0%, rgba(255,254,234,1) 35%, #B7E8EB 100%);
  overflow: hidden;
}

.ocean { 
  height: 5%;
  width:100%;
  position:absolute;
  bottom:0;
  left:0;
  background: #015871;
}

.wave {
  background: url(https://s3-us-west-2.amazonaws.com/s.cdpn.io/85486/wave.svg) repeat-x; 
  position: absolute;
  top: -198px;
  width: 6400px;
  height: 198px;
  animation: wave 7s cubic-bezier( 0.36, 0.45, 0.63, 0.53) infinite;
  transform: translate3d(0, 0, 0);
}

.wave:nth-of-type(2) {
  top: -175px;
  animation: wave 7s cubic-bezier( 0.36, 0.45, 0.63, 0.53) -.125s infinite, swell 7s ease -1.25s infinite;
  opacity: 1;
}

@keyframes wave {
  0% {
    margin-left: 0;
  }
  100% {
    margin-left: -1600px;
  }
}

@keyframes swell {
  0%, 100% {
    transform: translate3d(0,-25px,0);
  }
  50% {
    transform: translate3d(0,5px,0);
  }
}
    
   
 /* Style the links inside the navigation bar */
 .topnav a {
  float: left;
  color: #000000;
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
  font-size: 17px;
  font-weight: bold;
  font-style: sans-serif;
}

/* Change the color of links on hover */
.topnav a:hover {
  background-color: #213b92;
  color: rgb(231, 232, 237);
}

/* Add a color to the active/current link */
.topnav a.active {
  background-color: #00ffa2;
  color: white;
}
nav {
  background-color: #0bc57e;
  overflow: hidden;
}

nav ul {
  margin: 0;
  padding: 0;
  list-style: none;
}

nav li {
  float: left;
}

nav a {
  display: block;
  color: rgb(255, 255, 255);
  text-align: center;
  padding: 14px 16px;
  text-decoration: none;
  font-weight: bold;
}

nav a:hover {
  background-color: #ddd;
  color: black;
}
nav {
  background-color: #0bc57e;
  overflow: hidden;
}

nav ul {
  margin: 0;
  padding: 0;
  list-style: none;
}

nav li {
  float: left;
}


nav a:hover {
  background-color: #ddd;
  color: black;
}

:root {
  --scrollcolor: #fff;
  --scrollbackground: #141e27;
}

* {
  box-sizing: border-box;
}


  </style>

</head>

<body>
<div class="topnav">
    <nav>
      <ul>
        <li><a href="index.html">Dashboard</a></li>
        <li><a href="symtomsandtreatment.html">Symptoms and Treatments</a></li>
        <li><a href="index.php">Sensor Table Readings</a></li>
      </ul>
    </nav>
  </div>
    <h1>Sensor Data Readings</h1>
    
<?php
$servername = "localhost";
$username = "root";
$password = "";
$dbname = "esp32data";

// Create connection
$conn = new mysqli($servername, $username, $password, $dbname);
// Check connection
if ($conn->connect_error) {
    die("Connection failed: " . $conn->connect_error);
}

$sql = "SELECT id, sensor, value1, value2, value3, reading_time FROM sensordata ORDER BY id DESC"; /*select items to display from the sensordata table in the data base*/

echo '<table cellspacing="5" cellpadding="5">
      <tr> 
        <th>ID</th> 
        <th>Date $ Time</th> 
        <th>Sensor</thh> 
        <th>Temperature &deg;C</th> 
        <th>Ph Level</th> 
        <th>Total Dissolved Solids(PPM)</th>       
      </tr>';
 
if ($result = $conn->query($sql)) {
    while ($row = $result->fetch_assoc()) {
        $row_id = $row["id"];
        $row_reading_time = $row["reading_time"];
        $row_sensor = $row["sensor"];
        $row_value1 = $row["value1"];
        $row_value2 = $row["value2"]; 
        $row_value3 = $row["value3"]; 
        
        // Uncomment to set timezone to - 1 hour (you can change 1 to any number)
       // $row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time - 1 hours"));
      
        // Uncomment to set timezone to + 4 hours (you can change 4 to any number)
        //$row_reading_time = date("Y-m-d H:i:s", strtotime("$row_reading_time + 4 hours"));
      
        echo '<tr> 
                <td>' . $row_id . '</td> 
                <td>' . $row_reading_time . '</td> 
                <td>' . $row_sensor . '</td> 
                <td>' . $row_value1 . '</td> 
                <td>' . $row_value2 . '</td>
                <td>' . $row_value3 . '</td> 
                
              </tr>';
    }
    $result->free();
}

$conn->close();
?> 
</table>

<div class="topnav">
    <nav>
      <ul>
        <li><a href="index.html">Dashboard</a></li>
        <li><a href="symtomsandtreatment.html">Symptoms and Treatments</a></li>
        <li><a href="index.php">Sensor Table Readings</a></li>
      </ul>
</body>
</html>

	</body>
</html>