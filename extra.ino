/*
/////////////////////////////////////////////// LANDING PAGE ///////////////////////////////////////////////

const char landing_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>ESP32 LANDING PAGE</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        
        h1 {
            color: #0F3376;
            padding: 2vh;
        }
        
        button {
            display: inline-block;
            background-color: green;
            border: none;
            border-radius: 4px;
            color: white;
            padding: 16px 40px;
            text-decoration: none;
            font-size: 12px;
            margin: 2px;
            cursor: pointer;
        }
        
        button:hover {
            background-color: darkgreen;
        }
        
        button:active {
            background-color: rgb(0, 71, 0);
        }
               
        .button-container {
            position: relative;
        }
    </style>
    <script type="text/javascript">
      var allDaButtons = function(buttonName) {
//        document.getElementById('send').value=buttonName;
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", buttonName, true);
        xhttp.send();
      }
    </script>
</head>

<body>
    <h1>LANDING PAGE</h1>
    <div class="button-container">
        <h3>Choose the page you want to visit</h3>
        <form action="/lightswitch">
        <button class="button">LIGHTSWITCH CONTROL CENTER</button>
        </form>
        <form action="/whistle">
        <button class="button">WHISTLE CONTROL CENTER</button>
        </form>
        <form action="/stereo">
        <button class="button">STEREO CONTROL CENTER</button>
        </form>
    </div>
</body>

</html>)rawliteral";


/////////////////////////////////////////////// WHISTLE PAGE ///////////////////////////////////////////////
const char whistle_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>ESP32 WHISTLE CENTER</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        
        h1 {
            color: #0F3376;
            padding: 2vh;
        }
        
        button {
            display: inline-block;
            background-color: #008CBA;
            border: none;
            border-radius: 4px;
            color: white;
            padding: 16px 40px;
            text-decoration: none;
            font-size: 24px;
            margin: 2px;
            cursor: pointer;
        }

        button:hover {
            background-color: #006688;
        }
        
        button:active {
            background-color: #004961;
        }
        
        .whistle-container {
            position: relative;
        }
    </style>
    <script type="text/javascript">
      var allDaButtons = function(buttonName) {
        //document.getElementById('send').value=buttonName;
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", buttonName, true);
        xhttp.send();
      }
    </script>
</head>

<body>
    <h1>WHISTLE CONTROL CENTER</h1> <br>
    <div class="whistle-container">
        <button class="button" onClick="allDaButtons('/skyler');">WHISTLE FOR SKYLER</button> <br>
        <button class="button" onClick="allDaButtons('/amira');">WHISTLE FOR AMIRA</button> <br>
        <button class="button" onClick="allDaButtons('/griffyn');">WHISTLE FOR GRIFFYN</button> <br>
        <button class="button" onClick="allDaButtons('/razz');">WHISTLE FOR RAZZ</button> <br>
    </div>
</body>

</html>)rawliteral";


/////////////////////////////////////////////// STEREO_HTML ///////////////////////////////////////////////
const char stereo_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>ESP32 STEREO CENTER</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        
        h1 {
            color: #0F3376;
            padding: 2vh;
        }
        
        button {
            display: inline-block;
            background-color: #008CBA;
            border: none;
            border-radius: 4px;
            color: white;
            padding: 16px 40px;
            text-decoration: none;
            font-size: 18px;
            margin: 2px;
            cursor: pointer;
        }
        
        button:active {
            background-color: #0F3376;
        }

        .button-container {
            position: relative;
        }
        
    </style>
    <script type="text/javascript">
    var allDaButtons = function(buttonName) {
      //document.getElementById('send').value=buttonName;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", buttonName, true);
      xhttp.send();
    }
  </script>
</head>

<body>
    <h1>STEREO CONTROL CENTER</h1> <br>
    <div class="button-container">
        <button class="button" onClick="allDaButtons('/stereo_power');">ON / OFF</button>
        <button class="button" onClick="allDaButtons('/stereo_mute');">MUTE</button>
        <button class="button" onClick="allDaButtons('/stereo_bass');">BASS BOOST</button>
    </div> <br>
    <div>
        <button class="button" onClick="allDaButtons('/stereo_cd');">CD</button>
        <button class="button" onClick="allDaButtons('/stereo_bt');">BLUETOOTH</button>
        <button class="button" onClick="allDaButtons('/stereo_fm');">FM RADIO</button>
        <button class="button" onClick="allDaButtons('/stereo_aux');">AUX</button>
    </div> <br>
    <div>
        <button class="button" onClick="allDaButtons('/stereo_rr');">REWIND</button>
        <button class="button" onClick="allDaButtons('/stereo_ff');">FORWARD</button> <br>
        <button class="button" onClick="allDaButtons('/stereo_play');">PLAY / PAUSE</button>
        <button class="button" onClick="allDaButtons('/stereo_stop');">STOP</button> <br>
    </div> <br>
    <div> 
        <button class="button" onClick="allDaButtons('/stereo_rpt');">REPEAT</button> 
        <button class="button" onClick="allDaButtons('/stereo_prog');">PROGRAM</button>
        <button class="button" onClick="allDaButtons('/stereo_voldown');">VOLUME DOWN</button>
        <button class="button" onClick="allDaButtons('/stereo_volup');">VOLUME UP</button>
    </div>
</body>

</html>)rawliteral";


/////////////////////////////////////////////// INDEX_HTML ///////////////////////////////////////////////
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>

<head>
    <title>ESP32 Web Server</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style type="text/css">
        html {
            font-family: Helvetica;
            display: inline-block;
            margin: 0px auto;
            text-align: center;
        }
        
        h1 {
            color: #0F3376;
            padding: 2vh;
        }
        
        button {
            display: inline-block;
            background-color: #008CBA;
            border: none;
            border-radius: 4px;
            color: white;
            padding: 16px 40px;
            text-decoration: none;
            font-size: 18px;
            margin: 2px;
            cursor: pointer;
        }
        
        button:active {
            background-color: #0F3376;
        }

        .button-container {
            position: relative;
        }
        
    </style>
    <script type="text/javascript">
    var allDaButtons = function(buttonName) {
      //document.getElementById('send').value=buttonName;
      var xhttp = new XMLHttpRequest();
      xhttp.open("GET", buttonName, true);
      xhttp.send();
    }
  </script>
</head>

<body>
    <h1>LIGHTSWITCH CONTROL CENTER</h1> <br>
    <div class="button-container">
        <button class="button" onClick="allDaButtons('/on');">LIGHTSWITCH ON</button>
        <button class="button" onClick="allDaButtons('/off');">LIGHTSWITCH OFF</button> <br>
        <button class="button" onClick="allDaButtons('/lock');">LOCK SENSORS</button>
        <button class="button" onClick="allDaButtons('/unlock');">UNLOCK SENSORS</button> <br>
        <button class="button" onClick="allDaButtons('/rlock');">LOCK REMOTE</button>
        <button class="button" onClick="allDaButtons('/runlock');">UNLOCK REMOTE</button> <br>
        <button class="button" onClick="allDaButtons('/hlock');">LOCK HIDDEN</button>
        <button class="button" onClick="allDaButtons('/hunlock');">UNLOCK HIDDEN</button> <br>
        <button class="button" onClick="allDaButtons('/alock');">LOCK ALL</button>
        <button class="button" onClick="allDaButtons('/aunlock');">UNLOCK ALL</button>  <br>
    </div>
</body>

</html>)rawliteral";
 */
