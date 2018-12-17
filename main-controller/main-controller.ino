/**
 TTGO-T8-ESP32
 
 GND  [GROUND]
 3v3  [3.3V]
  21  [SDA][GPIO21][V_SPI_HD][LED]
 TXD  [CLX3][GPIO01][V_SPI_TXD]
 RXD  [CLX2][GPIO03][V_SPI_RXD]
  22  [SCL][GPIO22][V_SPI_WP]           [ESP32 IO22 – I2S codec DATA]
  19  [GPIO19][V_SPI_Q][V0_CTS]
  23  [GPIO23][V_SPI_D]
  18  [GPIO18][V_SPI_CLK]
   5  [GPIO05][V_SPI_SCO]
   2  [GPIO02][ADC2_2][HSPI_WP][TOUCH2]
 GND  [GROUND]
   0  [CLX1][GPIO00][ADC2_1][TOUCH1]
   4  [GPIO04][ADC2_0][HSPI_HD][TOUCH0]
  5v  [5V]

  VP  [36][S_VP][GPIO36][ADC1_0*]
  VN  [39][S_VN][GPIO39][ADC1_3*]
 RST  [Reset]
  34  [GPIO34][ADC1_6*]
  35  [GPIO35][ADC1_7*]
  32  [XTAL32][GPIO32][ADC1_4*]          ButtonPin4
  33  [XTAL33][GPIO33][ADC1_5*]          ButtonPin3            [ESP32 IO33 – CS MICROSD]
  25  [GPIO25][ADC2_8*][DAC2]            ButtonPin2            [ESP32 IO25 – I2S codec LRCK]
  26  [GPIO26][ADC2_9][DAC1]             ButtonPin1            [ESP32 IO26 – I2S codec BCK]
  27  [GPIO27][ADC2_7*][TOUCH7]          HomeNetwork for FOTA  [ESP32 IO27 – SCK MICROSD]
  14  [GPIO14][ADC2_6*][TOUCH6]          Relay4                [ESP32 IO14 – MOSI MICROSD]
  12  [GPIO12][ADC2_5*][TOUCH5]          Relay3
  13  [GPIO13][ADC2_4][TOUCH4]           Relay2                [ESP32 IO12 – MISO MICROSD]
  15  [GPIO15][ADC2_3][HSPI_CSO][TOUCH7] Relay1  
 GND  [GROUND]


[ESP32 IO33 – CS MICROSD]
[ESP32 IO14 – MOSI MICROSD]
[ESP32 IO12 – MISO MICROSD]
[ESP32 IO27 – SCK MICROSD]
[ESP32 IO26 – I2S codec BCK]
[ESP32 IO22 – I2S codec DATA]
[ESP32 IO25 – I2S codec LRCK]
[ESP32 GND – I2S codec GND]
[ESP32 GND – GND MICROSD]

*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <esp32fota.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>

#include "credentials.h"
#include "relay.h"

// Add to 'credentials.h'
// WiFi credentials for home network
// const char* ssid = ".....";
// const char* password = ".....";

// MQTT server on PI
// const char* mqtt_server = ".....";


const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;


// Set web server port number to 80
AsyncWebServer server(80);

const char * html = "<!DOCTYPE html><html>"
"<head>"
"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"

"<link rel=\"icon\" href=\"data:,\">"

"<style>"

"html {"
" font-family: Helvetica;"
" display: inline-block;"
" margin: 0px auto;"
" text-align: center;"
"}"

"body {"
" background-color: #000000;"
"}"

"h1 {"
" color: #AAAAAA;"
"}"

"h2 {"
" color: #CCCCCC;"
" font-size:16px;"
"}"

".button-on {"
" background:-webkit-gradient(linear, left top, left bottom, color-stop(0.05, #44c767), color-stop(1, #5cbf2a));"
" background:-moz-linear-gradient(top, #44c767 5%, #5cbf2a 100%);"
" background:-webkit-linear-gradient(top, #44c767 5%, #5cbf2a 100%);"
" background:-o-linear-gradient(top, #44c767 5%, #5cbf2a 100%);"
" background:-ms-linear-gradient(top, #44c767 5%, #5cbf2a 100%);"
" background:linear-gradient(to bottom, #44c767 5%, #5cbf2a 100%);"
" filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#44c767', endColorstr='#5cbf2a',GradientType=0);"
" background-color:#44c767;"
" -moz-border-radius:28px;"
" -webkit-border-radius:28px;"
" border-radius:28px;"
" border:1px solid #18ab29;"
" display:inline-block;"
" cursor:pointer;"
" color:#ffffff;"
" font-family:Arial;"
" font-size:16px;"
" padding:18px 50px;"
" text-decoration:none;"
" text-shadow:0px 1px 0px #2f6627;"
"}"

".button-off {"
" background:-webkit-gradient(linear, left top, left bottom, color-stop(0.05, #fc8d83), color-stop(1, #fc8d83));"
" background:-moz-linear-gradient(top, #fc8d83 5%, #fc8d83 100%);"
" background:-webkit-linear-gradient(top, #fc8d83 5%, #fc8d83 100%);"
" background:-o-linear-gradient(top, #fc8d83 5%, #fc8d83 100%);"
" background:-ms-linear-gradient(top, #fc8d83 5%, #fc8d83 100%);"
" background:linear-gradient(to bottom, #fc8d83 5%, #fc8d83 100%);"
" filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#fc8d83', endColorstr='#fc8d83',GradientType=0);"
" background-color:#fc8d83;"
" -moz-border-radius:28px;"
" -webkit-border-radius:28px;"
" border-radius:28px;"
" border:1px solid #d83526;"
" display:inline-block;"
" cursor:pointer;"
" color:#ffffff;"
" font-family:Arial;"
" font-size:16px;"
" padding:18px 50px;"
" text-decoration:none;"
" text-shadow:0px 1px 0px #b23e35;"
"}"

"</style>"

"<script type=\"text/javascript\">"
"/* iOS re-orientation fix */"
"if (navigator.userAgent.match(/iPhone/i) || navigator.userAgent.match(/iPad/i)) {"
"    /* iOS hides Safari address bar */"
"    window.addEventListener(\"load\",function() {"
"        setTimeout(function() {"
"            window.scrollTo(0, 1);"
"        }, 1000);"
"    });"
"}"
"</script>"

"</head>"
"<body><h1>Camper Control</h1>"

"%BUTTON_1%"
"%BUTTON_2%"
"%BUTTON_3%"
"%BUTTON_4%"

"</body></html>";

//"html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}"
//".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;"
//"text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;"
//".buttonoff {background-color: #555555;}"

int FirmwareUpdatePin = 27;
bool FirmwareUpdateState = LOW;
bool FirmwareUpdateLastState = LOW;
bool FirmwareUpdateNow = false;
unsigned long FirmwareUpdateLastDebounceTime = 0;

// esp32fota esp32fota("<Type of Firme for this device>", <this version>);
// esp32FOTA esp32FOTA("esp32-fota-http", 1);

void setup()
{
  pinMode(ButtonPin1, INPUT_PULLUP);
  pinMode(ButtonPin2, INPUT_PULLUP);
  pinMode(ButtonPin3, INPUT_PULLUP);
  pinMode(ButtonPin4, INPUT_PULLUP);
  pinMode(FirmwareUpdatePin, INPUT_PULLUP);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);

  digitalWrite(Relay1, !RelayState1);
  digitalWrite(Relay2, !RelayState2);
  digitalWrite(Relay3, !RelayState3);
  digitalWrite(Relay4, !RelayState4);

  //esp32FOTA.checkURL = "http://server/fota/fota.json";
  Serial.begin(115200);
  
  // setup_wifi();
  setup_wifi_AP();
  SetupWebServer() ;

}

void setup_wifi_AP() {
  delay(10);
  Serial.print("CSetting up AP ");
  Serial.println(ssid);


  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(APssid, APpassword);
  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

}

void setup_wifi()
{
  delay(10);
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println(WiFi.localIP());
}

void loop()
{
  dnsServer.processNextRequest();
  buttonloop();
  firmwareloop();
}

void firmwareloop()
{
  FirmwareUpdateState = digitalRead(FirmwareUpdatePin);

//  Serial.print("FirmwareUpdateState : ");
//  Serial.println(FirmwareUpdateState);

  // Update Firmware Button
  if (!FirmwareUpdateState != FirmwareUpdateLastState)
  {
    // reset the debouncing timer
    FirmwareUpdateLastDebounceTime = millis();
    FirmwareUpdateLastState = true;
    FirmwareUpdateNow = true;
  }
  if ((millis() - FirmwareUpdateLastDebounceTime) > debounceDelay)
  {
    if (FirmwareUpdateNow)
    {
      Serial.println("This is where we should update the firmware");
    }
  }
}

void toggleRelay1()
{
  if (!RelayState1 == HIGH)
  {
    digitalWrite(Relay1, LOW);
    RelayState1 = HIGH;
    Serial.println("Relay 1 on");
  }
  else
  {
    digitalWrite(Relay1, HIGH);
    RelayState1 = LOW;
    Serial.println("Relay 1 off");
  }
}

void setRelay1(bool OnOff)
{
  if (OnOff)
  {
    digitalWrite(Relay1, LOW);
    RelayState1 = HIGH;
    Serial.println("Relay 1 on");
  }
  else
  {
    digitalWrite(Relay1, HIGH);
    RelayState1 = LOW;
    Serial.println("Relay 1 off");
  }
}

void toggleRelay2()
{
  if (!RelayState2 == HIGH)
  {
    digitalWrite(Relay2, LOW);
    RelayState2 = HIGH;
    Serial.println("Relay 2 on");
  }
  else
  {
    digitalWrite(Relay2, HIGH);
    RelayState2 = LOW;
    Serial.println("Relay 2 off");
  }
}

void setRelay2(bool OnOff)
{
  if (OnOff)
  {
    digitalWrite(Relay2, LOW);
    RelayState2 = HIGH;
    Serial.println("Relay 2 on");
  }
  else
  {
    digitalWrite(Relay2, HIGH);
    RelayState2 = LOW;
    Serial.println("Relay 2 off");
  }
}

void toggleRelay3()
{
  if (!RelayState3 == HIGH)
  {
    digitalWrite(Relay3, LOW);
    RelayState3 = HIGH;
    Serial.println("Relay 3 on");
  }
  else
  {
    digitalWrite(Relay3, HIGH);
    RelayState3 = LOW;
    Serial.println("Relay 3 off");
  }
}

void setRelay3(bool OnOff)
{
  if (OnOff)
  {
    digitalWrite(Relay3, LOW);
    RelayState3 = HIGH;
    Serial.println("Relay 3 on");
  }
  else
  {
    digitalWrite(Relay3, HIGH);
    RelayState3 = LOW;
    Serial.println("Relay 3 off");
  }
}

void toggleRelay4()
{
  if (!RelayState4 == HIGH)
  {
    digitalWrite(Relay4, LOW);
    RelayState4 = HIGH;
    Serial.println("Relay 4 on");
  }
  else
  {
    digitalWrite(Relay4, HIGH);
    RelayState4 = LOW;
    Serial.println("Relay 4 off");
  }
}

void setRelay4(bool OnOff)
{
  if (OnOff)
  {
    digitalWrite(Relay4, LOW);
    RelayState4 = HIGH;
    Serial.println("Relay 4 on");
  }
  else
  {
    digitalWrite(Relay4, HIGH);
    RelayState4 = LOW;
    Serial.println("Relay 4 off");
  }
}

void buttonloop()
{

  ButtonState1 = digitalRead(ButtonPin1);
  ButtonState2 = digitalRead(ButtonPin2);
  ButtonState3 = digitalRead(ButtonPin3);
  ButtonState4 = digitalRead(ButtonPin4);

  // Button 1
  if (!ButtonState1 != LastButtonState1)
  {
    // reset the debouncing timer
    lastDebounceTime1 = millis();
    LastButtonState1 = true;
    ButtonSwitchNow1 = true;
  }
  if ((millis() - lastDebounceTime1) > debounceDelay)
  {
    
    if (ButtonSwitchNow1)
    {
      ButtonSwitchNow1 = false;
      toggleRelay1();
    }
  }

  // Button 2
  if (!ButtonState2 != LastButtonState2)
  {
    // reset the debouncing timer
    lastDebounceTime2 = millis();
    LastButtonState2 = true;
    ButtonSwitchNow2 = true;
  }
  if ((millis() - lastDebounceTime2) > debounceDelay)
  {
    if (ButtonSwitchNow2)
    {
      ButtonSwitchNow2 = false;
      toggleRelay2();
    }
  }

  // Button 3
  if (!ButtonState3 != LastButtonState3)
  {
    // reset the debouncing timer
    lastDebounceTime3 = millis();
    LastButtonState3 = true;
    ButtonSwitchNow3 = true;
  }
  if ((millis() - lastDebounceTime3) > debounceDelay)
  {
    if (ButtonSwitchNow3)
    {
      ButtonSwitchNow3 = false;
      toggleRelay3();
    }
  }

  // Button 4
  if (!ButtonState4 != LastButtonState4)
  {
    // reset the debouncing timer
    lastDebounceTime4 = millis();
    LastButtonState4 = true;
    ButtonSwitchNow4 = true;
  }
  if ((millis() - lastDebounceTime4) > debounceDelay)
  {
    if (ButtonSwitchNow4)
    {
      ButtonSwitchNow4 = false;
      toggleRelay4();
    }
  }
}

void SetupWebServer()
{

  server.onNotFound([](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html, processor);
  });
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", html, processor);
  });
 
    server.on("/01/on", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay1(true);
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/01/off", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay1(false);
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/02/on", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay2(true);
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/02/off", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay2(false);
    request->send_P(200, "text/html", html, processor);
  });

      server.on("/03/on", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay3(true);
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/03/off", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay3(false);
    request->send_P(200, "text/html", html, processor);
  });

      server.on("/04/on", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay4(true);
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/04/off", HTTP_GET, [](AsyncWebServerRequest *request){
    setRelay4(false);
    request->send_P(200, "text/html", html, processor);
  }); 
  server.begin();
}



String processor(const String& var)
{
 
 // Serial.println(var);
  String match ;
  String title ;
  String label ;
  String link ; 
  String button ;
 // buttonon buttonoff
    
  if(var == "BUTTON_1"){
    
    title = "Main light";
    if (RelayState1)
    {
      label = "On" ;
      link = "/01/off";
      button = "button-on"; 
    }
    else
    {
      label = "Off" ;
      link = "/01/on";
      button = "button-off"; 
    }
            
    match = "<h2>" + title + "</h2>"
    "<p><a href=\""+ link+ "\"><button class=\""+ button +"\">"+label+"</button></a></p>" ;
    
    return String(match);
  }
 
  else if(var == "BUTTON_2"){
    title = "Bed light";
    if (RelayState2)
    {
      label = "On" ;
      link = "/02/off";
      button = "button-on"; 
    }
    else
    {
      label = "Off" ;
      link = "/02/on";
      button = "button-off"; 
    }
    
    match = "<h2>" + title + "</h2>"
    "<p><a href=\""+ link+ "\"><button class=\""+ button +"\">"+label+"</button></a></p>" ;
    
    return String(match);
  }

   else if(var == "BUTTON_3"){
    title = "Tool Box";
    if (RelayState3)
    {
      label = "On" ;
      link = "/03/off";
      button = "button-on"; 
    }
    else
    {
      label = "Off" ;
      link = "/03/on";
      button = "button-off"; 
    }
    
    match = "<h2>" + title + "</h2>"
    "<p><a href=\""+ link+ "\"><button class=\"button "+ button +"\">"+label+"</button></a></p>" ;
    
    return String(match);
  }

    else if(var == "BUTTON_4"){
    title = "Night light";
    if (RelayState4)
    {
      label = "On" ;
      link = "/04/off";
      button = "button-on"; 
    }
    else
    {
      label = "Off" ;
      link = "/04/on";
      button = "button-off"; 
    }
    
    match = "<h2>" + title + "</h2>"
    "<p><a href=\""+ link+ "\"><button class=\"button "+ button +"\">"+label+"</button></a></p>" ;
    
    return String(match);
  }
  return String();
}
