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
#include <Preferences.h>

/* create an instance of Preferences library */
Preferences preferences;

#include <esp32fota.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "credentials.h"
#include "relay.h"
#include "html.h"

// Add to 'credentials.h'
// WiFi credentials for home network
// const char* ssid = ".....";
// const char* password = ".....";
// 
// const char* APssid = ".......";
// const char* APpassword = ".......";
//
// String fotaURL = "";

// MQTT server on PI
// const char* mqtt_server = ".....";


bool wifiIsAP ;

// Set web server port number to 80
AsyncWebServer server(80);


// Data wire is plugged into port 0 on the Arduino
#define ONE_WIRE_BUS 0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
unsigned long TemperatureUpdate = 0;
float temperature = 0;

int FirmwareUpdatePin = 27;
bool FirmwareUpdateState = LOW;
bool FirmwareUpdateLastState = LOW;
bool FirmwareUpdateNow = false;
unsigned long FirmwareUpdateLastDebounceTime = 0;

esp32FOTA esp32FOTA("main-controller-ttgo.bin", 1);

void setup()
{
  preferences.begin("my-app", false);
  Serial.begin(115200);
  Serial.println();

  // Start up the Dallas Temperature Lib
  sensors.begin();
  
  wifiIsAP = preferences.getBool("wifiIsAP",true);
  Serial.printf("wifiIsAP value: %d\n", wifiIsAP);
  Serial.print("wifiIsAP : ");
  Serial.println(wifiIsAP);
  
  pinMode(ButtonPin1, INPUT_PULLUP);
  pinMode(ButtonPin2, INPUT_PULLUP);
  pinMode(ButtonPin3, INPUT_PULLUP);
  pinMode(ButtonPin4, INPUT_PULLUP);
  pinMode(FirmwareUpdatePin, INPUT_PULLUP);

  pinMode(Relay1, OUTPUT);
  pinMode(Relay2, OUTPUT);
  pinMode(Relay3, OUTPUT);
  pinMode(Relay4, OUTPUT);

  RelayState1 = preferences.getBool("RelayState1",false);
  RelayState2 = preferences.getBool("RelayState2",false);
  RelayState3 = preferences.getBool("RelayState3",false);
  RelayState4 = preferences.getBool("RelayState4",false);

  Serial.printf("RelayState1 value: %d\n", RelayState1);
  Serial.printf("RelayState2 value: %d\n", RelayState2);
  Serial.printf("RelayState3 value: %d\n", RelayState3);
  Serial.printf("RelayState4 value: %d\n", RelayState4);
  
  digitalWrite(Relay1, !RelayState1);
  digitalWrite(Relay2, !RelayState2);
  digitalWrite(Relay3, !RelayState3);
  digitalWrite(Relay4, !RelayState4);

  esp32FOTA.checkURL = fotaURL;

  if(wifiIsAP) {
    Serial.println("Setting wifiIsAP to true");
    Serial.println("Start as AP");
    setup_wifi_AP();
  } 
  else 
  {
    Serial.println("Setting wifiIsAP to false");
    Serial.println("Start as client");
    setup_wifi_station();
  }
  delay(100);

//  //ESP32 As access point IP: 192.168.4.1
//  WiFi.mode(WIFI_AP); //Access Point mode
//  WiFi.softAP("ESPWebServer", "12345678");    //Password length minimum 8 char
// 
////ESP32 connects to your wifi -----------------------------------
//  WiFi.mode(WIFI_STA); //Connectto your wifi
//  WiFi.begin(ssid, password);
// 
//  Serial.println("Connecting to ");
//  Serial.print(ssid);
// 
//  //Wait for WiFi to connect
//  while(WiFi.waitForConnectResult() != WL_CONNECTED){      
//      Serial.print(".");
//      delay(1000);
//    }

    
  setupWebServer() ;

}

void setup_wifi_AP() {
  delay(10);
  Serial.print("Setting up AP : ");
  Serial.println(APssid);
  Serial.println(APpassword);
  WiFi.mode(WIFI_MODE_NULL);
  delay(50);
  WiFi.mode(WIFI_AP);
  delay(50);
  WiFi.softAPConfig(IPAddress(10, 1, 1, 1), IPAddress(10, 1, 1, 1), IPAddress(255, 255, 255, 0));
  delay(50);
  //WiFi.softAP("ESPWebServer", "12345678");
  WiFi.softAP(APssid, APpassword);
  wifiIsAP = true ;
  preferences.putBool("wifiIsAP",true);
  Serial.println("");
  Serial.println(WiFi.softAPIP());
}

void setup_wifi_station()
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
  wifiIsAP = false ;
  preferences.putBool("wifiIsAP",false);
  Serial.println("");
  Serial.println(WiFi.localIP());


  delay(250);
  bool updatedNeeded = esp32FOTA.execHTTPcheck();
  if (updatedNeeded)
  {
    preferences.end();
    esp32FOTA.execOTA();
  }
  
}

void wifi_toggle()
{
   if(wifiIsAP) {
    Serial.println("Setting wifiIsAP to false");
    preferences.putBool("wifiIsAP",false);
    wifiIsAP = false ;
    delay(250);
  } 
  else {
    Serial.println("Setting wifiIsAP to true");
    preferences.putBool("wifiIsAP",true);
    wifiIsAP = true ;
    delay(250);
  }

  wifiIsAP = preferences.getBool("wifiIsAP",true);
  Serial.printf("wifiIsAP value: %d\n", wifiIsAP);
  
  preferences.end();
  delay(500);
  ESP.restart();
}


void set_wifi_type(bool as_AP)
{
   if(as_AP) {
    Serial.println("Setting wifiIsAP to true");
    preferences.putBool("wifiIsAP",true);
    wifiIsAP = true ;
    delay(250);
  } 
  else {
    Serial.println("Setting wifiIsAP to false");
    preferences.putBool("wifiIsAP",false);
    wifiIsAP = false ;
    delay(250);
  }

  wifiIsAP = preferences.getBool("wifiIsAP",true);
  Serial.printf("wifiIsAP value: %d\n", wifiIsAP);
  
  preferences.end();
  delay(500);
  ESP.restart();
}

void loop()
{
  buttonloop();
  firmwareloop();
  temperatureloop();
}

  
void firmwareloop()
{
  FirmwareUpdateState = digitalRead(FirmwareUpdatePin);
  
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
      FirmwareUpdateNow = false;
      wifi_toggle() ;
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
   preferences.putBool("RelayState1",RelayState1);
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
  preferences.putBool("RelayState1",RelayState1);
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
  preferences.putBool("RelayState2",RelayState2);
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
  preferences.putBool("RelayState2",RelayState2);
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
  preferences.putBool("RelayState3",RelayState3);
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
  preferences.putBool("RelayState3",RelayState3);
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
  preferences.putBool("RelayState4",RelayState4);
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
  preferences.putBool("RelayState4",RelayState4);
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

void setupWebServer()
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

    server.on("/wifi/station", HTTP_GET, [](AsyncWebServerRequest *request){
    wifi_toggle() ;
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/wifi/ap", HTTP_GET, [](AsyncWebServerRequest *request){
    wifi_toggle() ;
    request->send_P(200, "text/html", html, processor);
  });

    server.on("/fw/update", HTTP_GET, [](AsyncWebServerRequest *request){
    set_wifi_type(false);
    request->send_P(200, "text/html", html, processor);
  });

  
  server.begin();
}



String processor(const String& var)
{

  String match ;
  String title ;
  String label ;
  String link ; 
  String button ;
    
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
else if(var == "MODE"){
    title = "Night light";
    if (wifiIsAP)
    {
      label = "wifi AP" ;
      link = "/wifi/station";
      button = "button-ap"; 
    }
    else
    {
      label = "wifi Station" ;
      link = "/wifi/ap";
      button = "button-station"; 
    }
    
    match = "<a href=\""+ link+ "\"><button class=\"button "+ button +"\">"+label+"</button></a>" ;
    
    return String(match);
  }
 else if(var == "UPDATE"){

      label = "wifi AP" ;
      link = "/fw/update";
      button = "button-fw"; 

    match = "<a href=\""+ link+ "\"><button class=\"button "+ button +"\">"+label+"</button></a>" ;
    
    return String(match);
  }

  else if(var == "TEMPERATURE"){
    char tempString[8];
    dtostrf(temperature, 1, 2, tempString);   
    match = "<h2>temperature : " + String(tempString)  + "</h2>" ;
    
    return String(match);
  }



  return String();
}


void temperatureloop() {
  if ((millis() - TemperatureUpdate) > 5000) 
  {
    sensors.requestTemperatures();
    temperature =  sensors.getTempCByIndex(0) ;
    TemperatureUpdate = millis() ;
  }
}
