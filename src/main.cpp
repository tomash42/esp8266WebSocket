#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
//wifi ACCESS POINT - konfiguracja dostepu do okreslonej sieci  
const char* ssid = "UPC1064739";
const char* password = "FDVECCFJ";
//end AP

//WIFI softAP -  konfiguracja mozna dowolnie zmienic. 
IPAddress local_IP(192,168,10,10);
IPAddress gateway(192,168,10,1);
IPAddress subnet(255,255,255,0);

const char* ssid2 = "haslo od 1 do 8";
const char* password2 = "12345678";
//end WiFi soft AP
 


int webSockMillis = 200;

//serwer www
ESP8266WebServer server(80);
//end server

//serwer websocket
WebSocketsServer webSocket = WebSocketsServer(81);
WebSocketsServer webSocket2 = WebSocketsServer(82);//nie skonfigurowany
//end serwer wb


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
//funkcja wyswietlajaca strone www.
void WebSite(){
  server.send(webSockMillis, "text/html", webPage);
}

int dist = 0 ;
//przypisanie pinow czujnika d6=12 d5=14
const int trigP = 12;
const int echoP = 14;

//czas trwania pomiaru
long duration;
//zmierzona odleglosc od obiektu
int distance;
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 100;



void setup() {
  //predkosc transmisji 9600
  Serial.begin(9600);

  // Begin Access Point

//sAP uruchomienie wifi
Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
Serial.print("Setting soft-AP ... ");
Serial.println(WiFi.softAP(ssid2,password2) ? "Ready" : "Failed!");
Serial.print("Soft-AP IP address = ");
Serial.println(WiFi.softAPIP());
// end sAP
//STA polaczenie do okreslonej sieci 
Serial.println(ssid);
WiFi.begin(ssid, password);
while (WiFi.status() != WL_CONNECTED) 
{
    delay(500);
    Serial.print(".");
}

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address is : ");
  Serial.println(WiFi.localIP());



  //funkcja z www. Uruchomienie serwerow. 
  server.on("/",WebSite);
  server.begin();
  webSocket.begin();
  webSocket2.begin();

// wejście wyjscie sygnału wyzwalającego cykl pomiarowy trig i echo
//Ustawienie pinu trigP jako wyjście
//Ustawienie pinu echoP jako wejscie
  pinMode(trigP, OUTPUT);
  pinMode(echoP, INPUT);

//początkowy czas rozpoczęcia
startMillis = millis();
}

void loop() {
  webSocket.loop();
  server.handleClient();


currentMillis = millis();
if (currentMillis - startMillis >= period)
{
  //Wyzerowanie sygnalu trig
  digitalWrite(trigP,LOW);
  //odczekanie 2 mikrosekundy
  delayMicroseconds(2);
//start pomiaru - stan wysoki
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

