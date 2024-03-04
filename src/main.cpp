#include <Arduino.h>

#include <ArduinoOTA.h>
#include <Wifi.h>
#include <WiFiUDP.h>

#include <ld2410.h>

ld2410 radar;

const char *ssid = "TRNNET-2G";
const char *password = "ripcord1";
const char *hostname = "ESP_ld2410c";

WiFiUDP Udp;

uint32_t lastReading = 0;

void connect()
{
  WiFi.mode(WIFI_STA);

  WiFi.hostname(hostname);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  // MDNS.begin("esp123");

  Serial.println(WiFi.localIP());

  ArduinoOTA.begin();
}


void setup(void)
{
  Serial.begin(115200); //Feedback over Serial Monitor
  //radar.debug(Serial); //Uncomment to show debug information from the library on the Serial Monitor. By default this does not show sensor reads as they are very frequent.
  Serial1.begin (115200, SERIAL_8N1, 33, 23); //UART for monitoring the radar
  delay(500);
  Serial.print(F("\nLD2410 radar sensor initialising: "));
  if(radar.begin(Serial1))
  {
    Serial.println(F("OK"));
  }
  else
  {
    Serial.println(F("not connected"));
  }

  connect();
}

void loop()
{
  radar.read();
  if(radar.isConnected() && millis() - lastReading > 100)  //Report every 1000ms
  {
    lastReading = millis();
    if(radar.presenceDetected())
    {
      if(radar.stationaryTargetDetected())
      {
       // Serial.print(F("Stationary target: "));
       // Serial.print(radar.stationaryTargetDistance());
       // Serial.print(F("cm energy:"));
       // Serial.println(radar.stationaryTargetEnergy());
      }
      if(radar.movingTargetDetected())
      {
        Udp.beginPacket("192.168.1.255", 15001);
        Udp.printf("%u\t%u\n",radar.movingTargetDistance(),radar.movingTargetEnergy());
        Udp.endPacket();

       // Serial.print(F("Moving target: "));
       // Serial.print(radar.movingTargetDistance());
       // Serial.print(F("cm energy:"));
       // Serial.println(radar.movingTargetEnergy());
      }
    }
    else
    {
      //Serial.println(F("No target"));
    }
  }

  ArduinoOTA.handle();
}