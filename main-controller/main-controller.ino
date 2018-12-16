/**
   esp32 firmware OTA

   0
   1
   2
   3
   4
   5
   6
   7
   8
   9
  10
  11
  12  Relay3
  13  Relay2  
  14  Relay4
  15  Relay1
  16
  17
  18
  19
  20
  21  OnBoard LED
  22
  23
  24
  25  ButtonPin2
  26  ButtonPin1
  27  HomeNetwork for FOTA
  28
  29
  30
  31
  32  ButtonPin4
  33  ButtonPin3
  34
  35


*/
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include <esp32fota.h>
#include <WiFi.h>

#include "credentials.h"
#include "relay.h"

// Add to 'credentials.h'
// WiFi credentials for home network
// const char* ssid = ".....";
// const char* password = ".....";

// MQTT server on PI
// const char* mqtt_server = ".....";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

int FirmwareUpdatePin;
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

  digitalWrite(Relay1, RelayState1);
  digitalWrite(Relay2, RelayState2);
  digitalWrite(Relay3, RelayState3);
  digitalWrite(Relay4, RelayState4);

  //esp32FOTA.checkURL = "http://server/fota/fota.json";
  Serial.begin(115200);
  setup_wifi();
  server.begin();
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

  WiFiClient client = server.available(); // Listen for incoming clients

  if (client)
  {                                // If a new client connects,
    Serial.println("New Client."); // print a message out in the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /01/on") >= 0)
            {
              setRelay1(bool true)
            }
            else if (header.indexOf("GET /01/off") >= 0)
            {
              setRelay1(bool false)
            }
            else if (header.indexOf("GET /02/on") >= 0)
            {
              setRelay2(bool true)
            }
            else if (header.indexOf("GET /02/off") >= 0)
            {
              setRelay2(bool false)
            }
            else if (header.indexOf("GET /03/on") >= 0)
            {
              setRelay3(bool true)
            }
            else if (header.indexOf("GET /03/off") >= 0)
            {
              setRelay3(bool false)
            }
            else if (header.indexOf("GET /04/on") >= 0)
            {
              setRelay4(bool true)
            }
            else if (header.indexOf("GET /04/off") >= 0)
            {
              setRelay4(bool false)
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");

            // Web Page Heading
            client.println("<body><h1>Camper Web Server</h1>");

            String state;
            // Display current state, and ON/OFF buttons for Relay 1
            if (RelayState1)
            {
              state = "On";
            }
            else
            {
              state = "Off";
            }
            client.println("<h2>RelayState1 - State " + state + "</h2>");
            // If the RelayState1 is off, it displays the ON button
            if (RelayState1 == LOW)
            {
              client.println("<p><a href=\"/01/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/01/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for Relay 2
            if (RelayState2)
            {
              state = "On";
            }
            else
            {
              state = "Off";
            }
            client.println("<h2>RelayState2 - State " + state + "</h2>");
            // If the output27State is off, it displays the ON button
            if (RelayState2 == LOW)
            {
              client.println("<p><a href=\"/02/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/02/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for Relay 3
            if (RelayState3)
            {
              state = "On";
            }
            else
            {
              state = "Off";
            }
            client.println("<h2>RelayState3 - State " + state + "</h2>");
            // If the output27State is off, it displays the ON button
            if (RelayState3 == LOW)
            {
              client.println("<p><a href=\"/03/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/03/off\"><button class=\"button button2\">OFF</button></a></p>");
            }

            // Display current state, and ON/OFF buttons for Relay 4
            if (RelayState4)
            {
              state = "On";
            }
            else
            {
              state = "Off";
            }
            client.println("<h2>RelayState4 - State " + state + "</h2>");
            // If the output27State is off, it displays the ON button
            if (RelayState4 == LOW)
            {
              client.println("<p><a href=\"/04/on\"><button class=\"button\">ON</button></a></p>");
            }
            else
            {
              client.println("<p><a href=\"/04/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  buttonloop();
  firmwareloop();
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