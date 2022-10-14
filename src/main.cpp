#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
//wifi ACCESS POINT 
const char* ssid = "UPC1064739";
const char* password = "FDVECCFJ";

//WIFI softAP
const char* ssid2 = "test";
const char* password2 = "12345678";

int webSockMillis = 200;

ESP8266WebServer server(80);

WebSocketsServer webSocket = WebSocketsServer(81);
WebSocketsServer webSocket2 = WebSocketsServer(82);



String webSite, JSONtxt;

//strona www 
const char webPage[] PROGMEM = R"====(
<script>
initWebsocket();
function initWebsocket()
{

websocket = new WebSocket('ws://'+window.location.hostname+':81/');
websocket.onmessage = function(e)
{
  JSONobj = JSON.parse(e.data);
  var dist = parseInt(JSONobj.Distance);
  document.getElementById("dynRect").style.height = 600- dist +"px";
  console.log("dystans",dist)
  console.log("600 -dystans",600-dist)
}
}
</script>
<htm>
<head>
<style>
body{
  background-color:       rgb(28, 44, 77);
  color:                  rgb(255, 123, 8);
}
#dynRect{
  background-color:        red;
  margin-top:              8px;
  height:                  600px;
  top:                     9px;
  width:                   200px;
  z-index=                -1;
  
}

#rectangle{
background-image:
                    linear-gradient(0deg,
                    rgba(255,255,255,0.5) 0%,
                    rgba(255,255,255,0.5) 5%,
                    transparent 1%
                    );

background-size:    60px 11px;
font-size:          0;
height:         600px;
padding:            0;
transform:          rotate(180deg);
width:              200px;  
white-space:        nowrap;
}
.container{
  display:                flex;
    justify-content:      center;
  padding:                100px 100px 100px 100px; 
}


label{
  
  display:        block;
  font-size:      50px;
  height:         60px;
  margin-right:   100px;
  padding-top:    2px;
  text-indent:    3px;
  text-align:     center;
  width:          40px;
  
  
  
  
}
</style>
</head>
<body>
<div class="container">

    <div class="label1">
        <label>Full</label>
        <label></label>
        <label></label>
        <label></label>
        <label></label>
        <label>50%</label>
        <label></label>
        <label></label>
        <label></label>
        <label></label>
        <label>0</label>
    </div>
    <div class="rect" id="rectangle">

      <div id="dynRect">dddd</div>
    </div>
  
</div>
</body>
</html>

)====";

void WebSite(){
  server.send(webSockMillis, "text/html", webPage);
}

int dist = 0 ;

const int trigP = 12;//d6 pin na hs
const int echoP = 14;//pin

long duration;
int distance;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 100;




void setup() {
  Serial.begin(9600);

  // Begin Access Point
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid2 ,password2);


  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address is : ");
  Serial.println(WiFi.localIP());
  Serial.println("IP soft AP  is:  ");
  Serial.print(WiFi.softAPIP());


  //funkcja z www
  server.on("/",WebSite);
  server.begin();
  webSocket.begin();
  webSocket2.begin();


  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT);
startMillis = millis();//initial start time

}

void loop() {
  webSocket.loop();
  server.handleClient();


currentMillis = millis();
if (currentMillis - startMillis >= period)
{
  digitalWrite(trigP,LOW);
  delayMicroseconds(2);

  digitalWrite(trigP,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigP,LOW);

  duration = pulseIn(echoP, HIGH);
  distance = duration*0.034/2;

  Serial.println(distance);
  startMillis = currentMillis;

}

delay(100);
/* 
  dist ++;
  if(dist>=600)
    dist=0; */
    if (dist> (distance *600/200))
    {
       dist = dist-1 ;
       Serial.println(dist);
    } else if (dist < (distance * 600 /200))
        {
            dist = dist +1;
        }else{
                  distance = (distance * 600 /200);
        }
    
    if(distance >600)
        distance =600;
    
    
    String distanceStr = String(dist);
    JSONtxt = "{\"Distance\":\""+distanceStr+"\"}";
    webSocket.broadcastTXT(JSONtxt);
}

