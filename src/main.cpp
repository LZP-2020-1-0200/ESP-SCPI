#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

WiFiEventHandler gotIpEventHandler;
void onWiFiEventStationModeGotIP(const WiFiEventStationModeGotIP &evt)
{
  static bool run_once = true;
  if (run_once)
  {
    run_once = false;
    Serial.print("Station connected, IP: ");
    Serial.println(WiFi.localIP());
    Serial.flush();
    Serial.swap();
    Serial.write("*IDN?\r\n");
  }
}

#define SSID "TEST_SSID"
#define STAPSK "********"

ESP8266WebServer server(80);

void handleCmd()
{

  for (uint8_t i = 0; i < server.args(); i++)
  {
    // message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    if (strncasecmp("cmd", server.argName(i).c_str(), 3) == 0)
    {
      Serial.println(server.arg(i));
    }
  }

  char buffer[1024] = {0};
  int n = 0;
  for (uint32 start_time = system_get_time(), delta = 0; delta < 1000000; delta = system_get_time() - start_time)
  {
    if (Serial.available())
    {
      buffer[n++] = Serial.read();
    }
  }

  //  String message = "atbilde";
  //  ;
  server.send(200, "text/plain", buffer);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600, SerialConfig::SERIAL_8N1, SerialMode::SERIAL_FULL);
  Serial.println("Hello !");

  gotIpEventHandler = WiFi.onStationModeGotIP(&onWiFiEventStationModeGotIP);

  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, STAPSK);

  ArduinoOTA.begin();
  server.on("/cmd", HTTP_POST, handleCmd);

  // curl -d "cmd=MEAS:VOLT 3&timeout=1" -H "Content-Type: application/x-www-form-urlencoded" http://192.168.6.122/cmd
  /*
  import requests

  url = 'http://192.168.6.122/cmd'
  headers = {'Content-type': 'application/x-www-form-urlencoded'}
  r = requests.post(url, data="cmd=:*IDN?", headers=headers)
  print(r.text)

  r = requests.post(url, data="cmd=:MEAS:VOLT?", headers=headers)
  print(r.text)
  */

  server.begin();
}

void loop()
{
  // put your main code here, to run repeatedly:
  server.handleClient();
  ArduinoOTA.handle();
}
